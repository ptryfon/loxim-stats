#include <stdio.h>
#include <string>
#include <vector>

#include "QueryResult.h"
#include "RemoteExecutor.h"
#include "../QueryParser/ClassNames.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "QueryExecutor.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor {

EnvironmentStack::EnvironmentStack() { actualBindClassLid = NULL; actual_prior = 0; sectionDBnumber = 0; ec = new ErrorConsole("QueryExecutor"); }
EnvironmentStack::~EnvironmentStack() { this->deleteAll(); if (ec != NULL) delete ec; }

unsigned int EnvironmentStack::getSectionDBnumber() { return sectionDBnumber; }

int EnvironmentStack::push(QueryBagResult *r, Transaction *&tr, QueryExecutor *qe) {
	int errcode;
	
	unsigned bagSize = r->size();
	
	for(unsigned int i = 0; i < bagSize; i++) {
		QueryResult* qr;
		errcode = r->at(i, qr);
		if(errcode != 0) return errcode;
		if(qr->type() == QueryResult::QBINDER) {
			if(qr->isReferenceValue()) {
				QueryResult* useless;
				errcode = qr->getReferenceValue(useless);
				if(errcode != 0) return errcode;
				QueryReferenceResult* ref = (QueryReferenceResult*)useless;
				stringHashSet shs;
				errcode = qe->getCg()->fetchExtInvariantNamesForLid(ref->getValue(), tr, qe, shs);
				if(errcode != 0) return errcode;
				for(stringHashSet::iterator j = shs.begin(); j != shs.end(); ++j) {
					r->addResult( new QueryBinderResult(*j, ref->clone()) );
				}
			}
		}
	}
	*ec << r->toString(0, true);
	es.push_back(r);
	es_priors.push_back(actual_prior);
	*ec << "[QE] Environment Stack pushed";
	return 0;
}

