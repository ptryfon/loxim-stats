#include <stdio.h>
#include <string>
#include <vector>

#include <QueryExecutor/ExecutorViews.h>
#include <QueryExecutor/ClassGraph.h>
#include <QueryExecutor/QueryResult.h>
#include <QueryExecutor/InterfaceMaps.h>
#include <QueryExecutor/AccessMap.h>
#include <QueryParser/ClassNames.h>
#include <TransactionManager/Transaction.h>
#include <Store/Store.h>
#include <Store/DBDataValue.h>
#include <Store/DBLogicalID.h>
#include <QueryParser/QueryParser.h>
#include <QueryParser/TreeNode.h>
#include <QueryExecutor/QueryExecutor.h>
#include <QueryExecutor/EnvironmentStack.h>
#include <Errors/Errors.h>
#include <Errors/ErrorConsole.h>

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor {

EnvironmentStack::EnvironmentStack() { actualBindClassLid = NULL; actual_prior = 0; sectionDBnumber = 0; ec = &ErrorConsole::get_instance(EC_QUERY_EXECUTOR);}
EnvironmentStack::~EnvironmentStack() { this->deleteAll(); }

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
	debug_print(*ec,  r->toString(0, true));
	es.push_back(r);
	es_priors.push_back(actual_prior);
	qe->getAccessMap()->addSectionInfo(es.size() - 1, r); 
	debug_print(*ec,  "[QE] Environment Stack pushed");
	return 0;
}

int EnvironmentStack::pop(QueryExecutor *qe){
	if (es.empty()) {
		debug_print(*ec,  (ErrQExecutor | EQEmptySet));
		return (ErrQExecutor | EQEmptySet);
	}
	if (es.size() == sectionDBnumber) {
		debug_print(*ec,  "error: trying to pop() Data Base section");
		debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
		return (ErrQExecutor | EQEUnexpectedErr);
	}
	// delete ??
	unsigned int popedSectionNo = es.size();
	SectionToClassMap::iterator actClasses = classesPerSection.find(popedSectionNo);
	if(actClasses != classesPerSection.end()) {
		delete (*actClasses).second;
		classesPerSection.erase(actClasses);
	}
	
	SectionToInterfaceMap::iterator sectionIntIt = interfacePerSection.find(popedSectionNo);
	if (sectionIntIt != interfacePerSection.end())
		interfacePerSection.erase(sectionIntIt);
	
	qe->getAccessMap()->removeSection(es.size() - 1); 
	es.pop_back();
	es_priors.pop_back();
	debug_print(*ec,  "[QE] Environment Stack popped");
	return 0;
}

int EnvironmentStack::pushDBsection() {
	if (sectionDBnumber != 0) {
		debug_print(*ec,  "error: DBsection already pushed, can't be pushed once more");
		debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
		return (ErrQExecutor | EQEUnexpectedErr);
	}
	QueryResult *r = new QueryBagResult();
	es.push_back((QueryBagResult *)r);
	es_priors.push_back(actual_prior);
	sectionDBnumber = es.size();
	debug_print(*ec,  "[QE] Data Base section pushed on Environment Stack");
	return 0;
}

int EnvironmentStack::popDBsection() {
	if (sectionDBnumber == 0) {
		debug_print(*ec,  "error: DBsection wasn't pushed, can't be poped");
		debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
		return (ErrQExecutor | EQEUnexpectedErr);
	}
	if (es.empty()) {
		debug_print(*ec,  (ErrQExecutor | EQEmptySet));
		return (ErrQExecutor | EQEmptySet);
	}
	if (sectionDBnumber > es.size()) {
		debug_print(*ec, "error: DBsection number is greater then environment stack size");
		debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
		return (ErrQExecutor | EQEUnexpectedErr);
	}
	while (sectionDBnumber <= es.size()) {
		// delete ??
		es.pop_back();
		es_priors.pop_back();
	}	
	sectionDBnumber = 0;
	debug_print(*ec,  "[QE] Data Base section popped from Environment Stack");
	return 0;
}

