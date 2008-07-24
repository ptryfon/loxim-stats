#ifndef INDEXNODE_H_
#define INDEXNODE_H_

#include "TreeNode.h"
#include "../Indexes/IndexManager.h"
#include "../QueryExecutor/QueryResult.h"
#include "Indexes/BTree.h"

using namespace Indexes;
using namespace QExecutor;
using namespace std;

namespace QParser {
//namespace Indexes {


	/*
    class IndexNode : public TreeNode
    {

    	protected:
    		ErrorConsole *ec;

    		//IndexNode() { im = IndexManager::getHandle(); }
    		//getEc()

    	public:

    		//IndexNode(){}
    		virtual int type()=0;// {return TreeNode::TNINDEXDDL;}
    		virtual int putToString() {return 0;}
    		virtual ~IndexNode() {}
    		//virtual TreeNode* clone() {return new IndexNode();}
    };
    */
    class Indexes::Comparator;

    //data definition language
    class IndexDDLNode : public TreeNode {
    	public:
    	virtual int execute(int sessionId, QueryResult **result)=0;//{return 0;};
    	virtual int type() {return TreeNode::TNINDEXDDL;}
    };



    class ListIndexNode : public IndexDDLNode
    {
    	public:
    		ListIndexNode(){};
    		virtual ~ListIndexNode(){};
    		virtual int execute(int sessionId,QueryResult **result) {return IndexManager::getHandle()->listIndex(result);};

    		virtual TreeNode* clone() {return new ListIndexNode();}
	    	virtual int putToString() {cout << "list index" << endl; return 0;}
    };

    class DropIndexNode : public IndexDDLNode
    {
    	private:
    		string indexName;

    	public:
    		DropIndexNode(string indexName) : indexName(indexName) {}
    		virtual ~DropIndexNode(){};
    		virtual int execute(int sessionId,QueryResult **result) {return IndexManager::getHandle()->dropIndex(sessionId, indexName, result);};

    		virtual TreeNode* clone() {return new DropIndexNode(indexName) ;}
	    	virtual int putToString() {cout << "drop index " << indexName << endl; return 0;}
    };


//  create index emp_index on emp ( surname )
    class CreateIndexNode : public IndexDDLNode
    {
    	private:
    		string indexName,			// nazwa indeksu
    			   indexedRootName,		// jakie rooty indksujemy
    			   indexedFieldName;	// po jakim polu indeksujemy
    		Comparator* comp;
    		//CreateIndexNode(){}

    	public:
	    	CreateIndexNode(string _indexName, string _indexedRootName, string _indexedFieldName, Comparator* comp)
	    				: indexName(_indexName), indexedRootName(_indexedRootName),	indexedFieldName(_indexedFieldName), comp(comp) {}
	    	virtual TreeNode* clone() {return new CreateIndexNode(indexName, indexedRootName, indexedFieldName, comp);}
	    	virtual int putToString() {cout << "create index " << indexName << " on " << indexedRootName << "(" << indexedFieldName<< ")" << endl; return 0;}
	    	virtual int execute(int sessionId,QueryResult **result){return IndexManager::getHandle()->createIndex(sessionId, indexName, indexedRootName, indexedFieldName, comp, result);};
	    	virtual ~CreateIndexNode(){};
	    	//virtual TreeNode* clone() {return new CreateIndexNode();}
    };


}

#endif /*INDEXNODE_H_*/
