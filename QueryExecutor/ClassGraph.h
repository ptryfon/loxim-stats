#ifndef _CLASSGRAPH_H_
#define _CLASSGRAPH_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
   
#include "QueryExecutor.h"
#include "QueryResult.h"
#include "EnvironmentStack.h"
#include "SessionData.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/Privilige.h"
#include "QueryParser/TreeNode.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace SessionDataNms;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;


namespace QExecutor
{
	class ClassGraphVertex;
	
	typedef hash_map<LogicalID*, ClassGraphVertex*, hashLogicalID, eqLogicalID> MapOfClassVertices;
	
	class hashString {
	private:
		hash<const char*> hasher;
		
	public:
		size_t operator()(const string& s) const {
			return hasher(s.c_str());
		}
	};
	
	struct eqString{
		bool operator()(const string& s1, const string& s2) const{
			return s1 == s2;
		}
	};
	
	typedef hash_map<string, SetOfLids*, hashString, eqString> MapOfInvariantVertices;
	
	typedef hash_set<string, hashString, eqString> stringHashSet;
	
	/**
	 * Wszystkie LogicalID* jakie zawieraja obiekty tej klasy sa niszczone i tworzone w klasie ClassGraph
	 * i są przechowywane w ClassGraph::classGraph.
	 */
	class ClassGraphVertex {
	public:
		SetOfLids extends;
		SetOfLids subclasses;
		string invariant;
		string name;
		
		ClassGraphVertex(){};
		virtual ~ClassGraphVertex(){};
		virtual int initNotGraphProperties(DataValue* classData, Transaction *&tr, QueryExecutor *qe);
		virtual void addSubclass(LogicalID* lid) { subclasses.insert(lid); }
		virtual void addExtend(LogicalID* lid) { extends.insert(lid); }
	};
	
	class ClassGraph {
	private:
		ErrorConsole *ec;
		MapOfClassVertices classGraph;
		MapOfInvariantVertices invariants;
	protected:
		virtual int fetchSubInvariantNames(string& invariantName, stringHashSet& invariantsNames);
		
		virtual int fetchSubInvariantNames(ClassGraphVertex* cgvIn, stringHashSet& invariantsNames);
	public:
		virtual ~ClassGraph();
		ClassGraph() {ec = new ErrorConsole("QueryExecutor");};
		virtual void putToInvariants(string& invariantName, LogicalID* lid);
		
		virtual int fetchSubInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames, bool noInvariantName = true);
		
		virtual int completeSubgraph(LogicalID* lid, Transaction *&tr, QueryExecutor *qe);
		
		virtual int invariantToCohesiveSubgraph(string& invariantName, Transaction *&tr, QueryExecutor *qe);
		
		virtual bool vertexExist(LogicalID* lid) {
			return classGraph.find(lid) != classGraph.end();
		}
		
		virtual int getVertex(LogicalID* lid, ClassGraphVertex*& cgv) {
			cgv = classGraph[lid];
			return 0;
		}
		
		virtual int belongsToInvariant(SetOfLids* extClasses, string& invariantUpName, bool& inInvariant, bool isObject = true);
		
		// Metoda nie buduje grafu dla invariantUpName.
		virtual int belongsToInvariant(LogicalID* lid, string& invariantUpName, Transaction *&tr, QueryExecutor *qe, bool& inInvariant);
	};
}


#endif /*_CLASSGRAPH_H_*/
