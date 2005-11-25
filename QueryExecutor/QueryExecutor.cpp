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
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "QueryExecutor.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;

namespace QExecutor {

int QueryExecutor::executeQuery(TreeNode *tree, QueryResult **result) {
	Transaction *tr;
	LogicalID *lid;
	//AccessMode mode;
	ObjectPointer *optr;
	string name;
	DataValue* value;
	int nodeType; //to tylko tymczasowo.
	vector<ObjectPointer*>* vec;
	int vecSize;
	ErrorConsole ec;
	int errcode;
	  
	fprintf(stderr, "QueryExecutor method: queryResult\n");
	fprintf(stderr, "QueryExecutor asking TransactionManager for a new transaction\n");
	
	if (TransactionManager::getHandle()->createTransaction(tr) != 0) {
		fprintf(stderr, "Error in createTransaction\n");
	}
	
	nodeType = tree->type();
fprintf(stderr, "Otwarta transakcja, wzialem typ.\n");
	switch (nodeType) 
	
	{
	
	case TreeNode::TNNAME:
	
		{
		name = tree->getName();
		fprintf(stderr, "Typ: TNNAME\n");
		if ((errcode = tr->getRoots(name, vec)) != 0)
			{
			ec << (errcode);
			return -1; //EGetRoots | ErrQExecutor;
			}
		vecSize = vec->size();
		fprintf(stderr, "Wziete rootsy\n");
		QueryBagResult *resbag = new QueryBagResult;
		fprintf(stderr, "Jest worek\n");
		for (int i = 0; i < vecSize; i++ )
			{
   			optr = vec->at(i);
			lid = optr->getLogicalID();
			QueryReferenceResult *lidres = new QueryReferenceResult(lid);
			resbag->addResult(lidres);
			fprintf(stderr, "Dolozylem obiekt\n");
			}
		*result = (QueryResult *) resbag;
		fprintf(stderr, "Koncze\n");
		return 0;
		}

	case TreeNode::TNCREATE: 
		    
		{
		name = tree->getName();
		tree = tree->getArg();
		if (tree != NULL)
			{
			nodeType = tree->type();
			DBDataValue *dbValue;
			//dbValue = DBDataValue::new;			
			
			switch (nodeType) 
			{
			case TreeNode::TNINT: 
				{
				int intValue = ((IntNode *) tree)->getValue();
				dbValue->setInt(intValue);
				break;
				}
			
			case TreeNode::TNSTRING: 
				{
				string stringValue = ((StringNode *) tree)->getValue();
				dbValue->setString(stringValue);
				break;
				}

			case TreeNode::TNDOUBLE: 
				{
				double doubleValue = ((DoubleNode *) tree)->getValue();
				dbValue->setDouble(doubleValue);
				break;
				} 
		
			default: 
				{
				ec << (EBadType | ErrQExecutor);
				return EBadType | ErrQExecutor;
				}
			
			}
			
			value = dbValue;
			
			
			}
		else 	
			{
			value=NULL;
			}
		
		if ((errcode = tr->createObject(name, value, optr)) != 0)
			{
			ec << (errcode);
			return EObjCreate | ErrQExecutor;
			}
		if (tr->addRoot(optr) != 0)
			{
			ec << (ERootAdd | ErrQExecutor);
			return ERootAdd | ErrQExecutor;
			}
		*result = (QueryResult *) (new QueryNothingResult);
		return 0;
		}
		
	
	case TreeNode::TNINT: {break;}
	case TreeNode::TNSTRING: {break;}
	case TreeNode::TNDOUBLE: {break;} 
	case TreeNode::TNVECTOR: {break;}
	case TreeNode::TNAS: {break;}
	case TreeNode::TNUNOP: {break;}
	case TreeNode::TNALGOP: {break;}
	case TreeNode::TNNONALGOP: {break;}
	case TreeNode::TNTRANS: {break;}
	
	default:
		{
		ec << (ENoType | ErrQExecutor);
		return ENoType | ErrQExecutor;
		}
	
	} // end of switch
	return 0;
    }


QueryExecutor::~QueryExecutor() {};
}