int EnvironmentStack::top(QueryBagResult *&r) {
	if (es.empty()) {
		debug_print(*ec,  (ErrQExecutor | EQEmptySet));
		return (ErrQExecutor | EQEmptySet);
	}
	if (es.size() == sectionDBnumber) {
		debug_print(*ec,  "error: trying to top() Data Base section");
		debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
		return (ErrQExecutor | EQEUnexpectedErr);
	}
	r=(es.back());
	return 0;
}

bool EnvironmentStack::empty() { return es.empty(); }
int EnvironmentStack::size() { return es.size(); }



int EnvironmentStack::bindName(string name, int sectionNo, Transaction *&tr, QueryExecutor *qe, QueryResult *&r, string iName) {
	int errcode;
	debug_print(*ec,  "[QE] Name binding on ES");
	debug_print(*ec,  toString() );
	unsigned int number = (es.size());
	debug_printf(*ec, "[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	bool found_one = false;
	bool found_normal = false;
	bool found_virtual = false;
	
	InterfaceKey ik;
	if (!iName.empty())
		ik.setKey(iName);
	
	QueryBagResult *section;
	unsigned int sectionSize;
	QueryResult *sth;
	string current;
	for (unsigned int i = number; i >= 1; i--) {
		if (es_priors.at(i - 1) == actual_prior) {
			if (i == sectionDBnumber) {
				debug_printf(*ec, "[QE] bindName: ES section %u is Data Base section\n", i);
				vector<LogicalID*>* vec;
				if ((errcode = tr->getRootsLID(name, vec)) != 0) {
					debug_print(*ec,  "[QE] bindName - error in getRootsLID");
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize = vec->size();
				debug_printf(*ec, "[QE] %d Roots LID by name taken\n", vecSize);
				
				vector<LogicalID*>* vec_virt;
				if ((errcode = tr->getViewsLID(name, vec_virt)) != 0) {
					debug_print(*ec,  "[QE] bindName - error in getViewsLID");
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize_virt = vec_virt->size();
				debug_printf(*ec, "[QE] %d Views LID by name taken\n", vecSize_virt);
				
				vector<LogicalID*>* vec_sysvirt;
				if ((errcode = tr->getSystemViewsLID(name, vec_sysvirt)) != 0) {
					debug_print(*ec,  "[QE] bindName - error in getSystemViewsLID");
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize_sysvirt = vec_sysvirt->size();
				debug_printf(*ec, "[QE] %d SystemViews LID by name taken\n", vecSize_sysvirt);
				
				if (vecSize_virt == 0 && vecSize_sysvirt == 0) 
				{ 
					for (int i = 0; i < vecSize; i++ ) 
					{
						found_one = true;
						LogicalID *lid = vec->at(i);		
						QueryReferenceResult *lidres = new QueryReferenceResult(lid, ik);
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
							QueryReferenceResult *lidres = new QueryReferenceResult(lid, ik);
							r->addResult(lidres);
						}
						delete vecSubInvariant;
					}
				}
				else if (vecSize != 0) {
					if (vecSize_virt != 0) {
						debug_print(*ec,  "[QE] bindName error: Real and virtual objects have the same name");
					} else {
						debug_print(*ec,  "[QE] bindName error: Real and system view objects have the same name");
					}
					debug_print(*ec,  (ErrQExecutor | EBadBindName));
					return (ErrQExecutor | EBadBindName);
				} else if(vecSize_virt != 0 && vecSize_sysvirt != 0) {
					debug_print(*ec,  "[QE] bindName error: virtual objects and system view objects have the same name");
					debug_print(*ec,  (ErrQExecutor | EBadBindName));
					return (ErrQExecutor | EBadBindName);
				} else if (vecSize_virt > 1) {
					debug_print(*ec,  "[QE] bindName error: Multiple views defining virtual objects with the same name");
					debug_print(*ec,  (ErrQExecutor | EBadBindName));
					return (ErrQExecutor | EBadBindName);
				}
				else if (vecSize_sysvirt > 1) {
					debug_print(*ec,  "[QE] bindName error: Multiple system views defining objects with the same name");
					debug_print(*ec,  (ErrQExecutor | EBadBindName));
					return (ErrQExecutor | EBadBindName);
				}
				else {
					if (vecSize_virt == 1) {
						LogicalID *view_lid = vec_virt->at(0);
						string view_code;
						errcode = qe->getExViews()->checkViewAndGetVirtuals(view_lid, name, view_code, tr);
						if (errcode != 0) { qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode; }
						
						vector<QueryBagResult*> envs_sections;
						
						errcode = qe->getExViews()->createNewSections(NULL, NULL, view_lid, envs_sections, tr, qe);
						if (errcode != 0) { qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode; }
							
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
							vector<LogicalID *> view_defs;
							view_defs.push_back(view_lid);
							
							QueryResult *virt_res = new QueryVirtualResult(name, view_defs, seeds, NULL);
							((QueryVirtualResult *)virt_res)->setInterfaceKey(ik);
							r->addResult(virt_res);
						}
					} else {
						// Galaz dla wirtualnych widokow�
						debug_print(*ec,  "[QE] TUTAJ WYNIKI");
						found_one = true;
						LogicalID *svlid = vec_sysvirt->at(0);
						QueryReferenceResult *sysvirtres = new QueryReferenceResult(svlid, ik);
						r->addResult(sysvirtres);
					}
				}
				
				delete vec;
				delete vec_virt;
			}
			else {
				section = (es.at(i - 1));
				sectionSize = (section->size());
				debug_printf(*ec, "[QE] bindName: ES section %u got %u elements\n", i, sectionSize);
				for (unsigned int j = 0; j < sectionSize; j++) {
					errcode = (section->at(j,sth));
					if (errcode != 0) return errcode;
					
					if ((sth->type()) == (QueryResult::QBINDER)) {
						current = (((QueryBinderResult *) sth)->getName());
						debug_printf(*ec, "[QE] bindName: current %u name is: %s\n", j, current.c_str());
						
						if (qe->getAccessMap()->hasAccess(READ, current) == false)
						{
							debug_printf(*ec, "[QE] bindName: name %s is not accessible due to interface\n", current.c_str());
							continue;
						}
						
						if (current == name) {
							found_one = true;
							int found_type = (((QueryBinderResult*) sth)->getItem())->type();
							if (found_type == QueryResult::QVIRTUAL) found_virtual = true;
							else found_normal = true;
							if ((found_virtual) && (found_normal)) {
								debug_print(*ec,  "[QE] bindName error: Real and virtual objects have the same name");
								debug_print(*ec,  (ErrQExecutor | EBadBindName));
								return (ErrQExecutor | EBadBindName);
							}
							debug_print(*ec,  "[QE] bindName: Object added to Result");
							r->addResult(((QueryBinderResult *) sth)->getItem());
						}
					}
				}
				
			}
		}
		if (found_one) {
			debug_printf(*ec, "[QE] bindName: bind at %d section, Parser said it should be binded at %d\n", i - 1, sectionNo);
			return 0;
		}
	}
	debug_printf(*ec, "[QE] bindName: name not binded on ES, Parser said it should be binded at %d\n", sectionNo);
	return 0;
}


int EnvironmentStack::bindProcedureName(string name, unsigned int queries_size, Transaction *&tr, QueryExecutor *qe, string &code, vector<string> &params, int &bindSectionNo, LogicalID*& bindClassLid) {
	debug_print(*ec,  "[QE] Procedure Name binding on ES");
	int errcode;
	unsigned int envs_size = (es.size());
	int founded = 0;
	bindSectionNo = -1;
	bindClassLid = NULL;
	
	for (unsigned int i = envs_size; i >= 1; i--) {
		if (es_priors.at(i - 1) == actual_prior) {
			if (i == sectionDBnumber) {
				debug_printf(*ec, "[QE] bindProcedureName: ES section %u is Data Base section\n", i);
				vector<LogicalID*>* vec;
				if ((errcode = tr->getRootsLID(name, vec)) != 0) {
					debug_print(*ec,  "[QE] bindName - error in getRootsLID");
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
					return errcode;
				}
				int vecSize = vec->size();
				debug_printf(*ec, "[QE] %d Roots LID by name taken\n", vecSize);
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
				debug_printf(*ec, "[QE] bindProcedureName: ES section %u got %u elements\n", i, sectionSize);
				for (unsigned int j = 0; j < sectionSize; j++) {
					QueryResult *sth;
					errcode = (section->at(j,sth));
					if (errcode != 0) return errcode;
					if ((sth->type()) == (QueryResult::QBINDER)) {
						string current = (((QueryBinderResult *) sth)->getName());
						debug_printf(*ec, "[QE] bindProcedureName: current %u name is: %s\n", j, current.c_str());
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
					
					bool done = false;
					SectionToInterfaceMap::iterator ipsI = interfacePerSection.find(i);
					if (ipsI != interfacePerSection.end())
					{   //this section contains classes visible through interface
						pair<string, LogicalID*> entry = (*ipsI).second;
						string interfaceName = entry.first;
						LogicalID* classGraphLid = entry.second;
						SetOfLids* cgLids = new SetOfLids();
						cgLids->insert(classGraphLid);
						
						string methodCode;
						vector<string> methodParams;
						LogicalID* methodBindClassLid;
						
						errcode = qe->getCg()->findMethod(name, queries_size, cgLids, methodCode, methodParams, founded, actualBindClassLid, methodBindClassLid);
						if (errcode) return errcode;
						if (qe->getIm()->interfaceHasMethod(interfaceName, name, queries_size))
						{// method visible via interface
							done = true;
							code = methodCode;
							params = methodParams;
							bindClassLid = methodBindClassLid;
						}
						//else - method filtered out (belongs to interfaced class, but not present in interface)
					}
					
					if ((founded <= 0) && (!done))
					{
						SectionToClassMap::iterator cpsI = classesPerSection.find(i);
						if(cpsI != classesPerSection.end()) {
							errcode = qe->getCg()->findMethod(name, queries_size, (*cpsI).second, code, params, founded, actualBindClassLid, bindClassLid);
						}
					}
				}
			}
		}
		if (founded > 0) {
			debug_printf(*ec, "[QE] bindProcedureName: name binded at %d section of envs\n", i - 1);
			bindSectionNo = i - 1;
			return 0;
		}
	}
	debug_printf(*ec, "[QE] bindProcedureName: name not binded on ES\n");
	
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
	
	LogicalID* boundCgLid = NULL;
	SectionToInterfaceMap::iterator it = interfacePerSection.find(actualSectionNo);
	if (it != interfacePerSection.end())
	{
		boundCgLid = (*it).second.second; 
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
			if ((!boundCgLid) || (boundCgLid && (lid != boundCgLid)))
			{
				classFound = true;
				cgClasses->insert(lid);
			}
		}
	}
	if(cgClasses != NULL) {
		classesPerSection[actualSectionNo] = cgClasses;
	}
	return 0;
}

int EnvironmentStack::pushInterface(string interfaceName, LogicalID* boundClassLid)
{
	pair<string, LogicalID*> entry(interfaceName, boundClassLid);
	unsigned int actualSectionNo = size();
	if(actualSectionNo == 0) 
	{
		//TODO: error env stack corrupt
		return 1;
	}
	interfacePerSection[actualSectionNo] = entry;
	return 0;
}

void EnvironmentStack::deleteAll() {
	for(SectionToClassMap::iterator i = classesPerSection.begin(); i != classesPerSection.end(); ++i) {
		delete (*i).second;
	}  	
	interfacePerSection.clear();
	
	for (unsigned int i = 0; i < (es.size()); i++) {
		delete es.at(i);
	};
	
	es.clear();
	es_priors.clear();
}

string EnvironmentStack::toString() 
{
	stringstream c;
	c << "[EnvironmentStack] sectionDBnumber=" << sectionDBnumber 
		<< " actual_prior=" << actual_prior << endl;
	string result = c.str();
	for( unsigned int i = 0; i < es.size(); i++ ) {
		stringstream s;
		s << "es_elem prior: " << es_priors.at(i);
		result += es[i]->toString( 1, true, s.str() );
	}
	return result;
}

// nested function returns bag of binders, which will be pushed on the environment stack
int QueryResult::nested_and_push_on_envs(QueryExecutor * qe, Transaction *&tr) {
	int errcode;
	QueryResult *r;
	vector<DataValue*> dataVal_vec;
	errcode = this->nested(qe, tr, r, dataVal_vec);
	if (errcode != 0) return errcode;
	errcode = qe->getEnvs()->push(((QueryBagResult *) r), tr, qe);
	if(errcode != 0) return errcode;
	
	if (type() == QREFERENCE)
	{
		QueryReferenceResult *qrr = dynamic_cast<QueryReferenceResult *>(this);
		InterfaceKey k = qrr->getInterfaceKey();
		if (!k.isEmpty())
		{
			string interfaceName = k.getKey();
			LogicalID* classGraphLid = NULL;
			bool found;
			Schemas::ImplementationInfo iI = qe->getIm()->getImplementationForInterface(interfaceName, found, true);
			if (iI.getType() == Schemas::BIND_CLASS)
			{
				string className = iI.getName();
				qe->getCg()->getClassLidByName(className, classGraphLid);
				if (classGraphLid && qe->getCg()->vertexExist(classGraphLid))
				{
					qe->getEnvs()->pushInterface(interfaceName, classGraphLid);
				}
			}
		}
	}
	//FIXME gtimoszuk fix it!!!
	bool classFound;
	for (unsigned int i = 0; i < dataVal_vec.size(); i++) {
		classFound = false;
		errcode = qe->getEnvs()->pushClasses(dataVal_vec.at(i), qe, classFound);
		if(errcode != 0) return errcode;
	}

        //dataVal_vec.clear();
        //delete &dataVal_vec;
        return 0;
}


int QuerySequenceResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): ERROR! QuerySequenceResult shouldn't be nested");
	debug_print(*ec,  (ErrQExecutor | EOtherResExp));
	return (ErrQExecutor | EOtherResExp);
	// nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryBagResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): ERROR! QueryBagResult shouldn't be nested");
	debug_print(*ec,  (ErrQExecutor | EOtherResExp));
	return (ErrQExecutor | EOtherResExp);
	// nested () function is applied to rows of a QueryResult and so, it shouldn't be applied to sequences and bags
}

int QueryStructResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): QueryStructResult");
	r = new QueryBagResult();
	int errcode;
	for (unsigned int i = 0; i < str.size(); i++) {
		if ((str.at(i))->type() == QueryResult::QSTRUCT) {
			debug_print(*ec,  (ErrQExecutor | EOtherResExp));
			return (ErrQExecutor | EOtherResExp); // one row shouldn't contain another row;
		}
		else {
			QueryResult *tmp_res;
			errcode = ((str.at(i))->nested(qe, tr, tmp_res, dataVal_vec));
			if (errcode != 0) return errcode;
			r->addResult(tmp_res);
		}
	}
	return 0;
}

int QueryStringResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): QueryStringResult can't be nested");
	r = new QueryBagResult();
	return 0;
}

int QueryIntResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): QueryIntResult can't be nested");
	r = new QueryBagResult();
	return 0;
}

int QueryDoubleResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): QueryDoubleResult can't be nested");
	r = new QueryBagResult();
	return 0;
}

int QueryBoolResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): QueryBoolResult can't be nested");
	r = new QueryBagResult();
	return 0;
}

int QueryNothingResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	debug_print(*ec,  "[QE] nested(): QueryNothingResult can't be nested");
	r = new QueryBagResult();
	return 0;
}

int QueryBinderResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	r = new QueryBagResult();
	if (item != NULL) {
		QueryBinderResult *tmp_value = new QueryBinderResult(name,item);
		debug_printf(*ec, "[QE] nested(): QueryBinderResult copy returned name: %s\n", name.c_str());
		r->addResult(tmp_value);
	}
	return 0;
}

int QueryReferenceResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	r = new QueryBagResult();
	int errcode;
	debug_printf(*ec, "[QE] nested(): QueryReferenceResult\n");
	/* remote ID */
	if ((value != NULL) && (value->getServer() != "")) 
		return EUnknownNode;
	/* end of remoteID processing */
	
	ObjectPointer *optr = NULL;
        ObjectPointer *GTpom  = NULL;
	if (value != NULL) {
		if ((errcode = tr->getObjectPointer(value, Store::Read, optr, false)) != 0) {
			debug_print(*ec,  "[QE] Error in getObjectPointer");
			qe->antyStarveFunction(errcode);
			qe->inTransaction = false;
/*                        //gtimoszuk
                        if (optr  != NULL) {
                            delete  optr;
                        } */

			return errcode;
		}
                GTpom = optr;
                //gtimoszuk simple hack to be able to free optr later (which is GTpom)...
                //yes making memory leak to deal w with another memory leak...
		DataValue* dataVal = new DBDataValue(*optr->getValue());

                //DataValue* dataVal = optr->getValue();
                dataVal_vec.push_back(dataVal);
		
		/* Link */
		if (dataVal->getSubtype() == Store::Link) {
                    if (optr != NULL) {
                        delete optr;
                    }
			return EUnknownNode;
		}
		/* end of link processing */
		
		vector<LogicalID*> subviews_vector;
		map<string, bool> namesAccessible;

		int vType = dataVal->getType();
		switch (vType) {
			case Store::Integer: {
				debug_print(*ec,  "[QE] nested(): QueryReferenceResult pointing integer value - can't be nested");
				break;
			}
			case Store::Double: {
				debug_print(*ec,  "[QE] nested(): QueryReferenceResult pointing double value - can't be nested");
				break;
			}
			case Store::String: {
				debug_print(*ec,  "[QE] nested(): QueryReferenceResult pointing string value - can't be nested");
				break;
			}
			case Store::Pointer: {
				LogicalID *tmp_logID = dataVal->getPointer();
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr, false)) != 0) {
					debug_print(*ec,  "[QE] Error in getObjectPointer");
					qe->antyStarveFunction(errcode);
					qe->inTransaction = false;
                                        if (optr != NULL) {
                                            delete optr;
                                        }
					return errcode;
				}
				string tmp_name = optr->getName();
				QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
				QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
				debug_print(*ec,  "[QE] nested(): valuevalueQueryReferenceResult pointing reference value");
				r->addResult(final_binder);
				debug_printf(*ec, "[QE] nested(): new QueryBinderResult returned name: %s\n", tmp_name.c_str());
				
				break;
			}
			case Store::Vector: {
				vector<LogicalID*>* tmp_vec = (dataVal->getVector());
				debug_print(*ec,  "[QE] nested(): QueryReferenceResult pointing vector value");
				int vec_size = tmp_vec->size();
			
				bool filterOut;				
				string k = getInterfaceKey().getKey();
				TStringSet namesVisible = qe->getIm()->getAllFieldNamesAndCheckBind(k, filterOut);
				if (filterOut)
					debug_printf(*ec, "[QE] nested(): field names will be filtered out, %d names visible\n", namesVisible.size());
				
				for (int i = 0; i < vec_size; i++ ) {
					LogicalID *tmp_logID = tmp_vec->at(i);
                                        ObjectPointer *optr2;
					if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr2, false)) != 0) {
						debug_print(*ec,  "[QE] Error in getObjectPointer");
						qe->antyStarveFunction(errcode);
						qe->inTransaction = false;
/*						if ((optr2 != NULL) && !(*optr == *optr2)) {
                                                    delete optr2;
                                                } */
                                                return errcode;
					}
					string tmp_name = optr2->getName();
					if ((filterOut) && (namesVisible.find(tmp_name) == namesVisible.end()))
					{   //Field filtered out
						namesAccessible[tmp_name] = false;
					}
					else
					{
						namesAccessible[tmp_name] = true;
					}
					
					//MH TODO: add a 'this->value' reference to each tmp_logID ?
					tmp_logID->setDirectParent(this->value);
					QueryReferenceResult *final_ref = new QueryReferenceResult(tmp_logID);
					QueryBinderResult *final_binder = new QueryBinderResult(tmp_name, final_ref);
					debug_printf(*ec, "[QE] nested(): vector element number %d\n", i);
					r->addResult(final_binder);
					debug_printf(*ec, "[QE] nested(): new QueryBinderResult returned name: %s\n", tmp_name.c_str());
					
					DataValue* tdv;
					tdv = optr2->getValue();
					if ((dataVal->getSubtype() != Store::View) && (tdv->getType() == Store::Vector) && (tdv->getSubtype() == Store::View)) {
						subviews_vector.push_back(tmp_logID);
					}
