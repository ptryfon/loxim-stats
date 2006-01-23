/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>
#include <string>
#include <vector>
   
#include "QueryResult.h"
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

int QueryExecutor::executeQuery(TreeNode *tree, QueryResult **result) {
	ErrorConsole ec;
	int errcode;
	fprintf(stderr, "[QE] executeQuery()\n");
	
	if (tree != NULL)
	{
		if (tr == NULL) {
			fprintf(stderr, "[QE] Asking TransactionManager for a new transaction\n");
			if ((errcode = TransactionManager::getHandle()->createTransaction(tr)) != 0) 
				{
				fprintf(stderr, "Error in createTransaction\n");
    				return errcode;	
				}
			else {
				fprintf(stderr, "[QE] Transaction opened\n");
			}
		}
		int nodeType = tree->type();
		fprintf(stderr, "[QE] TreeType taken\n");
	
		switch (nodeType)
		{
		case TreeNode::TNNAME:
			{
			string name = tree->getName();
			fprintf(stderr, "[QE] Type: TNNAME\n");
			
			if ((stack.size()) == 1) {
			    errcode = (stack.pop());
			    if (errcode != 0) { return errcode; };
			};
			if (stack.empty()) {
				vector<ObjectPointer*>* vec;
				if ((errcode = tr->getRoots(vec)) != 0) {
					return errcode;
				};
				int vecSize = vec->size();
				fprintf(stderr, "[QE] All %d Roots taken\n", vecSize);
				QueryBagResult *stackSection = new QueryBagResult();
				for (int i = 0; i < vecSize; i++ )
					{
   					ObjectPointer *optr = vec->at(i);
					LogicalID *lid = optr->getLogicalID();
					fprintf(stderr, "[QE] LogicalID received\n");
					string optrName = (optr->getName());
					fprintf(stderr, "[QE] Name received\n");
					QueryReferenceResult *lidres = new QueryReferenceResult(lid);
					QueryBinderResult *newBinding = new QueryBinderResult(optrName, lidres);
					stackSection->addResult(newBinding);
				};
				if ( (stack.push(stackSection)) != 0 ) {
					return -1;
				};
			};			
			if ((stack.bindName(name, *result)) != 0) {
				return -1;
			};
			fprintf(stderr, "[QE] Done!\n");
			return 0;
			}//case

		case TreeNode::TNCREATE:
			{
			string name = tree->getName();
			DataValue* value;
			fprintf(stderr, "[QE] Type: TNCREATE\n");
			tree = tree->getArg();
			fprintf(stderr, "[QE] Getting node arguments\n");
			QueryResult* nextResult; 
			if ((errcode = executeQuery (tree, &nextResult)) != 0)
				{
				return errcode;
				}
			
			DBDataValue *dbValue = new DBDataValue;
			
			if (tree != NULL)
				{
				nodeType = tree->type();
				fprintf(stderr, "[QE] Node type recognized.\n");

				switch (nodeType) 
				{
				case TreeNode::TNINT:
					{
					int intValue = ((IntNode *) tree)->getValue();
					dbValue->setInt(intValue);
					fprintf(stderr, "[QE] It's 'integer'.\n");
					break;
					}
			
				case TreeNode::TNSTRING: 
					{
					string stringValue = ((StringNode *) tree)->getValue();
					dbValue->setString(stringValue);
					fprintf(stderr, "[QE] It's 'string'.\n");
					break;
					}

				case TreeNode::TNDOUBLE: 
					{
					double doubleValue = ((DoubleNode *) tree)->getValue();
					dbValue->setDouble(doubleValue);
					fprintf(stderr, "[QE] It's 'double'.\n");
					break;
					} 
				default:
					{
					ec << (EBadType | ErrQExecutor);
					fprintf(stderr, "[QE] Error - incorrect node type.\n");
					return EBadType | ErrQExecutor;
					}
			
				}//switch
				value = dbValue;
				fprintf(stderr, "[QE] Value set\n");
				} //if
			else
				{
				vector<LogicalID*> emptyVector;
				dbValue->setVector(&emptyVector);
				value = dbValue;
				fprintf(stderr, "[QE] No arguments (value = NULL)\n");
				}
			ObjectPointer *optr;
			if ((errcode = tr->createObject(name, value, optr)) != 0)
				{
				fprintf(stderr, "[QE] Error in createObject\n");
				return errcode;
				}
			if ((errcode = tr->addRoot(optr)) != 0)
				{
				fprintf(stderr, "[QE] Error in addRoot\n");
				return errcode;
				}
			*result = new QueryBagResult;
			fprintf(stderr, "[QE] QueryBagResult created\n");
			QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
			(*result)->addResult (lidres);
			fprintf(stderr, "[QE] Object added to QueryResult. Done!\n");
			return 0;
			}


		case TreeNode::TNINT: 
			{
			int intValue = ((IntNode *) tree)->getValue();
			fprintf(stderr, "[QE] TNINT: %d\n", intValue);
			*result = new QueryBagResult;
			QueryIntResult *tmpResult = new QueryIntResult(intValue);
			(*result)->addResult(tmpResult);
			fprintf(stderr, "[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
			}
		case TreeNode::TNSTRING: 
			{
			string stringValue = ((StringNode *) tree)->getValue();
			//fprintf(stderr, "[QE] QueryStringResult (%s) created\n",stringValue);
			*result = new QueryBagResult;
			QueryStringResult *tmpResult = new QueryStringResult(stringValue);
			(*result)->addResult(tmpResult);
			//fprintf(stderr, "[QE] QueryStringResult (%s) created\n",stringValue);
			return 0;
			}
		case TreeNode::TNDOUBLE:
			{
			double doubleValue = ((DoubleNode *) tree)->getValue();
			fprintf(stderr, "[QE] TNDOUBLE: %f\n", doubleValue);
			*result = new QueryBagResult;
			QueryDoubleResult *tmpResult = new QueryDoubleResult(doubleValue);
			(*result)->addResult(tmpResult);
			fprintf(stderr, "[QE] QueryDoubleResult (%f) created\n", doubleValue);
			return 0;
			}
		case TreeNode::TNVECTOR: {break;}
		case TreeNode::TNAS: {break;}
	
		case TreeNode::TNUNOP: 
			{
			UnOpNode::unOp op = ((UnOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] Unary operator - type recognized\n");
	    
			switch (op)
			{
			case UnOpNode::deleteOp:
				{
				fprintf(stderr, "[QE] Operator: deleteOp\n");
		
				tree = tree->getArg();
				fprintf(stderr, "[QE] Getting node arguments\n");
				
				QueryResult* nextResult; 
				if ((errcode = executeQuery (tree, &nextResult)) != 0)
					{
					return errcode;
					}

				QueryResult* toDelete;  //single object to be deleted
				unsigned int counter = nextResult->size();
				for (unsigned int i = 0; i < counter; i++ ) // Deleting objects
					{
   					nextResult->getResult(toDelete);  //bledy??
					LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
					ObjectPointer *optr;
					if ((errcode = tr->getObjectPointer (lid, Store::Write, optr)) !=0)
						{
						fprintf(stderr, "[QE] Error in getObjectPointer.\n");
						return errcode;
						}
					if ((errcode = tr->removeRoot(optr)) != 0)
						{
						fprintf(stderr, "[QE] Error in removeRoot.\n");
						return errcode;
						}
					fprintf(stderr, "[QE] Root removed\n");
					if ((errcode = tr->deleteObject(optr)) != 0)
						{
						fprintf(stderr, "[QE] Error in deleteObject.\n");
						return errcode;
						}
					fprintf(stderr, "[QE] Object deleted\n");
					} //for
				*result = new QueryNothingResult;
				fprintf(stderr, "[QE] QueryNothingResult created\n");
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				} //case DELETE
			case UnOpNode::unMinus:
				{
				fprintf(stderr, "[QE] unMinus operation\n");
				QueryResult *bagResult;
				if ((errcode = executeQuery (tree->getArg(), &bagResult)) != 0)
					{
					return errcode;
					}
				if (bagResult->type() != QueryResult::QBAG)
					{
					fprintf(stderr, "[QE] Error - wrong unMinus argument\n");
					return -1;
					}
				QueryResult *tmpResult, *negResult;
				if ((errcode = ((QueryBagResult *) bagResult)->getResult(tmpResult) != 0))
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Result counted, counting unMinus\n");
				int argType = tmpResult->type();
				fprintf(stderr, "[QE] left argument's type taken\n");
				switch (argType)
					{
					case QueryResult::QINT: //argument is a QueryIntResult
						{
						if ((errcode = ((QueryIntResult *) tmpResult)->minus(negResult)) != 0)
							{
							return errcode;
							}
						}
					case QueryResult::QDOUBLE: //argument is a QueryDoubleResult
						{
						if ((errcode = ((QueryDoubleResult *) tmpResult)->minus(negResult)) != 0)
							{
							return errcode;
							}
						}
					default: //wrong type!
						{
						fprintf(stderr, "[QE] Error - wrong unMinus argument\n");
						return -1;
						}
					}//switch
				*result = new QueryBagResult;
				(*result)->addResult(negResult);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case unMinus
			case UnOpNode::boolNot:
				{
				fprintf(stderr, "[QE] NOT operation\n");
				QueryResult *notResult, *tmpResult, *bagResult;;
				if ((errcode = executeQuery (tree->getArg(), &bagResult)) != 0)
					{
					return errcode;
					}
				if (bagResult->type() != QueryResult::QBAG)
					{
					fprintf(stderr, "[QE] Error - wrong boolNot argument\n");
					return -1;
					}
				if ((errcode = ((QueryBagResult *) bagResult)->getResult(tmpResult) != 0))
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Result counted, counting NOT\n");
				if ((errcode = ((QueryBoolResult *) tmpResult)->bool_not(notResult)) != 0)
					{
					return errcode;
					}
				*result = new QueryBagResult;
				(*result)->addResult(notResult);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case NOT
				
			case UnOpNode::deref:
				{
				fprintf(stderr, "[QE] DEREF operation\n");
				DataValue* value;
				QueryResult *queryBag;
				QueryResult *nextResult;
				
				if ((errcode = executeQuery (tree->getArg(), &queryBag)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Argument evaluated\n");
				int argType = queryBag->type();
				fprintf(stderr, "[QE] Argument's type taken\n");
				if (argType==QueryResult::QBAG)
					{
					*result = new QueryBagResult;
					unsigned int bagSize = (queryBag)->size();
					for (unsigned int i = 0; i < bagSize; i++) 
						{
						if ((errcode = ((QueryBagResult *) queryBag)->getResult(nextResult) != 0))
							{
							return errcode;
							}
						int resultType = nextResult->type();
						if (resultType == QueryResult::QREFERENCE)
							{
							fprintf(stderr, "[QE] Dereferencing %d result\n", i);
							ObjectPointer *optr;
							if ((errcode = tr->getObjectPointer(((QueryReferenceResult *)nextResult)->getValue(), Store::Read, optr)) != 0)
								{
								fprintf(stderr, "[QE] Error in getObjectPointer\n");
								return errcode;
								}
					
							value = optr->getValue();
							int vType = value->getType();
							
							
							switch (vType)
							    {
							    case Store::Integer: // it's an integer
								{
								fprintf(stderr, "[QE] ObjectValue = Int\n");
								QueryIntResult *tmpResult;
								tmpResult = new QueryIntResult;
								fprintf(stderr, "[QE] Value is %d\n", value->getInt());
								tmpResult->setValue(value->getInt());
			    					((QueryBagResult *)(*result))->addResult(tmpResult);
								fprintf(stderr, "[QE] Result added\n");
								break;
								}
							    case Store::Double: // it's a double
								{
								fprintf(stderr, "[QE] ObjectValue = Double\n");
								QueryDoubleResult *tmpResult;
								tmpResult = new QueryDoubleResult;
								tmpResult->setValue(value->getDouble());
			    					((QueryBagResult *)(*result))->addResult(tmpResult);
								fprintf(stderr, "[QE] Result added\n");
								break;
	   							}
							    case Store::String: // it's a string
								{
								fprintf(stderr, "[QE] ObjectValue = String\n");
								QueryStringResult *tmpResult;
								tmpResult = new QueryStringResult;
								tmpResult->setValue(value->getString());
			    					((QueryBagResult *)(*result))->addResult(tmpResult);
								fprintf(stderr, "[QE] Result added\n");
								break;
								}
							    default:
								{
								fprintf(stderr, "[QE] Error - wrong argument type\n");
								return -1;
								}
							    } //switch
							}
						else 
							{
							fprintf(stderr, "[QE] Error - the bag result must consist of QREFERENCE\n");
							return -1;
							}
						} //for
					fprintf(stderr, "[QE] Done!\n");
					return 0;
					} //case deref
				else 
	
					{
					fprintf(stderr, "[QE] Error - wrong DEREF argument\n");
					return -1;
					}				
				}
					
			case UnOpNode::distinct:
				{
				fprintf(stderr, "[QE] DISTINCT operation\n");
				QueryResult *nextResult;
				if ((errcode = executeQuery (tree->getArg(), &nextResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Result evaluated, computing distinct\n");
				int argType = nextResult->type();
				fprintf(stderr, "[QE] argument's type taken\n");
				switch (argType)
				{
				case QueryResult::QSEQUENCE: //The argument is a QuerySequenceResult
					{
					*result = new QuerySequenceResult;
					QueryResult *r1, *r2;
					for (unsigned int i = 0; i < (((QuerySequenceResult *) nextResult)->size()); i++)
						{
						fprintf(stderr, "[QE] Checking element no. %d\n", i);
						if ((errcode = ((QuerySequenceResult *) nextResult)->at(i, r1)) != 0)
							{
							return errcode;
							}
						bool already_exists = false;
						for (unsigned int j = 0; (j < ((*result)->size())) && (!already_exists); j++) //checking if r1 is already present in result
							{
							if ((errcode = ((QuerySequenceResult *) nextResult)->at(j, r2)) != 0)
								{
								return errcode;
								}
							already_exists = ((r1->equal(r2)) && (i != j));
							};
						if (!already_exists) 
							{
							(*result)->addResult(r1);
							fprintf(stderr, "[QE] Adding element %d to result as unique\n", i);
							}
						}
					fprintf(stderr, "[QE] Done!\n");
					return 0;	
					}//case QSEQUENCE
				case QueryResult::QBAG: //The argument is a QueryBagResult
					{
					*result = new QueryBagResult;
					QueryResult *r1, *r2;
					for (unsigned int i = 0; i < (((QueryBagResult *) nextResult)->size()); i++)
						{
						fprintf(stderr, "[QE] Checking element no. %d\n", i);
						if ((errcode = ((QueryBagResult *) nextResult)->at(i, r1)) != 0)
							{
							return errcode;
							}
						bool already_exists = false;
						for (unsigned int j = 0; (j < ((*result)->size())) && (!already_exists); j++) //checking if r1 is already present in result
							{
							if ((errcode = ((QueryBagResult *) nextResult)->at(j, r2)) != 0)
								{
								return errcode;
								}
							already_exists = ((r1->equal(r2)) && (i != j));
							};
						if (!already_exists)
							{
							(*result)->addResult(r1);
							fprintf(stderr, "[QE] Adding element %d to result as unique\n", i);
							}
						}
					fprintf(stderr, "[QE] Done!\n");
					return 0;
					}//case QBAG
				case QueryResult::QSTRUCT: //The argument is a QueryStructResult
					{
					*result = new QueryStructResult;
					QueryResult *r1, *r2;
					for (unsigned int i = 0; i < (((QueryStructResult *) nextResult)->size()); i++)
						{
						fprintf(stderr, "[QE] Checking element no. %d\n", i);
						if ((errcode = ((QueryStructResult *) nextResult)->at(i, r1)) != 0)
							{
							return errcode;
							}
						bool already_exists = false;
						for (unsigned int j = 0; (j < ((*result)->size())) && (!already_exists); j++) //checking if r1 is already present in result
							{
							if ((errcode = ((QueryStructResult *) nextResult)->at(j, r2)) != 0)
								{
								return errcode;
								}
							already_exists = ((r1->equal(r2)) && (i != j));
							};
						if (!already_exists)
							{
							(*result)->addResult(r1);
							fprintf(stderr, "[QE] Adding element %d to result as unique\n", i);
							}
						}
					fprintf(stderr, "[QE] Done!\n");	
					return 0;
					}//case QSTRUCT
				default: //the argument is a single value
					{
					*result = (nextResult->clone());
					fprintf(stderr, "[QE] Done!\n");
					return 0;
					}
				}//switch
				}//case DISTINCT
				
			default: {break;} // Reszta jeszcze nie zaimplementowane
			}//switch
			return -1; //Do ustalenia - nieokreslony blad (niezaimplementowana obsluga tego typu unOpa)
			}//case
	
	
		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] Algebraic operator - type recognized\n");
			QueryResult *lResult, *rResult;
			if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0) {
				return errcode;
			}
			if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0) {
				return errcode;
			}
			QueryResult *final_result;
			errcode = this->algOperate(op,lResult,rResult,final_result);
			if (errcode != 0) return errcode;
			*result = final_result;
			fprintf(stderr, "[QE] Algebraic operation Done!\n");
			return 0;
			}//case TNALGOP

		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] NonAlgebraic operator - type recognized\n");
			QueryResult *lResult;
			if ((errcode = executeQuery (((NonAlgOpNode *) tree)->getLArg(), &lResult)) != 0) {
				return errcode;
			}
			fprintf(stderr, "[QE] Left argument of NonAlgebraic query has been computed\n");
			QueryResult *partial_result = new QueryBagResult();
			if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {
				fprintf(stderr, "[QE] For each row of this score, the right argument will be computed \n");
				for (unsigned int i = 0; i < (lResult->size()); i++) {
					QueryResult *currentResult;
					QueryResult *newStackSection = new QueryBagResult();
					if ((lResult->type()) == QueryResult::QSEQUENCE)
						errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
					else
						errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
					if (errcode != 0) return errcode;
					errcode = (currentResult)->nested(tr, newStackSection);
					if (errcode != 0) return errcode;
					fprintf(stderr, "[QE] nested(): function calculated for current row number %d\n", i);
					if ( (stack.push((QueryBagResult *) newStackSection)) != 0 ) {
						return -1; //this would be very strange, this function can only return 0
					}
					QueryResult *rResult;
					if ((errcode = executeQuery (((NonAlgOpNode *) tree)->getRArg(), &rResult)) != 0) {
						return errcode;
					}
					fprintf(stderr, "[QE] Computing left Argument with a new scope of ES\n");
					errcode = this->combine(op,currentResult,rResult,partial_result);
					if (errcode != 0) return errcode;
					fprintf(stderr, "[QE] Combined partial results\n");
					errcode = stack.pop();
					if (errcode != 0) return errcode;
				}
			}
			else { //QSTRUCT, QINT, QDOUBLE, QBOOL, QSTRING, QNOTHING, QBINDER, QREFERENCE
				fprintf(stderr, "[QE] Left argument score has got only one row, the right argument will now be computed \n");
				QueryResult *newStackSection = new QueryBagResult();
				errcode = lResult->nested(tr, newStackSection);
				if (errcode != 0) return errcode;
				fprintf(stderr, "[QE] nested(): function calculated for the single row");
				if ( (stack.push((QueryBagResult *)newStackSection)) != 0 ) {
					return -1; //this would be very strange, this function can only return 0
				}
				QueryResult *rResult;
				if ((errcode = executeQuery (((NonAlgOpNode *) tree)->getRArg(), &rResult)) != 0) {
					return errcode;
				}
				fprintf(stderr, "[QE] Computing left Argument with a new scope of ES\n");
				errcode = this->combine(op,lResult,rResult,partial_result);
				if (errcode != 0) return errcode;
				fprintf(stderr, "[QE] Combined partial results\n");
				errcode = stack.pop();
				if (errcode != 0) return errcode;
				
				
			}
			QueryResult *final_result = new QueryBagResult();
			errcode = this->merge(op,partial_result, final_result);
			if (errcode != 0) return errcode;
			fprintf(stderr, "[QE] Merged partial results into final result\n");
			*result = final_result;
			fprintf(stderr, "[QE] NonAlgebraic operation Done!\n");
			return 0;
		}//case TNNONALGOP

		case TreeNode::TNTRANS: {
			// begin, abort and commit
			break;
		}

		default:
			{
			fprintf(stderr, "Unknow node type\n");
			ec << (ENoType | ErrQExecutor);
			*result = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return ENoType | ErrQExecutor;
			}

		} // end of switch
		
	} // if tree!=Null
	else // tree == NULL; return empty result
		{
		*result = new QueryNothingResult();
		fprintf(stderr, "[QE] QueryNothingResult created\n");
		}
	return 0;
    }//executeQuerry

int QueryExecutor::derefQuery(QueryResult *arg, QueryResult *&res) {
	fprintf(stderr, "[QE] derefQuery()\n");
	int errcode;
	int argType = arg->type();
	switch (argType) {
		case QueryResult::QSEQUENCE: {
			res = new QuerySequenceResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBAG: {
			res = new QueryBagResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryBagResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QSTRUCT: {
			res = new QueryStructResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryStructResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->derefQuery(((QueryBinderResult *) arg)->getItem(), tmp_item);
			if (errcode != 0) return errcode;
			res = new QueryBinderResult(tmp_name, tmp_item);
			break;
		}
		case QueryResult::QBOOL: {
			res = arg;
			break;
		}
		case QueryResult::QINT: {
			res = arg;
			break;
		}
		case QueryResult::QDOUBLE: {
			res = arg;
			break;
		}
		case QueryResult::QSTRING: {
			res = arg;
			break;
		}
		case QueryResult::QREFERENCE: {
			LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
			fprintf(stderr, "[QE] derefQuery() - dereferencing Object\n");
			ObjectPointer *optr;
			errcode = tr->getObjectPointer(ref_value, Store::Read, optr);
			if (errcode != 0) {
				fprintf(stderr, "[QE] Error in getObjectPointer\n");
				return errcode;
			}
			DataValue* value = optr->getValue();
			int vType = value->getType();
			switch (vType) {
				case Store::Integer: {
					fprintf(stderr, "[QE] derefQuery() - ObjectValue = Int\n");
					int tmp_value = value->getInt();
					res = new QueryIntResult(tmp_value);
					break;
				}
				case Store::Double: {
					fprintf(stderr, "[QE] derefQuery() - ObjectValue = Double\n");
					double tmp_value = value->getDouble();
					res = new QueryDoubleResult(tmp_value);
					break;
				}
				case Store::String: {
					fprintf(stderr, "[QE] derefQuery() - ObjectValue = String\n");
					string tmp_value = value->getString();
					res = new QueryStringResult(tmp_value);
					break;
				}
				default: {
					fprintf(stderr, "[QE] derefQuery() - wrong argument type\n");
					res = new QueryNothingResult();
					break;
				}
			}
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		default: {
			fprintf(stderr, "[QE] ERROR in derefQuery() - unknown result type\n");
			res = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return -1;
		}
	}
	if (res->isSingleValue()) {
		QueryResult *single;
		errcode = res->getSingleValue(single);
		if (errcode != 0) return errcode;
		res = single;
	}
	return 0;
}

int QueryExecutor::algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final) {
	int errcode;
	if ((op == AlgOpNode::plus) || (op == AlgOpNode::minus) || (op == AlgOpNode::times) || (op == AlgOpNode::divide)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		if ((derefL->type() == QueryResult::QINT) || (derefL->type() == QueryResult::QDOUBLE)) {
			switch (op) {
				case AlgOpNode::plus: {
					fprintf(stderr, "[QE] + operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->plus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->plus(derefR,final);
					if (errcode != 0) return errcode;
					return 0;
				}
				case AlgOpNode::minus: {
					fprintf(stderr, "[QE] - operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->minus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->minus(derefR,final);
					if (errcode != 0) return errcode;
					return 0;
				}
				case AlgOpNode::times: {
					fprintf(stderr, "[QE] * operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->times(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->times(derefR,final);
					if (errcode != 0) return errcode;
					return 0;
				}
				case AlgOpNode::divide: {
					fprintf(stderr, "[QE] / operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->divide_by(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->divide_by(derefR,final);
					if (errcode != 0) return errcode;
					return 0;
				}
				default: { break; }
			}
		}
		else {
			switch (op) {
				case AlgOpNode::plus: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! + arguments must be INT or DOUBLE\n");
					return -1;
				}
				case AlgOpNode::minus: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! - arguments must be INT or DOUBLE\n");
					return -1;
				}
				case AlgOpNode::times: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! * arguments must be INT or DOUBLE\n");
					return -1;
				}
				case AlgOpNode::divide: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! / arguments must be INT or DOUBLE\n");
					return -1;
				}
				default: { break; }
			}
		}
		return 0;
	}
	else if ((op == AlgOpNode::eq) || (op == AlgOpNode::neq) || (op == AlgOpNode::lt) || 
	(op == AlgOpNode::gt) || (op == AlgOpNode::le) || (op == AlgOpNode::ge)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		switch (op) {
			case AlgOpNode::eq: {
				fprintf(stderr, "[QE] = operation\n");
				bool bool_tmp = derefL->equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::neq: {
				fprintf(stderr, "[QE] != operation\n");
				bool bool_tmp = derefL->not_equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::lt: {
				fprintf(stderr, "[QE] < operation\n");
				bool bool_tmp = derefL->less_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::gt: {
				fprintf(stderr, "[QE] > operation\n");
				bool bool_tmp = derefL->greater_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::le: {
				fprintf(stderr, "[QE] <= operation\n");
				bool bool_tmp = derefL->less_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::ge: {
				fprintf(stderr, "[QE] >= operation\n");
				bool bool_tmp = derefL->greater_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			default : { break; }
		}
		return 0;
	} 
	else if ((op == AlgOpNode::boolOr) || (op == AlgOpNode::boolAnd)) {
		if ((lArg->isBool()) && (rArg->isBool())) {
			bool bool_l, bool_r;
			errcode = lArg->getBoolValue(bool_l);
			if (errcode != 0) return errcode;
			errcode = rArg->getBoolValue(bool_r);
			if (errcode != 0) return errcode;
			switch (op) {
				case AlgOpNode::boolOr: {
					fprintf(stderr, "[QE] OR operation\n");
					bool bool_tmp = (bool_l || bool_r);
					final = new QueryBoolResult(bool_tmp);
					return 0;
				}
				case AlgOpNode::boolAnd: {
					fprintf(stderr, "[QE] AND operation\n");
					bool bool_tmp = (bool_l && bool_r);
					final = new QueryBoolResult(bool_tmp);
					return 0;
				}
				default : { break; }
			}
		}
		else {
			switch (op) {
				case AlgOpNode::boolOr: {
					fprintf(stderr, "[QE] ERROR! OR arguments must be BOOLEAN\n");
					final = new QueryNothingResult();
					return -1;
				}
				case AlgOpNode::boolAnd: {
					fprintf(stderr, "[QE] ERROR! AND arguments must be BOOLEAN\n");
					final = new QueryNothingResult();
					return -1;
				}
				default : { break; }
			}
		}
		return 0;
	} 
	else if ((op == AlgOpNode::bagUnion) || (op == AlgOpNode::bagIntersect) || (op == AlgOpNode::bagMinus)) {
		QueryResult *leftBag = new QueryBagResult();
		QueryResult *rightBag = new QueryBagResult();
		leftBag->addResult(lArg);
		rightBag->addResult(rArg);
		switch (op) {
			case AlgOpNode::bagUnion: {
				fprintf(stderr, "[QE] BAG_UNION operation\n");
				final = rightBag;
				break;
			}
			case AlgOpNode::bagIntersect: {
				fprintf(stderr, "[QE] BAG_INTERSECT operation\n");
				final = new QueryBagResult();
				break;
			}
			case AlgOpNode::bagMinus: {
				fprintf(stderr, "[QE] BAG_MINUS operation\n");
				final = new QueryBagResult();
				break;
			}
			default: { break; }
		}
		for (unsigned int i = 0; i < leftBag->size(); i++) {
			QueryResult *tmp_res;
			errcode = ((QueryBagResult *) leftBag)->at(i, tmp_res);
			if (errcode != 0) return errcode;
			bool isIncl;
			errcode = this->isIncluded(tmp_res, rightBag, isIncl);
			if (errcode != 0) return errcode;
			switch (op) {
				case AlgOpNode::bagUnion: {
					if (not isIncl)
						final->addResult(tmp_res);
					break;
				}
				case AlgOpNode::bagIntersect: {
					if (isIncl)
						final->addResult(tmp_res);
					break;
				}
				case AlgOpNode::bagMinus: {
					if (not isIncl)
						final->addResult(tmp_res);
					break;
				}
				default: { break; }
			}
		}
		return 0;
	}
	else if (op == AlgOpNode::comma) {
		fprintf(stderr, "[QE] COMMA operation\n");
		final = new QueryBagResult();
		errcode = lArg->comma(rArg,final);
		if (errcode != 0) return errcode;
		return 0;
	}
	else if (op == AlgOpNode::insert) {
		fprintf(stderr, "[QE] INSERT operation\n");
		DataValue* value;
		vector<LogicalID*> *insVector;
		vector<ObjectPointer*>* vec;
		int argType = lArg->type();
		if (argType != QueryResult::QBAG) {  //Both arguments have to be a bags
			fprintf(stderr, "[QE] Error - wrong INSERT argument\n");
			return -1;
		}
		argType = rArg->type();
		if (argType != QueryResult::QBAG) {  //Both arguments have to be a bags
			fprintf(stderr, "[QE] Error - wrong INSERT argument\n");
			return -1;
		}
		QueryResult* toInsert;  //the object to be inserted
		unsigned int counter = lArg->size();
		if (counter != 1) {// chyba zle jesli counter<>1
			fprintf(stderr, "[QE] Wrong number of objects to be inserted.\n");
			return -1;
		}
		if ((errcode = ((QueryBagResult *) lArg)->getResult(toInsert) != 0)) {
			return errcode;
		}
		int resultType = toInsert->type();
		if (resultType != QueryResult::QREFERENCE) {
			fprintf(stderr, "[QE] Error - the bag result must consist of QREFERENCE\n");
			return -1;
		}
		LogicalID *lidIn = ((QueryReferenceResult *) toInsert)->getValue();
		ObjectPointer *optrIn;
		if ((errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn)) !=0) {
			fprintf(stderr, "[QE] Error in getObjectPointer.\n");
			return errcode;
		}
		if ((errcode = tr->removeRoot(optrIn)) != 0) {
			fprintf(stderr, "[QE] Error in removeRoot.\n");
			return errcode;
		}
		fprintf(stderr, "[QE] Root removed\n");
		QueryResult* outer; //the object into which the left agument will be inserted
		counter = rArg->size();
		if (counter != 1) { // chyba zle jesli counter<>1
			fprintf(stderr, "[QE] Wrong number of objects to be inserted.\n");
			return -1;
		}
		if ((errcode = ((QueryBagResult *) rArg)->getResult(outer) != 0)) {
			return errcode;
		}
		resultType = outer->type();
		if (resultType != QueryResult::QREFERENCE) {
			fprintf(stderr, "[QE] Error - the bag result must consist of QREFERENCE\n");
			return -1;
		}
		LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
		ObjectPointer *optrOut;
		if ((errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut)) !=0) {
			fprintf(stderr, "[QE] Error in getObjectPointer.\n");
			return errcode;
		}
		// We have to modify the "outer" object's value 
		value = optrOut->getValue();
		fprintf(stderr, "[QE] Value taken\n");
		int vType = value->getType();
		fprintf(stderr, "[QE] Type taken\n");
		if (vType != Store::Vector) {
			fprintf(stderr, "[QE] Error - the value has to be a Vector\n");
			return -1;
		}
		insVector = value->getVector();
		if (insVector == NULL) fprintf(stderr, "[QE] insVector == NULL\n");
		fprintf(stderr, "[QE] Vector taken\n");
		fprintf(stderr, "[QE] Vec.size = %d\n", vec->size());
		insVector->push_back(lidIn); // Inserting of the object
		fprintf(stderr, "[QE] Object added to value\n");
		fprintf(stderr, "[QE] New Vec.size = %d\n", vec->size());
		final = new QueryNothingResult();
		fprintf(stderr, "[QE] Done!\n");
		return 0;
	}
	else { // algOperation type not known
		fprintf(stderr, "[QE] ERROR! Algebraic operation type not known\n");
		final = new QueryNothingResult();
		return -1;
	}
	return 0;
}

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial) {
	fprintf(stderr, "[QE] combine() function applied to the partial results\n");
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <dot>\n");
			partial->addResult(lRes);
			break;
		}
		case NonAlgOpNode::join: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <join> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::where: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <where>\n");
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				if (tmp_bool_value) 
					partial->addResult(curr);
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <where> is not boolean!\n");
				return -1; // second argument of the <where> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::closeBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <closeBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <closeUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <leavesBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::orderBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <orderBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::exists: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <exists>\n");
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryBoolResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!\n");
				return -1; // second argument of the <exists> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <forAll> not imlemented yet\n");
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryBoolResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <forAll> is not boolean!\n");
				return -1; // second argument of the <forAll> operator must be boolean type
			}
			break;
		}
		default: {
			fprintf(stderr, "[QE] combine(): unknown NonAlgebraic operator!\n");
			return -1;
			break;
		}
	}
	return 0;
}

int QueryExecutor::merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final) {
	fprintf(stderr, "[QE] merge() function applied to the partial results, creating final result\n");
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <dot>\n");
			final = partial;
			break;
		}
		case NonAlgOpNode::join: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <join> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::where: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <where>\n");
			final = partial;
			break;
		}
		case NonAlgOpNode::closeBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <closeBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <closeUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <leavesBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::orderBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <orderBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::exists: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <exists>\n");
			bool tmp_bool = false;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					fprintf(stderr, "[QE] merge(): ERROR! operator <exists> expects boolean type arguments\n");
					return -1; //something went odd, partial results should be boolean type
				}
				errcode = (tmp_result->getBoolValue(current_bool));
				if (errcode != 0) return errcode;
				if (current_bool)
					tmp_bool = true;
			}
			QueryBoolResult * tmp_bool_res = new QueryBoolResult(tmp_bool);
			final->addResult(tmp_bool_res);
			break;
		}
		case NonAlgOpNode::forAll: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <forAll>\n");
			bool tmp_bool = true;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					fprintf(stderr, "[QE] merge(): ERROR! operator <exists> expects boolean type arguments\n");
					return -1; //something went odd, partial results should be boolean type
				}
				errcode = (tmp_result->getBoolValue(current_bool));
				if (errcode != 0) return errcode;
				if (not current_bool)
					tmp_bool = false;
			}
			QueryBoolResult *tmp_bool_res = new QueryBoolResult(tmp_bool);
			final->addResult(tmp_bool_res);
			break;
		}
		default: {
			fprintf(stderr, "[QE] merge(): unknown NonAlgebraic operator!\n");
			return -1;
			break;
		}
	}
	return 0;
}

