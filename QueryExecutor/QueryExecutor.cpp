/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>
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
			if ((errcode = tr->getRoots(name, vec)) != 0)
			{
			    return errcode;
			}
			vecSize = vec->size();
			fprintf(stderr, "[QE] Roots taken\n");
			*result = new QueryBagResult;
			fprintf(stderr, "[QE] QueryBagResult created\n");
			for (int i = 0; i < vecSize; i++ )
				{
   				optr = vec->at(i);
				lid = optr->getLogicalID();
				fprintf(stderr, "[QE] LogicalID received\n");
				QueryReferenceResult *lidres = new QueryReferenceResult(lid);
				(*result)->addResult(lidres);
				fprintf(stderr, "[QE] Object added to QueryResult\n");
				}
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
			if (tree != NULL)
				{
				nodeType = tree->type();
				fprintf(stderr, "[QE] Node type recognized.\n");
				DBDataValue *dbValue = new DBDataValue;

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
				value=NULL;
				fprintf(stderr, "[QE] No arguments (value = NULL)\n");
				}

			if ((errcode = tr->createObject(name, value, optr)) != 0)
				{
				fprintf(stderr, "[QE] Error in createObject.\n");
				return errcode;
				}
			if ((errcode = tr->addRoot(optr)) != 0)
				{
				fprintf(stderr, "[QE] Error in addRoot.\n");
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
							if ((errcode = tr->getObjectPointer(((QueryReferenceResult *)nextResult)->getValue(), Store::Read, optr)) != 0)
								{
								return errcode;
								}
					
							value = optr->getValue();
							int vType = value->getType();
							
							
/*							switch (vType)
							    {
							    case QueryResult::QINT: // it's an integer
								{
			    					if (errcode = ((QueryBagResult *)result->addResult(((QueryIntResult*) tmpResult) = (value->getInt()))) =! 0)
								    {
								    return errcode;
								    }
								}
							    case QueryResult::QDOUBLE: // it's a double
								{
			    					if (errcode = ((QueryBagResult *)result->addResult(((QueryDoubleResult*) tmpResult) = (value->getInt()))) =! 0)
								    {
								    return errcode;
								    }
								}
							    case QueryResult::QSTRING: // it's a string
								{
			    					if (errcode = ((QueryBagResult *)result->addResult(((QueryStringResult*) tmpResult) = (value->getInt()))) =! 0)
								    {
								    return errcode;
								    }
								}
							    default:
								{
								fprintf(stderr, "[QE] Error - wrong argument type\n");
								return -1;
								}
							    } //switch
*/										
							}
						else 
							{
							fprintf(stderr, "[QE] Error - the bag result must consist of logical ids\n");
							return -1;
							}
						}
					}
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
				int argType = rResult->type();
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
				int argType = rResult->type();
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
				fprintf(stderr, "[QE] Results computed, computing +\n");
				// We have to check if the arguments are of the same type
				int argType = rResult->type();
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
				fprintf(stderr, "[QE] Results computed, computing /\n");
				// We have to check if the arguments are of the same type
				int argType = rResult->type();
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
	
		case TreeNode::TNNONALGOP: {break;}
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


QueryExecutor::~QueryExecutor() { delete tr; };  // <- tu zamiast robic delete tr chyba powinnismy robic abort na tym tr i pozwolic by to TManager usunal te transakcje
}