/*                                        if ((optr2 != NULL) && !(*optr == *optr2)) {
                                            delete optr2;
                                        } */
				}
				qe->getAccessMap()->propagateAccess(optr->getName(), namesAccessible);
					
				break;
			}
			default : {
				debug_print(*ec,  "[QE] nested(): ERROR! QueryReferenceResult pointing unknown format value");
				debug_print(*ec,  (ErrQExecutor | EUnknownValue));
                                if (optr != NULL) {
                                    delete optr;
                                }
				return (ErrQExecutor | EUnknownValue);
				break;
			}
		}
		QueryBinderResult *selfBinder = new QueryBinderResult(QE_SELF_KEYWORD, this);
		r->addResult(selfBinder);
		
		if (subviews_vector.size() > 0) {
			QueryBinderResult *viewParentBinder = new QueryBinderResult(QE_NOTROOT_VIEW_PARENT_NAME, this);
			r->addResult(viewParentBinder);
		}
		for (unsigned int i = 0; i < subviews_vector.size(); i++ ) {
			LogicalID *tmp_logID = subviews_vector.at(i);
			string view_name = "";
			string view_code = "";
			errcode = qe->getExViews()->checkViewAndGetVirtuals(tmp_logID, view_name, view_code, tr);
			if (errcode != 0) {
                            if (optr != NULL) {
                                delete optr;
                            }
							qe->antyStarveFunction(errcode);
							qe->inTransaction = false;
                            return errcode;
                        }
			
			vector<QueryBagResult*> envs_sections;
			envs_sections.push_back((QueryBagResult *) r);
			
			errcode = qe->getExViews()->createNewSections(NULL, NULL, tmp_logID, envs_sections, tr, qe);
			if (errcode != 0) { qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode; }
			
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
				vector<LogicalID *> view_defs;
				view_defs.push_back(tmp_logID);
				QueryResult *virt_res = new QueryVirtualResult(view_name, view_defs, seeds, value);
				QueryBinderResult *virt_binder = new QueryBinderResult(view_name, virt_res);
				r->addResult(virt_binder);
				debug_printf(*ec, "[QE] nested(): new QueryVirtualResult returned name: %s\n", view_name.c_str());
			}
		}
