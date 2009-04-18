#ifndef _CLASSGRAPH_H_
#define _CLASSGRAPH_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

namespace QExecutor {
	class ClassGraph;
}

#include <QueryExecutor/QueryExecutor.h>
#include <QueryExecutor/QueryResult.h>
#include <QueryExecutor/EnvironmentStack.h>
#include <TransactionManager/Transaction.h>
#include <Store/Store.h>
#include <QueryParser/Privilige.h>
#include <QueryParser/TreeNode.h>
#include <Errors/Errors.h>
#include <Errors/ErrorConsole.h>

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;


namespace QExecutor
{
	class ClassGraphVertex;

	class Method {
	public:
		LogicalID* lid;
		vector<string> params;
		string code;
		Method(LogicalID* lid) { this->lid = lid; }
		virtual ~Method() { delete lid; }
	};

	typedef hash_map<unsigned int, Method*> ArgsCountToMethodMap;

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

	typedef hash_map<string, ArgsCountToMethodMap*, hashString, eqString> NameToArgCountToMethodMap;

	typedef hash_map<string, SetOfLids*, hashString, eqString> MapOfInvariantVertices;

	typedef hash_set<string, hashString, eqString> stringHashSet;

	typedef hash_map<string, LogicalID*, hashString, eqString> NameToLidMap;

	//typedef hash_map<string, LogicalID*, hashString, eqString> MapOfProcedure;

	/**
	 * Wszystkie LogicalID* jakie zawieraja obiekty tej klasy sa niszczone i tworzone w klasie ClassGraph
	 * i są przechowywane w ClassGraph::classGraph.
	 */
	class ClassGraphVertex {
	protected:
		virtual int fetchMethod(LogicalID* lid, Transaction *&tr, QueryExecutor *qe, Method*& method, unsigned int & params_count, string& name);
		virtual int insertIntoMethods(string& name, unsigned int argsCount, Method* m);
		virtual int putMethod(LogicalID* lid, Transaction *&tr, QueryExecutor *qe);
		virtual string fieldsToString(stringHashSet& fields);
		virtual string methodsToString();
	public:

		SetOfLids extends;
		SetOfLids subclasses;
		NameToArgCountToMethodMap methods;
		stringHashSet fields;
		stringHashSet staticFields;
		string invariant;
		string name;
		bool invalid;

		ClassGraphVertex():invalid(false){};
		virtual ~ClassGraphVertex();
		virtual int initNotGraphProperties(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe);
		virtual void addSubclass(LogicalID* lid) { subclasses.insert(lid); }
		virtual void addExtend(LogicalID* lid) { extends.insert(lid); }
		virtual void removeSubclass(LogicalID* lid) { subclasses.erase(lid); }
		virtual void removeExtend(LogicalID* lid) { extends.erase(lid); }
		virtual string toString(ClassGraph* cg);
		virtual int getMethod(const string& name, unsigned int argsCount, Method*& method, bool& found);
		virtual bool hasStaticField(string& staticField);

		static string classSetToString(ClassGraph* cg, SetOfLids* classSet);
	};

	class ClassGraph {
	private:
		//ErrorConsole *ec;
		bool lazy;
		MapOfClassVertices classGraph;
		MapOfInvariantVertices invariants;
		NameToLidMap nameIndex;
		unsigned int separatorLen;
		static ClassGraph* handle;

	protected:

		enum SearchPhase { SEARCHING, SKIP_ONE_STEP, SEARCHING_START_CLASS };

		virtual string lastPartFromExtName(const string& extName, bool& isExtName);

		virtual string firstPartFromExtName(const string& extName, bool& isExtName);

		virtual void removeFromInvariant(LogicalID* lid, string name);

		virtual void removeFromSubclasses(SetOfLids* extendedClasses, LogicalID* lid);

		virtual void removeFromExtends(SetOfLids* subclasses, LogicalID* lid);

		virtual int fetchInvariantNames(string& invariantName, stringHashSet& invariantsNames, bool sub);

		virtual int fetchInvariantNames(ClassGraphVertex* cgvIn, stringHashSet& invariantsNames, bool sub);

		virtual int fetchInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames, bool noInvariantName, bool sub);