int EnvironmentStack::pop(){
	if (es.empty()) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	if (es.size() == sectionDBnumber) {
		*ec << "error: trying to pop() Data Base section";
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	// delete ??
	unsigned int popedSectionNo = es.size();
	SectionToClassMap::iterator actClasses = classesPerSection.find(popedSectionNo);
	if(actClasses != classesPerSection.end()) {
		delete (*actClasses).second;
		classesPerSection.erase(actClasses);
	}
	es.pop_back();
	es_priors.pop_back();
	*ec << "[QE] Environment Stack popped";
	return 0;
}

int EnvironmentStack::pushDBsection() {
	if (sectionDBnumber != 0) {
		*ec << "error: DBsection already pushed, can't be pushed once more";
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	QueryResult *r = new QueryBagResult();
	es.push_back((QueryBagResult *)r);
	es_priors.push_back(actual_prior);
	sectionDBnumber = es.size();
	*ec << "[QE] Data Base section pushed on Environment Stack";
	return 0;
}

int EnvironmentStack::popDBsection() {
	if (sectionDBnumber == 0) {
		*ec << "error: DBsection wasn't pushed, can't be poped";
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	if (es.empty()) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	if (sectionDBnumber > es.size()) {
		*ec <<"error: DBsection number is greater then environment stack size";
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	while (sectionDBnumber <= es.size()) {
		// delete ??
		es.pop_back();
		es_priors.pop_back();
	}	
	sectionDBnumber = 0;
	*ec << "[QE] Data Base section popped from Environment Stack";
	return 0;
}

int EnvironmentStack::top(QueryBagResult *&r) {
	if (es.empty()) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	if (es.size() == sectionDBnumber) {
		*ec << "error: trying to top() Data Base section";
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	r=(es.back());
	return 0;
}

bool EnvironmentStack::empty() { return es.empty(); }
int EnvironmentStack::size() { return es.size(); }



int EnvironmentStack::bindName(string name, int sectionNo, Transaction *&tr, QueryExecutor *qe, QueryResult *&r) {
	int errcode;
	*ec << "[QE] Name binding on ES";
	*ec << toString() ;
	unsigned int number = (es.size());
	ec->printf("[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	bool found_one = false;
	QueryBagResult *section;
	unsigned int sectionSize;
	QueryResult *sth;
	string current;
	for (unsigned int i = number; i >= 1; i--) {
		if (es_priors.at(i - 1) == actual_prior) {
			if (i == sectionDBnumber) {
				ec->printf("[QE] bindName: ES section %u is Data Base section\n", i);
				vector<LogicalID*>* vec;
				if ((errcode = tr->getRootsLID(name, vec)) != 0) {
					*ec << "[QE] bindName - error in getRootsLID";
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize = vec->size();
				ec->printf("[QE] %d Roots LID by name taken\n", vecSize);
				
				vector<LogicalID*>* vec_virt;
				if ((errcode = tr->getViewsLID(name, vec_virt)) != 0) {
					*ec << "[QE] bindName - error in getViewsLID";
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize_virt = vec_virt->size();
				ec->printf("[QE] %d Views LID by name taken\n", vecSize_virt);
				
				vector<LogicalID*>* vec_sysvirt;
				if ((errcode = tr->getSystemViewsLID(name, vec_sysvirt)) != 0) {
					*ec << "[QE] bindName - error in getSystemViewsLID";
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize_sysvirt = vec_sysvirt->size();
				ec->printf("[QE] %d SystemViews LID by name taken\n", vecSize_sysvirt);
				
				if (vecSize_virt == 0 && vecSize_sysvirt == 0) { 
					for (int i = 0; i < vecSize; i++ ) {
						found_one = true;
						LogicalID *lid = vec->at(i);
						QueryReferenceResult *lidres = new QueryReferenceResult(lid);
						r->addResult(lidres);
					}
					ClassGraph* cg = qe->getCg();
					stringHashSet shs;
					cg->fetchSubInvariantNames(name, tr, qe, shs);
					vector< vector<LogicalID*>* > vecvec;
					for(stringHashSet::iterator i = shs.begin(); i != shs.end(); i++) {
						vector<LogicalID*>* vecSubInvariant;
						if ((errcode = tr->getRootsLID((*i), vecSubInvariant)) != 0) {
							return qe->trErrorOccur("[QE] bindName - error in getRootsLID", errcode);
						}
						vecvec.push_back(vecSubInvariant);
					}
					for(unsigned int i = 0; i < vecvec.size(); ++i) {
						vector<LogicalID*>* vecSubInvariant = vecvec.at(i);
						for (unsigned int j = 0; j < vecSubInvariant->size(); j++ ) {
							LogicalID *lid = vecSubInvariant->at(j);
							bool inInvariant;
							errcode = cg->belongsToInvariant(lid, name, tr, qe, inInvariant);
							if(errcode != 0) return errcode;
							if(!inInvariant) continue;
							found_one = true;
							QueryReferenceResult *lidres = new QueryReferenceResult(lid);
							r->addResult(lidres);
						}
						delete vecSubInvariant;
					}
				}
				else if (vecSize != 0) {
					if (vecSize_virt != 0) {
						*ec << "[QE] bindName error: Real and virtual objects have the sam name";
					} else {
						*ec << "[QE] bindName error: Real and system view objects have the sam name";
					}
					*ec << (ErrQExecutor | EBadBindName);
					return ErrQExecutor | EBadBindName;
				} else if(vecSize_virt != 0 && vecSize_sysvirt != 0) {
					*ec << "[QE] bindName error: virtual objects and system view objects have the sam name";
					*ec << (ErrQExecutor | EBadBindName);
					return ErrQExecutor | EBadBindName;
				} else if (vecSize_virt > 1) {
					*ec << "[QE] bindName error: Multiple views defining virtual objects with the same name";
					*ec << (ErrQExecutor | EBadBindName);
					return ErrQExecutor | EBadBindName;
				}
				else if (vecSize_sysvirt > 1) {
					*ec << "[QE] bindName error: Multiple system views defining objects with the same name";
					*ec << (ErrQExecutor | EBadBindName);
					return ErrQExecutor | EBadBindName;
				}
				else {
					if (vecSize_virt == 1) {
						LogicalID *view_lid = vec_virt->at(0);
						string view_code;
						errcode = qe->checkViewAndGetVirtuals(view_lid, name, view_code);
						if (errcode != 0) return errcode;
						
						vector<QueryBagResult*> envs_sections;
						QueryResult *tmp_View_LID_result = new QueryReferenceResult(view_lid);
						errcode = tmp_View_LID_result->nested(tr, qe);
						if(errcode != 0) return errcode;
						QueryBagResult *nested_tmp_View_LID_result;
						errcode = qe->getEnvs()->top(nested_tmp_View_LID_result);
						if(errcode != 0) return errcode;
						errcode = qe->getEnvs()->pop();
						if(errcode != 0) return errcode;
						envs_sections.push_back((QueryBagResult *) nested_tmp_View_LID_result);
						
						errcode = qe->callProcedure(view_code, envs_sections);
						if(errcode != 0) return errcode;
						
						QueryResult *res;
						qe->pop_qres(res);
						QueryResult *bagged_res = new QueryBagResult();
						((QueryBagResult *) bagged_res)->addResult(res);
						for (unsigned int i = 0; i < bagged_res->size(); i++) {
							found_one = true;
							QueryResult *seed;
							errcode = ((QueryBagResult *) bagged_res)->at(i, seed);
							if (errcode != 0) return errcode;
							vector<QueryResult *> seeds;
							seeds.push_back(seed);
							QueryResult *virt_res = new QueryVirtualResult(name, view_lid, seeds);
							r->addResult(virt_res);
						}
					} else {
						// Galaz dla wirtualnych widokow�
						*ec << "[QE] TUTAJ WYNIKI";
						found_one = true;
						LogicalID *svlid = vec_sysvirt->at(0);
						QueryReferenceResult *sysvirtres = new QueryReferenceResult(svlid);
						r->addResult(sysvirtres);
					}
				}
				
				delete vec;
				delete vec_virt;
			}
			else {
				section = (es.at(i - 1));
				sectionSize = (section->size());
				ec->printf("[QE] bindName: ES section %u got %u elements\n", i, sectionSize);
				for (unsigned int j = 0; j < sectionSize; j++) {
					errcode = (section->at(j,sth));
					if (errcode != 0) return errcode;
					if ((sth->type()) == (QueryResult::QBINDER)) {
						current = (((QueryBinderResult *) sth)->getName());
						ec->printf("[QE] bindName: current %u name is: %s\n", j, current.c_str());
						if (current == name) {
							found_one = true;
							*ec << "[QE] bindName: Object added to Result";
							r->addResult(((QueryBinderResult *) sth)->getItem());
						}
					}
				}
				
			}
		}
		if (found_one) {
			ec->printf("[QE] bindName: bind at %d section, Parser said it should be binded at %d\n", i - 1, sectionNo);
			return 0;
		}
	}
	ec->printf("[QE] bindName: name not binded on ES, Parser said it should be binded at %d\n", sectionNo);
	return 0;
}


int EnvironmentStack::bindProcedureName(string name, unsigned int queries_size, Transaction *&tr, QueryExecutor *qe, string &code, vector<string> &params, int &bindSectionNo, LogicalID*& bindClassLid) {
	*ec << "[QE] Procedure Name binding on ES";
	int errcode;
	unsigned int envs_size = (es.size());
	int founded = 0;
	bindSectionNo = -1;
	bindClassLid = NULL;
	
	for (unsigned int i = envs_size; i >= 1; i--) {
		if (es_priors.at(i - 1) == actual_prior) {
			if (i == sectionDBnumber) {
				ec->printf("[QE] bindProcedureName: ES section %u is Data Base section\n", i);
				vector<LogicalID*>* vec;
				if ((errcode = tr->getRootsLID(name, vec)) != 0) {
					*ec << "[QE] bindName - error in getRootsLID";
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize = vec->size();
				ec->printf("[QE] %d Roots LID by name taken\n", vecSize);
				for (int i = 0; i < vecSize; i++ ) {
					LogicalID *lid = vec->at(i);
					errcode = qe->procCheck(queries_size, lid, code, params, founded);
					if (errcode != 0) return errcode;
				}
				delete vec;
			}
			else {
				QueryBagResult *section = (es.at(i - 1));
				unsigned int sectionSize = (section->size());
				ec->printf("[QE] bindProcedureName: ES section %u got %u elements\n", i, sectionSize);
				for (unsigned int j = 0; j < sectionSize; j++) {
					QueryResult *sth;
					errcode = (section->at(j,sth));
					if (errcode != 0) return errcode;
					if ((sth->type()) == (QueryResult::QBINDER)) {
						string current = (((QueryBinderResult *) sth)->getName());
						ec->printf("[QE] bindProcedureName: current %u name is: %s\n", j, current.c_str());
						if (current == name) {
							QueryResult *item = (((QueryBinderResult *) sth)->getItem());
							if (item->isReferenceValue()) {
								QueryResult *ref_res;
								errcode = item->getReferenceValue(ref_res);
								if (errcode != 0) return errcode;
								LogicalID *lid = ((QueryReferenceResult*)ref_res)->getValue();
								errcode = qe->procCheck(queries_size, lid, code, params, founded);
								if (errcode != 0) return errcode;
							}
						}
					}
				}
				if(founded <= 0) {
					SectionToClassMap::iterator cpsI = classesPerSection.find(i);
					if(cpsI != classesPerSection.end()) {
						errcode = qe->getCg()->findMethod(name, queries_size, (*cpsI).second, code, params, founded, actualBindClassLid, bindClassLid);
					}
				}
			}
		}
		if (founded > 0) {
			ec->printf("[QE] bindProcedureName: name binded at %d section of envs\n", i - 1);
			bindSectionNo = i - 1;
			return 0;
		}
	}
	ec->printf("[QE] bindProcedureName: name not binded on ES\n");
	
	return 0;
}

int EnvironmentStack::pushClasses(DataValue* dv, QueryExecutor *qe, bool& classFound) {
	//klasy nie sa obiektami innych klas
	if(dv->getSubtype() == Store::Class) {
		return 0;
	}
	SetOfLids* classMarks = dv->getClassMarks(); 
	if(classMarks == NULL) {
		return 0;
	}
	unsigned int actualSectionNo = es.size();
	if(actualSectionNo == 0) {
		//TODO: error env stack corrupt
		return 1;
	}
	
	SetOfLids* cgClasses = NULL;
	for(SetOfLids::iterator i = classMarks->begin(); i != classMarks->end(); ++i) {
		LogicalID* lid;
		int errcode = qe->getCg()->lidToClassGraphLid(*i, lid);
		if(errcode != 0) {
			if(cgClasses != NULL) {
				delete cgClasses;
			}
			return errcode;
		}
		if(lid != NULL) {
			if(cgClasses == NULL) {
				cgClasses = new SetOfLids();
			}
			classFound = true;
			cgClasses->insert(lid);
		}
	}
	if(cgClasses != NULL) {
		classesPerSection[actualSectionNo] = cgClasses;
	}
	return 0;
}

void EnvironmentStack::deleteAll() {
	for(SectionToClassMap::iterator i = classesPerSection.begin(); i != classesPerSection.end(); ++i) {
		delete (*i).second;
	}  
	for (unsigned int i = 0; i < (es.size()); i++) {
		delete es.at(i);
	};
	es.clear();
	es_priors.clear();
}





// nested function returns bag of binders, which will be pushed on the environment stack

//Zmiana metody nested, teraz dokonuje też wstawienia na stos.
//Refaktoryzacja wykorzystuje zrealizowane wczesniej metody nested trzyargumentowa
//i dodaje w nadklasie metode nested dwoargumentowa, ktora wstawia cos na stos.
//Metody trzyargumentowe zmienione na protected.
//Dodana implementacja pusta metody trzyargumentowej w nadklasie.

int QueryResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	return 0;
}

int QueryResult::nested(Transaction *&tr, QueryExecutor * qe) {
	QueryResult *newStackSection = new QueryBagResult();
	int errcode = this->nested(tr, newStackSection, qe);
	if (errcode != 0) return errcode;
	return qe->getEnvs()->push((QueryBagResult *) newStackSection, tr, qe);
}

int QuerySequenceResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): ERROR! QuerySequenceResult shouldn't be nested";
	*ec << (ErrQExecutor | EOtherResExp);
	return ErrQExecutor | EOtherResExp;
	// nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryBagResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): ERROR! QueryBagResult shouldn't be nested";
	*ec << (ErrQExecutor | EOtherResExp);
	return ErrQExecutor | EOtherResExp;
	// nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryStructResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): QueryStructResult";
	int errcode;
	for (unsigned int i = 0; i < str.size(); i++) {
		if ((str.at(i))->type() == QueryResult::QSTRUCT) {
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp; // one row shouldn't contain another row;
		}
		else {
			errcode = ((str.at(i))->nested(tr, r, qe));
			if (errcode != 0) return errcode;
		}
	}
	return 0;
}

int QueryStringResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): QueryStringResult can't be nested";
	return 0;
}

int QueryIntResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): QueryIntResult can't be nested";
	return 0;
}

int QueryDoubleResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): QueryDoubleResult can't be nested";
	return 0;
}

int QueryBoolResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): QueryBoolResult can't be nested";
	return 0;
}

int QueryNothingResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	*ec << "[QE] nested(): QueryNothingResult can't be nested";
	return 0;
}

int QueryBinderResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	if (item != NULL) {
		QueryBinderResult *tmp_value = new QueryBinderResult(name,item);
		ec->printf("[QE] nested(): QueryBinderResult copy returned name: %s\n", name.c_str());
		r->addResult(tmp_value);
	}
	return 0;
}

int QueryReferenceResult::nested(Transaction *&tr, QueryExecutor * qe) {
	QueryBagResult *r = new QueryBagResult();
	int errcode;
	ec->printf("[QE] nested(): QueryReferenceResult\n");
	/* remote ID */
	if ((value != NULL) && (value->getServer() != "")) {
		*ec << "starting logicalID\n";
		QueryResult* qr;
		RemoteExecutor *rex = new RemoteExecutor(value->getServer(), value->getPort(), qe);
		rex->setLogicalID(value);
		errcode = rex->execute(&qr);
		if (errcode != 0) {
			return errcode;
		}
		r->addResult(qr);
		*ec << "remote logicalID\n";
		return qe->getEnvs()->push(r, tr, qe);
	}
	/* end of remoteID processing */
	
	DataValue* tmp_data_value;
	
	ObjectPointer *optr;
	if (value != NULL) {
		if ((errcode = tr->getObjectPointer(value, Store::Read, optr, false)) != 0) {
			*ec << "[QE] Error in getObjectPointer";
			qe->antyStarveFunction(errcode);
			qe->inTransaction = false;
			return errcode;
		}

		tmp_data_value = optr->getValue();
		
		/* Link */
		if (tmp_data_value->getSubtype() == Store::Link) {
			 *ec << "nested on Link object\n";
			 vector<LogicalID*>*  tmp_vec =tmp_data_value->getVector();
			 int vec_size = tmp_vec->size();
			 int errcode;
			 int port;
			 string ip;
			 ObjectPointer *optr;
			 DataValue* value;
			 for (int i = 0; i < vec_size; i++ ) {
				 LogicalID *tmp_logID = tmp_vec->at(i);
				 if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr, false)) != 0) {
					*ec << "[QE] Error in getObjectPointer";
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				string tmp_name = optr->getName();
				value = optr->getValue();
				switch (value->getType()) {
					case Store::Integer:
						 port = value->getInt();
						 break;
					case Store::String:
						 ip = value->getString();
						 break;
					default:
						*ec << "[QE] incorrect Link object structure";
						qe->antyStarveFunction(errcode);
						qe->inTransaction = false;
						return errcode;
				 }

			 }
			 *ec << "and now bring!!!!!!!!!!! ip: " <<  ip << " port: " << port <<"\n";
			 QueryResult* qr;// = new QueryIntResult(5);
			 RemoteExecutor *rex = new RemoteExecutor(ip, port, qe);
			 errcode = rex->execute(&qr);
			 if (errcode != 0) {
			 	return errcode;
			 }
			 r->addResult(qr);
			 return qe->getEnvs()->push(r, tr, qe);
		}
		/* end of link processing */
		
		vector<LogicalID*> subviews_vector;

		int vType = tmp_data_value->getType();
		switch (vType) {
			case Store::Integer: {
				*ec << "[QE] nested(): QueryReferenceResult pointing integer value - can't be nested";
				break;
			}
			case Store::Double: {
				*ec << "[QE] nested(): QueryReferenceResult pointing double value - can't be nested";
				break;
			}
			case Store::String: {
				*ec << "[QE] nested(): QueryReferenceResult pointing string value - can't be nested";
				break;
			}
			case Store::Pointer: {
				LogicalID *tmp_logID = (tmp_data_value->getPointer());
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr, false)) != 0) {
					*ec << "[QE] Error in getObjectPointer";
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				string tmp_name = optr->getName();
				QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
				QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
				*ec << "[QE] nested(): QueryReferenceResult pointing reference value";
				r->addResult(final_binder);
				ec->printf("[QE] nested(): new QueryBinderResult returned name: %s\n", tmp_name.c_str());
				
				break;
			}
			case Store::Vector: {
				vector<LogicalID*>* tmp_vec = (tmp_data_value->getVector());
				*ec << "[QE] nested(): QueryReferenceResult pointing vector value";
				int vec_size = tmp_vec->size();
				for (int i = 0; i < vec_size; i++ ) {
					LogicalID *tmp_logID = tmp_vec->at(i);
					if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr, false)) != 0) {
						*ec << "[QE] Error in getObjectPointer";
						qe->antyStarveFunction(errcode);
						qe->inTransaction = false;
						return errcode;
					}
					string tmp_name = optr->getName();
					//MH TODO: add a 'this->value' reference to each tmp_logID ?
					tmp_logID->setDirectParent(this->value);
					QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
					QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
					ec->printf("[QE] nested(): vector element number %d\n", i);
					r->addResult(final_binder);
					ec->printf("[QE] nested(): new QueryBinderResult returned name: %s\n", tmp_name.c_str());
					
					DataValue* tdv;
					tdv = optr->getValue();
					if ((tmp_data_value->getSubtype() != Store::View) && (tdv->getType() == Store::Vector) && (tdv->getSubtype() == Store::View)) {
						subviews_vector.push_back(tmp_logID);
					}
				}
				break;
			}
			default : {
				*ec << "[QE] nested(): ERROR! QueryReferenceResult pointing unknown format value";
				*ec << (ErrQExecutor | EUnknownValue);
				return ErrQExecutor | EUnknownValue;
				break;
			}
		}
		QueryBinderResult *selfBinder = new QueryBinderResult(QE_SELF_KEYWORD, this);
		r->addResult(selfBinder);
		
		//DG TODO
		//Gdy obiekt nie bedacy perspektywa mial jako podobiekt perspektywe, wynikiem nested sa tez wirtualne obiekty -- Na razie zakomentowane, by ta funkcjonalnosc dzialala potrzebne sa zmiany w zakresie widzialnosci nazw, tak zeby w procedurach on_ widoczne bylo srodowisko tego zewnetrznego obiektu
		if (subviews_vector.size() > 0) {
			QueryBinderResult *viewParentBinder = new QueryBinderResult(QE_NOTROOT_VIEW_PARENT_NAME, this);
			r->addResult(viewParentBinder);
		}
		for (unsigned int i = 0; i < subviews_vector.size(); i++ ) {
			LogicalID *tmp_logID = subviews_vector.at(i);
			string view_name = "";
			string view_code = "";
			errcode = qe->checkViewAndGetVirtuals(tmp_logID, view_name, view_code);
			if (errcode != 0) return errcode;
			
			vector<QueryBagResult*> envs_sections;
			envs_sections.push_back(r);
			
			QueryResult *tmp_View_LID_result = new QueryReferenceResult(tmp_logID);
			errcode = tmp_View_LID_result->nested(tr, qe);
			if(errcode != 0) return errcode;
			QueryBagResult *nested_tmp_View_LID_result;
			errcode = qe->getEnvs()->top(nested_tmp_View_LID_result);
			if(errcode != 0) return errcode;
			errcode = qe->getEnvs()->pop();
			if(errcode != 0) return errcode;
			envs_sections.push_back((QueryBagResult *) nested_tmp_View_LID_result);
			
			errcode = qe->callProcedure(view_code, envs_sections);
			if(errcode != 0) return errcode;
			
			QueryResult *res;
			qe->pop_qres(res);
			QueryResult *bagged_res = new QueryBagResult();
			((QueryBagResult *) bagged_res)->addResult(res);
			for (unsigned int j = 0; j < bagged_res->size(); j++) {
				QueryResult *seed;
				errcode = ((QueryBagResult *) bagged_res)->at(j, seed);
				if (errcode != 0) return errcode;
				vector<QueryResult *> seeds;
				seeds.push_back(seed);
				QueryResult *virt_res = new QueryVirtualResult(view_name, tmp_logID, seeds);
				QueryBinderResult *virt_binder = new QueryBinderResult(view_name, virt_res);
				r->addResult(virt_binder);
				ec->printf("[QE] nested(): new QueryVirtualResult returned name: %s\n", view_name.c_str());
			}
		}
		
		errcode = qe->getEnvs()->push(r, tr, qe);
		if(errcode != 0) return errcode;
		bool classFound = false;
		errcode = qe->getEnvs()->pushClasses(tmp_data_value, qe, classFound);
		if(errcode != 0) return errcode;
		return 0;
	}
	return qe->getEnvs()->push(r, tr, qe);
}