int QueryExecutor::isIncluded(QueryResult *elem, QueryResult *set, bool &score) {
	fprintf(stderr, "[QE] isIncluded()\n");
	int errcode;
	if (set->type() != QueryResult::QBAG) {
		fprintf(stderr, "[QE] isIncluded(): ERROR! set argument must be BAG\n");
		return -1;
	}
	QueryResult *derefElem;
	errcode = this->derefQuery(elem,derefElem);
	if (errcode != 0) return errcode;
	bool tmp_bool = false;
	for (unsigned int i = 0; i < set->size(); i++) {
		QueryResult *tmp_res;
		errcode = ((QueryBagResult *) set)->at(i, tmp_res);
		if (errcode != 0) return errcode;
		QueryResult *derefSetElem;
		errcode = this->derefQuery(tmp_res, derefSetElem);
		if (errcode != 0) return errcode;
		bool eq = derefElem->equal(derefSetElem);
		if (eq) 
			tmp_bool = true;
	} 
	score = tmp_bool;
	return 0;
}

QueryExecutor::~QueryExecutor() { tr->abort(); stack.deleteAll(); }

EnvironmentStack::EnvironmentStack() {}
EnvironmentStack::~EnvironmentStack() {}

int EnvironmentStack::push(QueryBagResult *r) {
	es.push_back(r);
	fprintf(stderr, "[QE] Environment Stack pushed\n");
	return 0;
}

