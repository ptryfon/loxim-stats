#include <QueryExecutor/InterfaceQuery.h>
#include <QueryParser/TreeNode.h>
#include <Errors/ErrorConsole.h>

using namespace Schemas;
using namespace QParser;
using namespace QExecutor;
using namespace Errors;

// InterfaceObjectName -> ImplementationObjectName.(interface fields) as InterfaceObjectName
int InterfaceQuery::produceQuery(Schema *interfaceSchema, ErrorConsole *ec, const string& classInvariantName, QueryNode *& query)
{
    NameNode *classInvariantNameNode = new NameNode(classInvariantName);
    
    //TODO - 0 fields!
    //TODO - no class invariant name (? - check earlier)
    
    QueryNode *selectionQuery = NULL;
    constructSelection(interfaceSchema, ec, selectionQuery);
    ec->printf("InterfaceQuery::produceQuery> selectionQuery ready: %s\n", selectionQuery->toString(0, true).c_str());
    
    QueryNode *dotQuery = new NonAlgOpNode(classInvariantNameNode, selectionQuery, NonAlgOpNode::dot);
    string interfaceObjName = interfaceSchema->getAssociatedObjectName();    
    query = new NameAsNode(dotQuery, interfaceObjName, false);

    ec->printf("InterfaceQuery::produceQuery> whole query ready: %s\n", query->toString(0, true).c_str());
    return 0;
}

int InterfaceQuery::constructSelection(Schema *interfaceSchema, ErrorConsole *ec, QueryNode *& selectionQuery)
{
    TFields interfaceFields = interfaceSchema->getFields();
    TFields::reverse_iterator it;
    string lastName("");
    QueryNode *algOpN = NULL;
    
    for (it = interfaceFields.rbegin(); it != interfaceFields.rend(); ++it)
    {
	Field *f = (*it);	
	string currentName = f->getName();
	//ec->printf("InterfaceQuery::constructSelection> processing name: %s\n", currentName.c_str());
	if (!algOpN)
	{
	    if (lastName.empty())
	    {
		lastName = currentName;
	    }
	    else
	    {
		QueryNode *lastNameNode = new NameNode(lastName);
		NameAsNode *lastNameAsNode = new NameAsNode(lastNameNode, lastName, false);

		QueryNode *currentNameNode = new NameNode(currentName);
		NameAsNode *currentNameAsNode = new NameAsNode(currentNameNode, currentName, false);
		algOpN = new AlgOpNode(currentNameAsNode, lastNameAsNode, AlgOpNode::comma);
		//ec->printf("InterfaceQuery::constructSelection> algOpN: %s\n", algOpN->toString(0, true).c_str());
	    }
	}
	else
	{
	    QueryNode *currentNameNode = new NameNode(currentName);
	    NameAsNode *currentNameAsNode = new NameAsNode(currentNameNode, currentName, false);
	    algOpN = new AlgOpNode(currentNameAsNode, algOpN, AlgOpNode::comma);
	    //ec->printf("InterfaceQuery::constructSelection> algOpN c.d: %s\n", algOpN->toString(0, true).c_str());
	}
    }
    selectionQuery = algOpN;
    
    //ec->printf("InterfaceQuery::constructSelection> selection query is: %s\n", selectionQuery->toString(0, true).c_str());
    
    if ((!algOpN) && (!lastName.empty()))
	selectionQuery = new NameNode(lastName);
    
    
    return 0;
}
