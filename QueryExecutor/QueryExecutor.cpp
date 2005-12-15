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
				for (unsigned int i = 0; i < (nextResult->size()); i++ ) // Deleting objects
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
				fprintf(stderr, "[QE] Done!\n");
				} //case DELETE
			case UnOpNode::unMinus:
				{
				break;
				}//case
			case UnOpNode::boolNot:
				{
				fprintf(stderr, "[QE] NOT operation\n");
				QueryResult *nextResult;
				if ((errcode = executeQuery (tree->getArg(), &nextResult)) != 0)
					{
					return errcode;
					}
				fprintf(stderr, "[QE] Result counted, counting NOT\n");
				*result = ((QueryBoolResult *) nextResult)->bool_not();
				if ((*result)->type() == QueryResult::QNOTHING)
				{
					// ec << 
					return -1;
				}
				return 0;
				}//case NOT
			default: {break;} // Reszta jeszcze nie zaimplementowane
			}//switch
			*result = new QueryNothingResult;
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return 0;
			}//case
	
	
		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] Algebraic operator - type recognized\n");
	    
			switch (op)
			{
			case AlgOpNode::plus:
				{
				break;
				}//case
			case AlgOpNode::minus:
				{
				break;
				}//case
			case AlgOpNode::times:
				{
				break;
				}//case
			case AlgOpNode::divide:
				{
				break;
				}//case
			case AlgOpNode::eq:
				{
				break;
				}//case
			case AlgOpNode::neq:
				{
				break;
				}//case
			case AlgOpNode::gt:
				{
				break;
				}//case
			case AlgOpNode::lt:
				{
				break;
				}//case
			case AlgOpNode::ge:
				{
				break;
				}//case
			case AlgOpNode::le:
				{
				break;
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
				*result = ((QueryBoolResult *) lResult)->bool_and(rResult);
				if ((*result)->type() == QueryResult::QNOTHING)
				{
					// ec << 
					return -1;
				}
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
				*result = ((QueryBoolResult *) lResult)->bool_or(rResult);
				if ((*result)->type() == QueryResult::QNOTHING)
				{
					// ec << 
					return -1;
				}
				return 0;
				}//case OR
			default: {break;} // Reszta jeszcze nie zaimplementowane
			}//switch
			*result = new QueryNothingResult;
			fprintf(stderr, "[QE] QueryNothingResult created\n");
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
