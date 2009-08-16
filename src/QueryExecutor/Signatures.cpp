#include "Signatures.h"
#include "QueryResult.h"
#include "QueryBuilder.h"
#include <QueryParser/TreeNode.h>
#include <Errors/ErrorConsole.h>
#include <TypeCheck/TCConstants.h>

namespace QExecutor
{
	Signatures::Signatures()
	{
		ec = &ErrorConsole::get_instance(EC_QUERY_EXECUTOR); 
	}
	
	int Signatures::checkDelCard(QueryResult *arg, QueryResult *&final, Transaction *tr) 
	{
		int errcode = 0;
		QueryBagResult* bagArg = new QueryBagResult();
		bagArg->addResult(arg);
		//Map below holds: for each parent-log-id, for a name of its subobject: how many are there. (so if -
		// after delete - the nr was to be zero - throw error - can't delete non-optional object.
		map<std::pair<unsigned int, string>, int> delSubMap;
		map<string, int> delRootMap;
		int objectsLeft = 0;
		for (unsigned int i = 0; i < bagArg->size(); i++) {
			QueryResult* toDelete;  //the object to be deleted
			errcode = bagArg->at(i, toDelete);
			if (errcode != 0) return errcode;
			if (toDelete->type() != QueryResult::QREFERENCE) return (ErrQExecutor | ERefExpected);
			LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
			ObjectPointer *optr;
			errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] check delete card - Error in getObjectPointer.");
				return errcode;
			}
			string optrName = optr->getName();
			if (optr->getIsRoot()) {
				errcode = checkUpInRootCardMap(optrName, delRootMap, tr);
				if (errcode != 0) return errcode;
				if ((objectsLeft = --delRootMap[optrName]) == 0) {
					debug_print(*ec,  "[QE] would leave no more roots with this name, but their card is >= 1.");
					return (ErrQExecutor | ECrcDelNonOptional);
				}
			} else {// complex ! - reach for the parent and check local card...
				if (lid->getDirectParent() == NULL) {
					debug_print(*ec,  "[QE] check delete card - Cannot delete child element, because parent not set.");
					return (ErrQExecutor | EQEUnexpectedErr);
				}
				LogicalID *ptLid = lid->getDirectParent();
				errcode = checkUpInDelSubMap(ptLid, optrName, delSubMap, tr);
				if (errcode != 0) return errcode;
				std::pair<unsigned int, string> key(ptLid->toInteger(), optrName);
				if ((objectsLeft = --delSubMap[key]) == 0) {
					debug_print(*ec,  "[QE] would delete all subobjects with this name, but their card is >= 1.");
					return (ErrQExecutor | ECrcDelNonOptional);
				}
			}
		}
		// May proceed with delete...
		final = arg;
		return 0;
	}

	int Signatures::checkUpInDelSubMap(LogicalID *ptLid, string name, map<std::pair<unsigned int, string>, int> &delMap, Transaction *tr) {
		int errcode = 0;
		std::pair<unsigned int, string> key(ptLid->toInteger(),name);
		if (delMap.find(key) == delMap.end()) {
			cout << "		[QE]::checkUpInDelSubMap - no entry for key: (" << key.first << ", " << key.second <<")! \n";
			ObjectPointer *optr;
			errcode = tr->getObjectPointer (ptLid, Store::Read, optr, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] check delete non-root - Error in getObjectPointer.");
				return errcode;
			}
			int vType = optr->getValue()->getType();
			if (vType != Store::Vector) {
				debug_print(*ec,  "[QE] check delete non-root  - ERROR! the l-Value has to be a reference to Vector");
				return (ErrQExecutor | EOtherResExp);
			}
			vector<LogicalID*> *vec = optr->getValue()->getVector();
			int sameNameObjectsNum = 0;
			for (unsigned int i = 0; i < vec->size(); i++) {
				errcode = tr->getObjectPointer(vec->at(i), Store::Read, optr, false);
				if (errcode != 0) {
					debug_print(*ec,  "[QE] check delete non-root - Error in getObjectPointer for one of the siblings.");
					return errcode;
				}
				if (optr->getName() == name) sameNameObjectsNum++;
			}
			delMap[key] = sameNameObjectsNum;
		}
		return 0;
	}

	int Signatures::checkUpInRootCardMap(string optrName, map<string, int> &delRootMap, Transaction *tr) {
		int errcode = 0;
		if (delRootMap.find(optrName) == delRootMap.end()) {
			//add root with its number to map.
			vector<LogicalID*>* vec;
			if ((errcode = tr->getRootsLID(optrName, vec)) != 0) {
				debug_print(*ec,  "[QE] check up in root Card map card - error in getRootsLID");
				return errcode;
			}
			delRootMap[optrName] = vec->size();
			delete vec;
		}
		return 0;
	}

	int Signatures::checkSubCardsRec(Signature *sig, ObjectPointer *optr, Transaction *tr) {
		if (optr == NULL) cout <<"optr is null!\n";
		if (sig == NULL) cout <<"sig is null!\n";
		if (sig == NULL || optr == NULL) return (ErrQExecutor | EOtherResExp);
		int errcode = 0;
		int result = 0;
		int vType = optr->getValue()->getType();
		if (vType != Store::Vector) return 0;
		debug_print(*ec,  "[QE][TC] checkSubCardsRec(optr) - ObjectValue = Vector");
		vector<LogicalID*>* tmp_vec = optr->getValue()->getVector();
		int tmp_vec_size = tmp_vec->size();

		SigColl *sigc = (sig->type() == Signature::SBINDER ? (SigColl *) ((StatBinder *)sig)->getValue() : (SigColl *) sig);
		map<string, int> subMap;
		for (int i = 0; i < tmp_vec_size; i++) {
			LogicalID *currID = tmp_vec->at(i);
			ObjectPointer *currOptr;
			errcode = tr->getObjectPointer(currID, Store::Read, currOptr, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] Error in getObjectPointer, in checkSubCardsRec(optr)");
				return errcode;
			}

			string currName = currOptr->getName();
			Signature *mptr = sigc->getMyList();
			bool foundName = false;
			Signature *match = NULL;
			while (mptr != NULL && not foundName) {
				if (((StatBinder *) mptr)->getName() == currName) {
					foundName = true;
					match = mptr;
				}
				mptr = mptr->getNext();
			}
			if (not foundName || match == NULL) return (ErrQExecutor | EOtherResExp);
			//recursively check each elt
			result = checkSubCardsRec(match, currOptr, tr);
			if (result != 0) return result;
			//register subobject in map. (adjusting the nr of same objects registered)
			if (subMap.find(currName) == subMap.end()) {
				subMap[currName] = 0;
			}
			subMap[currName]++;

		}
		Signature *ptr = sigc->getMyList();
		while (ptr != NULL) {
			string name = ((StatBinder *)ptr)->getName();
			string card = ptr->getCard();
			if (subMap.find(name) == subMap.end()) {
				subMap[name] = 0;
			}
			//check if its missing: not found in subMap  -> error
			if (card[0] != '0' && subMap[name] == 0) return (ErrQExecutor | ESigMissedSubs);

			//check for overflow: if rightBound of card '<' subMap[name].first -> error
			if (card[3]!= '*' && (card[3]-'0') < subMap[name]) return (ErrQExecutor | ESigCdOverflow);

			ptr = ptr->getNext();
		}
		return 0;
	}

	int Signatures::checkSubCardsRec(Signature *sig, QueryResult *res, bool isBinder, int &obtsSize) {
		if (sig == NULL || res == NULL) return (ErrQExecutor | EOtherResExp);
		if (sig->type() != Signature::SBINDER && sig->type() != Signature::SSTRUCT) { obtsSize = 1; return 0; }
		int result = 0;
		if (sig->type() == Signature::SBINDER) {
			QueryResult *toCompare = (isBinder ? ((QueryBinderResult *)res)->getItem() : res);
			Signature *sigVal = ((StatBinder *)sig)->getValue();
			int cmpType = toCompare->type();

			//in case of multiple objects ...
			if (cmpType == QueryResult::QBAG || cmpType == QueryResult::QSEQUENCE) {
				string card = sig->getCard();
				int compSize = toCompare->size();
				obtsSize = 0;
				for (int pos = 0; pos < compSize; pos++) {
					QueryResult *single;
					int singleSize = 0;
					if (cmpType == QueryResult::QBAG) {
						result = ((QueryBagResult *)toCompare)->at(pos, single);
					} else {
						result = ((QuerySequenceResult *)toCompare)->at(pos, single);
					}
					if (result == 0) {
						result = checkSubCardsRec(sigVal, single, true, singleSize);
					}
					if (result != 0) return result;
					obtsSize += singleSize;
				}
				if ((card[0] != '0' && obtsSize == 0) || (card[3] != '*' && obtsSize > 1))
					return (ErrQExecutor | EBadInternalCd);
				return 0;
			}
			//'toCompare' is some single object...
			obtsSize = 1;
			return checkSubCardsRec(sigVal, toCompare, true, obtsSize);
		}
		//sig is a STRUCT signature, res - a QueryStructResult
		SigColl *sigc = (SigColl *) sig;
		QueryStructResult *obj = (QueryStructResult *)res;
		obtsSize = 1;
		map<string, int> subMap;
		//for each elt of guest obj - check it doesn't violate card constraints of some subobject of sig.
		for (unsigned int i = 0; i < (obj->size()); i++) {
			QueryResult *tmp_res = NULL;
			result = obj->at(i, tmp_res);
			if (result != 0) return result;
			//in case of multiple objects ...
			if (tmp_res->type() == QueryResult::QBAG || tmp_res->type() == QueryResult::QSEQUENCE) {
				for (unsigned int pos = 0; pos < tmp_res->size(); pos++) {
					QueryResult *single;
					if (tmp_res->type() == QueryResult::QBAG) {
						result = ((QueryBagResult *)tmp_res)->at(pos, single);
					} else {
						result = ((QuerySequenceResult *)tmp_res)->at(pos, single);
					}
					if (result != 0) return result;
					result = checkSingleSubCard(sigc, single, subMap);
					if (result != 0) return result;
				}
			} else {
				result = checkSingleSubCard(sigc, tmp_res, subMap);
				if (result != 0) return result;
			}
		}
		//now -knowing the subMap - check for: subOverflows, subsMissing.
		Signature *ptr = sigc->getMyList();
		while (ptr != NULL) {
			string name = ((StatBinder *)ptr)->getName();
			string card = ptr->getCard();
			if (subMap.find(name) == subMap.end()) {
				subMap[name] = 0;
			}
			//check if its missing: not found in subMap  -> error
			if (card[0] != '0' && subMap[name] == 0) return (ErrQExecutor | ESigMissedSubs);

			//check for overflow: if rightBound of card '<' subMap[name].first -> error
			if (card[3]!= '*' && (card[3]-'0') < subMap[name]) return (ErrQExecutor | ESigCdOverflow);

			ptr = ptr->getNext();
		}
		return 0;
	}

	int Signatures::checkSingleSubCard(SigColl *sigc, QueryResult *tmp_res, map<string, int> &subMap) {
		int result = 0;
		if (tmp_res == NULL || (tmp_res->type()) != QueryResult::QBINDER) return (ErrQExecutor | EOtherResExp);
		string binderName = ((QueryBinderResult *) tmp_res)->getName();
			//QueryResult *binderItem = ((QueryBinderResult *) tmp_res)->getItem();
		Signature *mptr = sigc->getMyList();
		bool foundName = false;
		Signature *match = NULL;
		while (mptr != NULL && not foundName) {
			if (((StatBinder *) mptr)->getName() == binderName) {
				foundName = true;
				match = mptr;
			}
			mptr = mptr->getNext();
		}
		if (not foundName || match == NULL) return (ErrQExecutor | EOtherResExp);
			//recursively check each elt
		int eltSize = 0;
		result = checkSubCardsRec(match, tmp_res, true, eltSize);
		if (result != 0) return result;
			//register subobject in map. (adjusting the nr of same objects registered)
		if (subMap.find(binderName) == subMap.end()) {
			subMap[binderName] = 0;
		}
		subMap[binderName] += eltSize;
		return 0;
	}
	
	int Signatures::objDeclRec(DeclareDefineNode *obd, string rootName, bool typeDef, string ownerName, vector<string> *queries, bool isTopLevel, Transaction *tr) {
		string query = "";
		string objName = (isTopLevel ? TC_MDN_NAME : TC_SUB_OBJ_NAME);
		int errcode = 0;
		if (isTopLevel && typeDef) {
			query = QueryBuilder::getHandle()->query_create_typedef(obd->getName(), ((TypeDefNode *)obd)->getIsDistinct(), obd->getSigNode()->getHeadline());
		} else {
			query = QueryBuilder::getHandle()->query_create_obj_mdn(obd->getName(), ((ObjectDeclareNode *)obd)->getCard(), obd->getSigNode()->getHeadline(), objName);
		}
		queries->push_back(query);	// base query for declaration

		int sigKind =  obd->getSigNode()->getSigKind();
		string tName = "";
		vector<ObjectDeclareNode*> *subs = NULL;
		switch (sigKind) {
			case SignatureNode::atomic : break; //nothing needs be done here.
			case SignatureNode::ref : break;//Not inserting target-not needed by new DScheme.(though still be handled well)
		//QBuilder::query_insert_target(0, obdNode->getName(), false, obdNode->getSigNode()->getTypeName());push_back(query);
			case SignatureNode::defType :
				tName = obd->getSigNode()->getTypeName();
				//check if its an object. (if so - well, error, can't do that, only defined type name allowed here).
				if ((tr->getDmlStct()->findRoot(tName)) == DMLControl::object) {
					debug_print(*ec,  "Found MDN object while looking for type - have to report error");
					return (ErrQExecutor | EObjectInsteadOfType);
				}
				if (typeDef) {//If not in typedef -were done. else - check for recurrence & update dependency structs.
					debug_print(*ec,  "will look for dependencies..\n");
					if (tr->getDmlStct()->findDependency(tName, rootName) != 0) {
						debug_print(*ec,  "Attempt to define inftly recurrent types: " + tName + " <--> " + obd->getName());
						return (ErrQExecutor | ERecurrentTypes);
					}
					tr->getDmlStct()->markDependency (rootName, tName);
				}
				break;
			case SignatureNode::complex :
				subs = obd->getSigNode()->getStructArg()->getSubDeclarations();
				for (unsigned int i = 0; i < subs->size(); i++) {
					errcode = objDeclRec(subs->at(i), rootName, typeDef, obd->getName(), queries, false, tr);
					if (errcode != 0) {
						return errcode;
					}
				}
				break;
			default: debug_print(*ec,  "Unknown signature node kind"); break;
		}
		if (!isTopLevel) {
			//decide whether owner(parent node) is __MDN__, or __MDNT__, or some subobject somewhere...
			int ownerBase = (ownerName == rootName ? (typeDef ? 1: 0) : 2);
			queries->push_back(QueryBuilder::getHandle()->query_insert_owner(obd->getName(), ownerName, ownerBase));
			queries->push_back(QueryBuilder::getHandle()->query_insert_subobj(ownerBase, ownerName, obd->getName()));
		}
		return 0;
	}
	
	// May appear only through TC coerce augments
	int Signatures::coerceOperate(int cType, QueryResult *arg, QueryResult *&final, TreeNode */*tree*/, Transaction *tr) {
		int errcode = 0;

		switch (cType) {
			case CoerceNode::to_string : {
				debug_print(*ec,  "	Coercing to string");
				string final_value = "";
				stringstream ss;
				switch(arg->type()) {
					case QueryResult::QINT: {
						ss << (((QueryIntResult *)arg)->getValue());
						ss >> final_value;
						break;
					}
					case QueryResult::QDOUBLE: {
						ss << (((QueryDoubleResult *)arg)->getValue());
						ss >> final_value;
						break;
					}
					case QueryResult::QSTRING: {
						final_value = ((QueryStringResult *)arg)->getValue();
						break;
					}
					case QueryResult::QBOOL: {
						final_value = (((QueryBoolResult *)arg)->getValue() ? "true" : "false");
						break;
					}
					default: final = arg; return 0; // ignore the coerce
				}
				final = new QueryStringResult(final_value);
				return 0;
			}
			case CoerceNode::to_double : { // from int/double/string/bool
				debug_print(*ec,  "	Coercing to double");
				double final_value = 0.0;
				stringstream ss;
				switch (arg->type()) {
					case QueryResult::QINT: {
						final_value = double(((QueryIntResult *)arg)->getValue());
						break;
					}
					case QueryResult::QDOUBLE: {
						final_value = ((QueryDoubleResult *)arg)->getValue();
						break;
					}
					case QueryResult::QSTRING: {
						stringstream ss(((QueryStringResult *)arg)->getValue());
						string rest = "";
						bool wellDone = (ss >> final_value);
						ss >> rest;
						if (not wellDone || not rest.empty()) return (ErrQExecutor | ECrcToDouble);
						break;
					}
					case QueryResult::QBOOL: {
						final_value = (((QueryBoolResult *)arg)->getValue() ? 1.0 : 0.0);
						break;
					}
					default: final = arg; return 0; // ignore the coerce
				}
				final = new QueryDoubleResult(final_value);
				return 0;
			}
			case CoerceNode::to_int : { // from int/double/string/bool (!) rounds doubles down to their int part
				debug_print(*ec,  "	Coercing to integer");
				int final_value = 0;
				stringstream ss;
				switch (arg->type()) {
					case QueryResult::QINT: {
						final_value = ((QueryIntResult *)arg)->getValue();
						break;
					}
					case QueryResult::QDOUBLE: {
						final_value = int(((QueryDoubleResult *)arg)->getValue());
						break;
					}
					case QueryResult::QSTRING: {
						stringstream ss(((QueryStringResult *)arg)->getValue());
						string rest = "";
						bool wellDone = (ss >> final_value);
						ss >> rest;
						if (not wellDone || (!rest.empty() && rest.at(0) != '.')) return (ErrQExecutor | ECrcToInt);
						break;
					}
					case QueryResult::QBOOL : {
						final_value = (((QueryBoolResult *)arg)->getValue() ? 1 : 0);
						break;
					}
					default: final = arg; return 0; // ignore the coerce
				}
				final = new QueryDoubleResult(final_value);
				return 0;
			}
			case CoerceNode::to_bool : { // from int / double / string
				debug_print(*ec,  "	Coercing to bool");
				string fval = "";
				stringstream ss;
				switch(arg->type()) {
					case QueryResult::QINT: {
						ss << (((QueryIntResult *)arg)->getValue());
						ss >> fval;
						break;
					}
					case QueryResult::QDOUBLE: {
						ss << (((QueryDoubleResult *)arg)->getValue());
						ss >> fval;
						break;
					}
					case QueryResult::QSTRING: {
						fval = ((QueryStringResult *)arg)->getValue();
						break;
					}
					default: final = arg; return 0; // ignore the coerce /  for QBOOL: no need to cast
				}
				if (fval == "true" || fval == "TRUE" || fval == "1" || fval == "yes" || fval == "YES")
					final = new QueryBoolResult(true);
				else if(fval == "false" || fval == "FALSE" || fval == "0" || fval == "no" || fval == "NO")
					final = new QueryBoolResult(false);
				else return (ErrQExecutor | ECrcToBool);
				return 0;
			}
			case CoerceNode::element : {
				debug_print(*ec,  "	Coercing - element()");
				//Arg is already a single element...
				if (arg->type() != QueryResult::QBAG && arg->type() != QueryResult::QSEQUENCE) {
					final = arg;
					return 0;
				}
				if (arg->type() == QueryResult::QBAG) {
					QueryBagResult *bagArg = ((QueryBagResult *) arg);
					//If collection is empty or has more than 1 elt - return error.
					if (bagArg->isEmpty())	return (ErrQExecutor | ECrcEltEmptySet);
					if (bagArg->size() > 1) return (ErrQExecutor | ECrcEltMultiple);
					QueryResult *elem;
					errcode = bagArg->at(0, elem);
					if (errcode != 0) return errcode;
					final = elem;
					return 0;
				}
				if (arg->type() == QueryResult::QSEQUENCE) {
					QuerySequenceResult *seqArg = ((QuerySequenceResult *) arg);
					//If collection is empty or has more than 1 elt - return error.
					if (seqArg->isEmpty())	return (ErrQExecutor | ECrcEltEmptySet);
					if (seqArg->size() > 1) return (ErrQExecutor | ECrcEltMultiple);
					QueryResult *elem;
					errcode = seqArg->at(0, elem);
					if (errcode != 0) return errcode;
					final = elem;
					return 0;
				}
				break;
			}
			case CoerceNode::to_bag : {
				debug_print(*ec,  "	Coercing to bag");
				// AddResult() takes care of everything!
				final = new QueryBagResult();
				final->addResult(arg);
				return 0;
				// If arg is already a bag - nothing to be done.
				//if (arg->type() == QueryResult::QBAG) { final = arg; return 0;}
				// If arg is a seq - iterate through its elts, adding to a new bag.
				//if (arg->type() == QueryResult::QSEQUENCE) {}
			}
			case CoerceNode::to_seq : {
				debug_print(*ec,  "	Coercing to sequence");
				final = new QuerySequenceResult();
				final->addResult(arg);
				return 0;
			}
			case CoerceNode::can_del : {
				debug_print(*ec,  "	Coerce checking if delete allowed, with card 1..*");
				return checkDelCard(arg, final, tr);
			}
			case CoerceNode::ext_crt : {
				debug_print(*ec,  "	Coerce checking if cardinality of created object allows more creates.");
				QueryResult *bagRes = new QueryBagResult();
				bagRes->addResult(arg);
				map<string, int> rootCdMap;
				//Assumption: this dynamic coerce only made when card of this root is != '?..*'. So
				//it means there can only be 1 such obj. If one day more cards are available (eg. 0..3 ?)
				//this check below would have to be enhanced, size() would need be checked against upper bound.
				int maxAllowed = 1;
				for (unsigned int i = 0; i < bagRes->size(); i++) {
					QueryResult *binder;
					errcode = ((QueryBagResult *) bagRes)->at(i, binder);
					if (errcode != 0) return errcode;
					if ((binder->type()) != QueryResult::QBINDER) {
						return EOtherResExp;
					}
					string crtName = ((QueryBinderResult*)binder)->getName();

					errcode = checkUpInRootCardMap(crtName, rootCdMap, tr);
					if (errcode != 0) return errcode;
					if ((++rootCdMap[crtName]) > maxAllowed) {
						debug_print(*ec,  "[QE] would create too many '" + crtName + "' roots. Returning coerce error.");
						return (ErrQExecutor | ECrcCrtExtTooMany);
					}
				}
				break;
			}
			default: debug_print(*ec,  "	Coerce type not recognized"); break;
		}
		final = arg;
		return 0;
	}
}