int EnvironmentStack::pop(){ 
	if (es.empty()) { 
		return -1; 
	} 
	else {
		es.pop_back();
	};
	fprintf(stderr, "[QE] Environment Stack poped\n"); 
	return 0;
}

int EnvironmentStack::top(QueryBagResult *&r) {
	if (es.empty()) {
		return -1;
	}
	else {
		r=(es.back());
	};
	return 0;
}

bool EnvironmentStack::empty() { return es.empty(); }
int EnvironmentStack::size() { return es.size(); }

int EnvironmentStack::bindName(string name, QueryResult *&r) {
	fprintf(stderr, "[QE] Name binding on ES\n");
	unsigned int number = (es.size());
	fprintf(stderr, "[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	bool found_one = false;
	QueryBagResult *section;
	unsigned int sectionSize;
	QueryResult *sth;
	string current;
	for (unsigned int i = number; i >= 1; i--) {
		section = (es.at(i - 1));
		sectionSize = (section->size());
		fprintf(stderr, "[QE] bindName: ES section %u got %u elements\n", (i - 1), sectionSize);
		for (unsigned int j = 0; j < sectionSize; j++) {
			int errNo = (section->at(j,sth));
			if (errNo != 0) { return errNo; };
			if ((sth->type()) == (QueryResult::QBINDER)) {
				current = (((QueryBinderResult *) sth)->getName());
				fprintf(stderr, "[QE] bindName: current %u name is: ", j);
				cout << current << endl;
				if (current == name) {
					found_one = true;
					fprintf(stderr, "[QE] bindName: Object added to Result\n");
					r->addResult(((QueryBinderResult *) sth)->getItem());
				};
			};
		};
		if (found_one) {
			return 0;
		};
	};
	return 0;
}

void EnvironmentStack::deleteAll() {
	for (unsigned int i = 0; i < (es.size()); i++) {
		delete es.at(i);
	};
}

}
