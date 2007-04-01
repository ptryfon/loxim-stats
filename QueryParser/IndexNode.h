#ifndef INDEXNODE_H_
#define INDEXNODE_H_

#include "TreeNode.h"
#include "../Indexes/IndexManager.h"

using namespace QParser;
using namespace std;

namespace Indexes {
	

	
    class IndexNode : public TreeNode
    {
    	
    	protected:
    		ErrorConsole *ec;
    		
    		//IndexNode() { im = IndexManager::getHandle(); }
    		//getEc()
    	
    	public:
    		IndexManager *im;
    		virtual int execute(QueryResult **result)=0;//{return 0;};
    		//IndexNode(){}
    		virtual int type()=0;// {return TreeNode::TNINDEXDDL;}
    		virtual int putToString() {return 0;}
    		virtual ~IndexNode() {}
    		//virtual TreeNode* clone() {return new IndexNode();}
    };
    
    //data definition language
    class IndexDDLNode : public IndexNode {
    	virtual int type() {return TreeNode::TNINDEXDDL;}
    };
    
    // create index emp_index on emp ( surname )
    class CreateIndexNode : public IndexDDLNode
    {
    	private:
    		string indexName,			// nazwa indeksu 
    			   indexedRootName,		// jakie rooty indksujemy
    			   indexedFieldName;	// po jakim polu indeksujemy
    		//CreateIndexNode(){}
    	
    	public:
	    	CreateIndexNode(string _indexName, string _indexedRootName, string _indexedFieldName)
	    				: indexName(_indexName), indexedRootName(_indexedRootName),	indexedFieldName(_indexedFieldName) {}
	    	virtual TreeNode* clone() {return new CreateIndexNode(indexName, indexedRootName, indexedFieldName);}
	    	virtual int putToString() {cout << "create index" << indexName << " on " << indexedRootName << "(" << indexedFieldName<< ")" << endl; return 0;}
	    	virtual int execute(QueryResult **result){return im->createIndex(indexName, indexedRootName, indexedFieldName, result);};
	    	virtual ~CreateIndexNode(){};
	    	//virtual TreeNode* clone() {return new CreateIndexNode();}
    };
    
    class ListIndexNode : public IndexDDLNode
    {
    	public:
    		ListIndexNode(){};
    		virtual ~ListIndexNode(){};
    		virtual int execute(QueryResult **result) {return im->listIndex(result);};	
    };
    
    class DropIndexNode : public IndexDDLNode
    {
    	public:
    		DropIndexNode(){};
    		virtual ~DropIndexNode(){};
    		virtual int execute(string indexName, QueryResult **result) {return im->dropIndex(indexName, result);};	
    };
}

#endif /*INDEXNODE_H_*/
