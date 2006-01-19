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
	
	LogicalID *lid;
	ObjectPointer *optr;
	string name;
	DataValue* value;
	int nodeType; //to tylko tymczasowo.
	vector<ObjectPointer*>* vec;
	int vecSize;
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
		nodeType = tree->type();
		fprintf(stderr, "[QE] TreeType taken\n");
	
		switch (nodeType)
		{
		case TreeNode::TNNAME:
			{
			name = tree->getName();
			fprintf(stderr, "[QE] Type: TNNAME\n");
			
			if ((stack.size()) == 1) {
			    errcode = (stack.pop());
			    if (errcode != 0) { return errcode; };
			};
			if (stack.empty()) {
				if ((errcode = tr->getRoots(vec)) != 0) {
					return errcode;
				};
				vecSize = vec->size();
				fprintf(stderr, "[QE] All %d Roots taken\n", vecSize);
				QueryBagResult *stackSection = new QueryBagResult();
				for (int i = 0; i < vecSize; i++ )
					{
   					optr = vec->at(i);
					lid = optr->getLogicalID();
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
			fprintf(stderr, "[QE] QueryBagResult created\n");
			fprintf(stderr, "[QE] Done!\n");
			return 0;
			}//case

		case TreeNode::TNCREATE:
			{
			name = tree->getName(); 
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
				vector<ObjectPointer*> emptyVector;
				dbValue->setVector(&emptyVector);
				value = dbValue;
				fprintf(stderr, "[QE] No arguments (value = NULL)\n");
				}

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
			*result = new QueryIntResult(intValue);
			fprintf(stderr, "[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
			}
		case TreeNode::TNSTRING: 
			{
			string stringValue = ((StringNode *) tree)->getValue();
			//fprintf(stderr, "[QE] QueryStringResult (%s) created\n",stringValue);
			*result = new QueryStringResult(stringValue);
			//fprintf(stderr, "[QE] QueryStringResult (%s) created\n",stringValue);
			return 0;
			}
		case TreeNode::TNDOUBLE:
			{
			double doubleValue = ((DoubleNode *) tree)->getValue();
			fprintf(stderr, "[QE] TNDOUBLE: %f\n", doubleValue);
			*result = new QueryDoubleResult(doubleValue);
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
					lid = ((QueryReferenceResult *) toDelete)->getValue();
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
				QueryResult *nextResult;
				if ((errcode = executeQuery (tree->getArg(), &nextResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Result counted, counting unMinus\n");
				if ((errcode = ((QueryIntResult *) nextResult)->minus(*result)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case unMinus
			case UnOpNode::boolNot:
				{
				fprintf(stderr, "[QE] NOT operation\n");
				QueryResult *nextResult;
				if ((errcode = executeQuery (tree->getArg(), &nextResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Result counted, counting NOT\n");
				if ((errcode = ((QueryBoolResult *) nextResult)->bool_not(*result)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case NOT
				
			case UnOpNode::deref:
				{
				fprintf(stderr, "[QE] DEREF operation\n");
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
					}//case QSTRUCT
				default: //the argument is a single value
					{
					*result = (nextResult->clone());
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
	    
			switch (op)
			{
			case AlgOpNode::plus:
				{
				fprintf(stderr, "[QE] + operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results computed, computing +\n");
				// We have to check if the arguments are of the same type
				int argType = lResult->type();
				fprintf(stderr, "[QE] left argument's type taken\n");
				switch (argType)
				{
				case QueryResult::QINT: //Left argument is a QueryIntResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QINT: //Both arguments are QueryIntResults
						{
						if ((errcode = ((QueryIntResult *) lResult)->plus(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QDOUBLE: //Right argument is a double
						{
						break;  // Trzeba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					} //case QINT
				case QueryResult::QDOUBLE: //Left argument is a QueryDoubleResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QDOUBLE: //Both arguments are QueryDoubleResults
						{
						if ((errcode = ((QueryDoubleResult *) lResult)->plus(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QINT: //Right argument is an int
						{
						break;  // Treba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					}//case QDOUBLE
				default:
					{
					fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
					return -1;
					}
				}//switch
				return 0;
				}//case
			case AlgOpNode::minus:
				{
				fprintf(stderr, "[QE] - operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results computed, computing -\n");
				// We have to check if the arguments are of the same type
				int argType = lResult->type();
				fprintf(stderr, "[QE] left argument's type taken\n");
				switch (argType)
				{
				case QueryResult::QINT: //Left argument is a QueryIntResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QINT: //Both arguments are QueryIntResults
						{
						if ((errcode = ((QueryIntResult *) lResult)->minus(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QDOUBLE: //Right argument is a double
						{
						break;  // Trzeba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					} //case QINT
				case QueryResult::QDOUBLE: //Left argument is a QueryDoubleResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QDOUBLE: //Both arguments are QueryDoubleResults
						{
						if ((errcode = ((QueryDoubleResult *) lResult)->minus(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QINT: //Right argument is an int
						{
						break;  // Treba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					}//case QDOUBLE
				default:
					{
					fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
					return -1;
					}
				}//switch
				return 0;
				}//case
			case AlgOpNode::times:
				{
				fprintf(stderr, "[QE] * operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results computed, computing *\n");
				// We have to check if the arguments are of the same type
				int argType = lResult->type();
				fprintf(stderr, "[QE] left argument's type taken\n");
				switch (argType)
				{
				case QueryResult::QINT: //Left argument is a QueryIntResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QINT: //Both arguments are QueryIntResults
						{
						if ((errcode = ((QueryIntResult *) lResult)->times(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QDOUBLE: //Right argument is a double
						{
						break;  // Trzeba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					} //case QINT
				case QueryResult::QDOUBLE: //Left argument is a QueryDoubleResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QDOUBLE: //Both arguments are QueryDoubleResults
						{
						if ((errcode = ((QueryDoubleResult *) lResult)->times(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QINT: //Right argument is an int
						{
						break;  // Trzeba jeszcze dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					}//case QDOUBLE
				default:
					{
					fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
					return -1;
					}
				}//switch
				return 0;
				}//case
			case AlgOpNode::divide:
				{
				fprintf(stderr, "[QE] / operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results computed, computing /\n");
				// We have to check if the arguments are of the same type
				int argType = lResult->type();
				fprintf(stderr, "[QE] left argument's type taken\n");
				switch (argType)
				{
				case QueryResult::QINT: //Left argument is a QueryIntResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QINT: //Both arguments are QueryIntResults
						{
						if ((errcode = ((QueryIntResult *) lResult)->divide_by(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QDOUBLE: //Right argument is a double
						{
						break;  // Trzeba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					} //case QINT
				case QueryResult::QDOUBLE: //Left argument is a QueryDoubleResult
					{
					argType = rResult->type();
					fprintf(stderr, "[QE] right argument's type taken\n");
					switch (argType)
					{
					case QueryResult::QDOUBLE: //Both arguments are QueryDoubleResults
						{
						if ((errcode = ((QueryDoubleResult *) lResult)->divide_by(rResult, (*result))) != 0)
							{
							return errcode;
							}
						fprintf(stderr, "[QE] Done!\n");
						return 0;
						}
					case QueryResult::QINT: //Right argument is an int
						{
						break;  // Trzeba jeszce dopisac
						}
					default:
						{
						fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
						return -1;
						}
					} //switch
					return 0;
					}//case QDOUBLE
				default:
					{
					fprintf(stderr, "[QE] Error - wrong AlgOp argument\n");
					return -1;
					}
				}//switch
				return 0;
				}//case
			case AlgOpNode::eq:
				{
				fprintf(stderr, "[QE] = operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting =\n");
				bool tmp_res_bool = lResult->equal(rResult);
				*result = new QueryBoolResult(tmp_res_bool);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case
			case AlgOpNode::neq:
				{
				fprintf(stderr, "[QE] != operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting !=\n");
				bool tmp_res_bool = lResult->not_equal(rResult);
				*result = new QueryBoolResult(tmp_res_bool);
				fprintf(stderr, "[QE] Done!\n");
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case
			case AlgOpNode::gt:
				{
				fprintf(stderr, "[QE] > operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting >\n");
				bool tmp_res_bool = lResult->greater_than(rResult);
				*result = new QueryBoolResult(tmp_res_bool);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case
			case AlgOpNode::lt:
				{
				fprintf(stderr, "[QE] < operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting <\n");
				bool tmp_res_bool = lResult->less_than(rResult);
				*result = new QueryBoolResult(tmp_res_bool);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case
			case AlgOpNode::ge:
				{
				fprintf(stderr, "[QE] >= operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting >=\n");
				bool tmp_res_bool = lResult->greater_eq(rResult);
				*result = new QueryBoolResult(tmp_res_bool);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case
			case AlgOpNode::le:
				{
				fprintf(stderr, "[QE] >= operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting >=\n");
				bool tmp_res_bool = lResult->less_eq(rResult);
				*result = new QueryBoolResult(tmp_res_bool);
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case
			case AlgOpNode::boolAnd:
				{
				fprintf(stderr, "[QE] AND operation\n");
				QueryResult *lResult, *rResult;
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting AND\n");
				if ((errcode = ((QueryBoolResult *) lResult)->bool_and(rResult, (*result))) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Done!\n");					
				return 0;
				}//case AND
			case AlgOpNode::boolOr:
				{
				fprintf(stderr, "[QE] OR operation\n");
				QueryResult *lResult, *rResult; 
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Results counted, counting OR\n");
				if ((errcode = ((QueryBoolResult *) lResult)->bool_or(rResult, (*result))) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case OR
			case AlgOpNode::insert:
				{
				
				fprintf(stderr, "[QE] INSERT operation\n");                                                  /******************/
				QueryResult *lResult, *rResult; 
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0)
					{
					return errcode;
					}
				int argType = lResult->type();
				fprintf(stderr, "[QE] left argument's type taken\n");
				if (argType!=QueryResult::QBAG) //Both arguments have to be a bags
					{
					fprintf(stderr, "[QE] Error - wrong INSERT argument\n");
					return -1;
					}
				if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0)
					{
					return errcode;
					}
				argType = rResult->type();
				fprintf(stderr, "[QE] right argument's type taken\n");
				if (argType!=QueryResult::QBAG) //Both arguments have to be a bags
					{
					fprintf(stderr, "[QE] Error - wrong INSERT argument\n");
					return -1;
					}
				
				// Both arguments are bags
					
				QueryResult* toInsert;  //the object to be inserted
				unsigned int counter = lResult->size();
				if (counter != 1) // chyba zle jesli counter<>1 ????????????????????????????????????????????????????????????????????????????????
					{
					fprintf(stderr, "[QE] Wrong number of objects to be inserted.\n");
					return -1;
					}
				if ((errcode = ((QueryBagResult *) lResult)->getResult(toInsert) != 0))
					{
					return errcode;
					}
				int resultType = toInsert->type();
				if (resultType != QueryResult::QREFERENCE)
					{
					fprintf(stderr, "[QE] Error - the bag result must consist of QREFERENCE\n");
					return -1;
					}
				LogicalID *lidIn = ((QueryReferenceResult *) toInsert)->getValue();
				ObjectPointer *optrIn;
				if ((errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn)) !=0)
					{
					fprintf(stderr, "[QE] Error in getObjectPointer.\n");
					return errcode;
					}
				if ((errcode = tr->removeRoot(optrIn)) != 0)
					{
					fprintf(stderr, "[QE] Error in removeRoot.\n");
					return errcode;
					}
				fprintf(stderr, "[QE] Root removed\n");
				
				
				//the object into which the left agument will be inserted
				QueryResult* outer; 
				counter = rResult->size();
				if (counter != 1) // chyba zle jesli counter<>1 ????????????????????????????????????????????????????????????????????????????????
					{
					fprintf(stderr, "[QE] Wrong number of objects to be inserted.\n");
					return -1;
					}
				if ((errcode = ((QueryBagResult *) rResult)->getResult(outer) != 0))
					{
					return errcode;
					}
				resultType = outer->type();
				if (resultType != QueryResult::QREFERENCE)
					{
					fprintf(stderr, "[QE] Error - the bag result must consist of QREFERENCE\n");
					return -1;
					}
				LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
				ObjectPointer *optrOut;
				if ((errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut)) !=0)
					{
					fprintf(stderr, "[QE] Error in getObjectPointer.\n");
					return errcode;
					}
				// We have to modify the "outer" object's value 
				value = optrOut->getValue();
				fprintf(stderr, "[QE] Value taken\n");
				int vType = value->getType();
				fprintf(stderr, "[QE] Type taken\n");
				if (vType != Store::Vector) 
					{
					fprintf(stderr, "[QE] Error - the value has to be a Vector\n");
					return -1;
					}
				vec = value->getVector();
				
				vector<ObjectPointer*> aaa;
				
				if (vec == NULL) fprintf(stderr, "Ale gupie\n");
				fprintf(stderr, "[QE] Vector taken\n");
				fprintf(stderr, "[QE] Vec.size = %d\n", vec->size());
				vec->push_back(optrIn); // Inserting of the object
				fprintf(stderr, "[QE] Object added to value\n");
				fprintf(stderr, "[QE] New Vec.size = %d\n", vec->size());
				
				*result = new QueryNothingResult;
				fprintf(stderr, "[QE] QueryNothingResult created\n");
				fprintf(stderr, "[QE] Done!\n");
				return 0;
				}//case INSERT INTO
			default:
				{
				fprintf(stderr, "[QE] Unknown AlgOp type\n");
				return -1;
				} // Reszta jeszcze nie zaimplementowane
			}//switch
			*result = new QueryNothingResult;
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			fprintf(stderr, "[QE] Done!\n");
			return 0;
			}//case
	
		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] NonAlgebraic operator - type recognized\n");
			QueryResult *lResult;
			if ((errcode = executeQuery (((NonAlgOpNode *) tree)->getLArg(), &lResult)) != 0) {
				return errcode;
			}
			fprintf(stderr, "[QE] Left argument of NonAlgebraic querry has been computed\n");
			QueryResult *partial_result = new QueryBagResult();
			QueryResult *final_result;
			if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {
				fprintf(stderr, "[QE] For each row of this score, the right argument will be computed \n");
				for (unsigned int i = 0; i < (lResult->size()); i++) {
					QueryResult *currentResult;
					QueryResult *newStackSection;
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
				fprintf(stderr, "[QE] The score has got only one row, the right argument will now be computed \n");
				QueryResult *newStackSection;
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
			errcode = this->merge(op,partial_result,final_result);
			if (errcode != 0) return errcode;
			fprintf(stderr, "[QE] Merged partial results into final result\n");
			*result = final_result;
			fprintf(stderr, "[QE] NonAlgebraic operation Done!\n");
			return 0;
		}//case TNNONALGOP
		
		case TreeNode::TNTRANS: {break;}
	
		default:
			{
			fprintf(stderr, "Unknow node type\n");
			ec << (ENoType | ErrQExecutor);
			return ENoType | ErrQExecutor;
			}

		} // end of switch
		
	} // if tree!=Null
	else // tree == NULL; return empty result
		{
		*result = new QueryNothingResult;
		fprintf(stderr, "[QE] QueryNothingResult created\n");
		}
	return 0;
    }

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial) {
	fprintf(stderr, "[QE] combine() function applied to the partial results\n");
	//int errcode;
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
			if ((lRes->type()) == QueryResult::QBOOL) {
				if (((QueryBoolResult *) lRes)->getValue())
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
			if ((lRes->type()) == QueryResult::QBOOL) {
				if (((QueryBoolResult *) lRes)->getValue()) {
					QueryBoolResult *tmp_result = new QueryBoolResult(true); 
					partial->addResult(tmp_result);
				}
				else {
					QueryBoolResult *tmp_result = new QueryBoolResult(false); 
					partial->addResult(tmp_result);
				}
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!\n");
				return -1; // second argument of the <exists> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <forAll> not imlemented yet\n");
			if ((lRes->type()) == QueryResult::QBOOL) {
				if (((QueryBoolResult *) lRes)->getValue()) {
					QueryBoolResult *tmp_result = new QueryBoolResult(true); 
					partial->addResult(tmp_result);
				}
				else {
					QueryBoolResult *tmp_result = new QueryBoolResult(false); 
					partial->addResult(tmp_result);
				}
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
			final = new QueryBagResult();
			for (unsigned i = 0; i > (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				final->addResult(tmp_result);
				if (errcode != 0) return errcode;
			}
			break;
		}
		case NonAlgOpNode::join: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <join> not imlemented yet\n");
			final = new QueryBagResult();
			break;
		}
		case NonAlgOpNode::where: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <where>\n");
			final = new QueryBagResult();
			for (unsigned i = 0; i > (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				final->addResult(tmp_result);
				if (errcode != 0) return errcode;
			}
			break;
		}
		case NonAlgOpNode::closeBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <closeBy> not imlemented yet\n");
			final = new QueryBagResult();
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <closeUniqueBy> not imlemented yet\n");
			final = new QueryBagResult();
			break;
		}
		case NonAlgOpNode::leavesBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <leavesBy> not imlemented yet\n");
			final = new QueryBagResult();
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet\n");
			final = new QueryBagResult();
			break;
		}
		case NonAlgOpNode::orderBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <orderBy> not imlemented yet\n");
			final = new QueryBagResult();
			break;
		}
		case NonAlgOpNode::exists: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <exists> not imlemented yet\n");
			bool tmp_bool = false;
			for (unsigned i = 0; i > (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if ((tmp_result->type()) != QueryResult::QBOOL) {
					fprintf(stderr, "[QE] merge(): ERROR! operator <exists> expects boolean type arguments\n");
					return -1; //something went odd, partial results should be boolean type
				}
				if (((QueryBoolResult *) tmp_result)->getValue())
					tmp_bool = true;
				if (errcode != 0) return errcode;
			}
			final = new QueryBoolResult(tmp_bool);
			break;
		}
		case NonAlgOpNode::forAll: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <forAll> not imlemented yet\n");
			bool tmp_bool = true;
			for (unsigned i = 0; i > (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if ((tmp_result->type()) != QueryResult::QBOOL) {
					fprintf(stderr, "[QE] merge(): ERROR! operator <forAll> expects boolean type arguments\n");
					return -1; //something went odd, partial results should be boolean type
				}
				if (not (((QueryBoolResult *) tmp_result)->getValue()))
					tmp_bool = false;
				if (errcode != 0) return errcode;
			}
			final = new QueryBoolResult(tmp_bool);
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