		virtual int findMethod(string name, unsigned int argsCount, SetOfLids* classesToSearch, Method*& method, bool& found, LogicalID* classLidToStartSearching, SearchPhase searchStarted, LogicalID*& bindClassLid);

		virtual int findMethod(string name, unsigned int argsCount, SetOfLids* classesToSearch, Method*& method, bool& found, LogicalID* actualBindClassLid, LogicalID*& bindClassLid);

		virtual void putToInvariants(string& invariantName, LogicalID* lid);

		virtual void putToNameIndex(string& className, LogicalID* classLid);

		virtual bool classExist(const string& className) const {return nameIndex.find(className) != nameIndex.end();};

		virtual void removeFromNameIndex(string& className);

		virtual int classBelongsToExtSubGraph(const MapOfClassVertices::iterator& classI, const MapOfClassVertices::iterator& subGraphI, bool& belongs);

		ClassGraph();

	public:
		static ErrorConsole *ec;

		static int trErrorOccur(QueryExecutor* qe, string msg, int errcode );

		static int getHandle(ClassGraph*& cg);

		static int init(int sessionId);

		static void shutdown();

		static string classNameToExtPrefix(const string& className);

		static bool isExtName(const string& extName);

		virtual ~ClassGraph();

		virtual string toString();

		virtual int addClass(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe);

		virtual int removeClass(LogicalID* lid);

		virtual int updateClass(ObjectPointer *optr, Transaction *&tr, QueryExecutor *qe);

		virtual int fetchSubInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames, bool noInvariantName = true);

		virtual int fetchExtInvariantNames(string& invariantName, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames,  bool noInvariantName = true);

		virtual int fetchExtInvariantNamesForLid(LogicalID* lid, Transaction *&tr, QueryExecutor *qe, stringHashSet& invariantsNames,  bool noObjectName = true);

		virtual int completeSubgraph(LogicalID* lid, Transaction *&tr, QueryExecutor *qe);

		virtual int invariantToCohesiveSubgraph(string& invariantName, Transaction *&tr, QueryExecutor *qe);

		virtual int lidToClassGraphLid(LogicalID* lidFrom, LogicalID*& lidTo);

		int classExists(string name, Transaction *tr, bool &exists);
		int classesLIDsFromNames(set<string>* names, Transaction *tr, vector<LogicalID*>& lids);
		int removePersistFromSubclasses(ObjectPointer *optr, Transaction *tr);		
		int removePersistFromSuperclasses(ObjectPointer *optr,Transaction *tr);

		virtual bool vertexExist(LogicalID* lid) {
			return classGraph.find(lid) != classGraph.end();
		}

		virtual int getVertex(LogicalID* lid, ClassGraphVertex*& cgv) {
			cgv = classGraph[lid];
			return 0;
		}

		virtual int getClassLidByName(const string& className, LogicalID*& classLid) {
			classLid = nameIndex[className];
			return 0;
		}

		virtual int getClassVertexByName(const string& className, ClassGraphVertex*& cgv, bool& fieldExist);

		virtual string getClassInvariantByName(const string& className, bool& exists) const;

		virtual int belongsToInvariant(SetOfLids* extClasses, string& invariantUpName, bool& inInvariant, bool isObject = true);

		// Metoda nie buduje grafu dla invariantUpName.
		virtual int belongsToInvariant(LogicalID* lid, string& invariantUpName, Transaction *&tr, QueryExecutor *qe, bool& inInvariant);

		virtual int findMethod(string name, unsigned int argsCount, SetOfLids* classesToSearch, string &code, vector<string> &params, int& founded, LogicalID* actualBindClassLid, LogicalID*& bindClassLid);

		virtual int staticFieldExist(const string& extName, bool& exist);

		virtual int isCastAllowed(LogicalID* classLid, SetOfLids* classesToCheck, bool& includes);

		virtual int isCastAllowed(LogicalID* classLid, ObjectPointer *optr, bool& includes);

		virtual int isCastAllowed(string& className, ObjectPointer *optr, bool& includes);

		virtual int checkExtendsForUpdate(const string& updatedClass, SetOfLids* extendedClasses, LogicalID*& upLid, bool& isUpdatePossible);
	};
}


#endif /*_CLASSGRAPH_H_*/