int QueryVirtualResult::nested(Transaction *&tr, QueryResult *&r, QueryExecutor * qe) {
	int errcode;
	ec->printf("[QE] nested(): QueryVirtualResult\n");
	
	vector<QueryBagResult*> envs_sections;
	
	QueryResult *tmp_View_LID_result = new QueryReferenceResult(view_def);
	errcode = tmp_View_LID_result->nested(tr, qe);
	if(errcode != 0) return errcode;
	QueryBagResult *nested_tmp_View_LID_result;
	errcode = qe->getEnvs()->top(nested_tmp_View_LID_result);
	if(errcode != 0) return errcode;
	errcode = qe->getEnvs()->pop();
	if(errcode != 0) return errcode;
	envs_sections.push_back((QueryBagResult *) nested_tmp_View_LID_result);
	
	for (int k = ((seeds.size()) - 1); k >= 0; k-- ) {
		QueryResult *current_seed = seeds.at(k);
		errcode = current_seed->nested(tr, qe);
		if(errcode != 0) return errcode;
		QueryBagResult *nested_seed;
		errcode = qe->getEnvs()->top(nested_seed);
		if(errcode != 0) return errcode;
		errcode = qe->getEnvs()->pop();
		if(errcode != 0) return errcode;
		envs_sections.push_back((QueryBagResult *) nested_seed);
	}
	
	string on_navigate_code;
	string on_navigate_paramname;
	errcode = qe->getOn_procedure(view_def, "on_navigate", on_navigate_code, on_navigate_paramname);
	if (errcode != 0) return errcode;
	
	// Gdy znaleziona jest procedura on_navigate, wykonywane jest przejscie po wirtualnym pointerze.
	if (on_navigate_code != "") {
	
		errcode = qe->callProcedure(on_navigate_code, envs_sections);
		if(errcode != 0) return errcode;
		
		QueryResult *result;
		qe->pop_qres(result);
		QueryResult *bagged_result = new QueryBagResult();
		((QueryBagResult *) bagged_result)->addResult(result);
		for (unsigned int k = 0; k < bagged_result->size(); k++) {
			QueryResult *next_res;
			errcode = ((QueryBagResult *) bagged_result)->at(k, next_res);
			if (errcode != 0) return errcode;
			
			int next_res_type = next_res->type();
			switch (next_res_type) {
				case QueryResult::QVIRTUAL: {
					string next_res_name = ((QueryVirtualResult *) next_res)->vo_name;
					QueryBinderResult *next_res_binder = new QueryBinderResult(next_res_name, next_res);
					r->addResult(next_res_binder);
					break;
				}
				case QueryResult::QREFERENCE: {
					ObjectPointer *optr;
					LogicalID *tmp_logID = ((QueryReferenceResult *) next_res)->getValue();
					if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr, false)) != 0) {
						*ec << "[QE] Error in getObjectPointer";
						qe->antyStarveFunction(errcode);
						qe->inTransaction = false;
						return errcode;
					}
					string next_res_name = optr->getName();
					
					QueryBinderResult *next_res_binder = new QueryBinderResult(next_res_name, next_res);
					r->addResult(next_res_binder);
					break;
				}
				case QueryResult::QBINDER: {
					r->addResult(next_res);
					break;
				}
			}
		}
	}
	// Przymujac semantyke, ktora wyklucza jednoczesne podperspektywy, gdy jest on_navigate
	// trzeba odkomentowac else { i }
	
	//else {
		vector<LogicalID *> subviews;
		errcode = qe->getSubviews(view_def, vo_name, subviews);
		if (errcode != 0) return errcode;
		for (unsigned int i = 0; i < subviews.size(); i++ ) {
			LogicalID *subview_lid = subviews.at(i);
			string subview_name = "";
			string subview_code = "";
			errcode = qe->checkViewAndGetVirtuals(subview_lid, subview_name, subview_code);
			if (errcode != 0) return errcode;
			
			errcode = qe->callProcedure(subview_code, envs_sections);
			if(errcode != 0) return errcode;
			
			QueryResult *res;
			qe->pop_qres(res);
			QueryResult *bagged_res = new QueryBagResult();
			((QueryBagResult *) bagged_res)->addResult(res);
			for (unsigned int j = 0; j < bagged_res->size(); j++) {
				QueryResult *sub_seed;
				errcode = ((QueryBagResult *) bagged_res)->at(j, sub_seed);
				if (errcode != 0) return errcode;
				vector<QueryResult *> sub_seeds;
				sub_seeds.push_back(sub_seed);
				for (unsigned int k = 0; k < (seeds.size()); k++ ) {
					sub_seeds.push_back(seeds.at(k));
				}
				QueryResult *virt_res = new QueryVirtualResult(subview_name, subview_lid, sub_seeds);
				QueryBinderResult *final_binder = new QueryBinderResult(subview_name, virt_res);
				r->addResult(final_binder);
			}
			
		}
	//}
	return 0;
}

}