/*            if (GTpom != NULL) {
        		delete GTpom;
            } */
	}
    
	return 0;
}

int QueryVirtualResult::nested(QueryExecutor * qe, Transaction *&tr, QueryResult *&r, vector<DataValue*> &dataVal_vec) {
	r = new QueryBagResult();
	int errcode;
	debug_printf(*ec, "[QE] nested(): QueryVirtualResult\n");
	
	string on_navigate_code = "";
	string on_navigate_paramname = "";
	errcode = qe->getExViews()->getOn_procedure(view_defs.at(0), "on_navigate", on_navigate_code, on_navigate_paramname, tr);
	if (errcode != 0) { qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode;}
	
	bool filterOut;
	string k = getInterfaceKey().getKey();
	map<string, bool> accessibleNames;
	TStringSet namesVisible = qe->getIm()->getAllFieldNamesAndCheckBind(k, filterOut);
	if (filterOut)
	{
		for (TStringSet::iterator it = namesVisible.begin(); it != namesVisible.end(); ++it)
			accessibleNames[*it] = true;
		debug_printf(*ec, "[QE] nested(): field names will be filtered out, %d names visible\n", namesVisible.size());
	}			
	
	// Gdy znaleziona jest procedura on_navigate, wykonywane jest przejscie po wirtualnym pointerze.
	if (on_navigate_code != "") {
		
		vector<QueryBagResult*> envs_sections1;
		
		errcode = qe->getExViews()->createNewSections(this, NULL, NULL, envs_sections1, tr, qe);
		if (errcode != 0) { qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode; }
	
		errcode = qe->callProcedure(on_navigate_code, envs_sections1);
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
					if (filterOut) 
						((QueryVirtualResult *) next_res)->setInterfaceKey(getInterfaceKey());
					QueryBinderResult *next_res_binder = new QueryBinderResult(next_res_name, next_res);
					r->addResult(next_res_binder);
					break;
				}
				case QueryResult::QREFERENCE: {
					ObjectPointer *optr;
					LogicalID *tmp_logID = ((QueryReferenceResult *) next_res)->getValue();
					if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, optr, false)) != 0) {
						debug_print(*ec,  "[QE] Error in getObjectPointer");
						qe->antyStarveFunction(errcode);
						qe->inTransaction = false;
						return errcode;
					}
					string next_res_name = optr->getName();
					
					if (filterOut) 
						((QueryReferenceResult *) next_res)->setInterfaceKey(getInterfaceKey());
					
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
	// Przymujac semantyke, ktora nie wyklucza jednoczesnych podperspektyw, gdy jest on_navigate
	// trzeba zakomentowac else { i }
	
	else {
		vector<LogicalID *> subviews;
		errcode = qe->getExViews()->getSubviews(view_defs.at(0), vo_name, subviews, tr);
		if (errcode != 0) {qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode;}
		for (unsigned int i = 0; i < subviews.size(); i++ ) {
			LogicalID *subview_lid = subviews.at(i);
			string subview_name = "";
			string subview_code = "";
			errcode = qe->getExViews()->checkViewAndGetVirtuals(subview_lid, subview_name, subview_code, tr);
			if (errcode != 0) {qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode;}

			if ((filterOut) && (namesVisible.find(subview_name) == namesVisible.end()))
			{   //Field filtered out
				accessibleNames[subview_name] = false;
			}
			else
			{
				accessibleNames[subview_name] = true;
			}
			
			vector<QueryBagResult*> envs_sections2;
		
			errcode = qe->getExViews()->createNewSections(this, NULL, subview_lid, envs_sections2, tr, qe);
			if (errcode != 0) { qe->antyStarveFunction(errcode); qe->inTransaction = false; return errcode; }
			
			errcode = qe->callProcedure(subview_code, envs_sections2);
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
				vector<LogicalID *> subview_defs;
				subview_defs.push_back(subview_lid);
				for (unsigned int k = 0; k < (view_defs.size()); k++ ) {
					subview_defs.push_back(view_defs.at(k));
				}
				for (unsigned int k = 0; k < (seeds.size()); k++ ) {
					sub_seeds.push_back(seeds.at(k));
				}
				QueryResult *virt_res = new QueryVirtualResult(subview_name, subview_defs, sub_seeds, view_parent);
				QueryBinderResult *final_binder = new QueryBinderResult(subview_name, virt_res);
				r->addResult(final_binder);
			}
			
		}		
	}
	qe->getAccessMap()->propagateAccess(vo_name, accessibleNames);
	qe->getAccessMap()->addViewEnvironment(this);
	
	return 0;
}

}
