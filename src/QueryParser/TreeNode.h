#ifndef _TREE_NODE_H_
#define _TREE_NODE_H_

#include <iostream>

#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <QueryParser/Stack.h>
#include <QueryParser/ClassNames.h>
#include <Store/Store.h>
#include <QueryParser/Privilige.h>
#include <QueryExecutor/QueryResult.h>
#include <Indexes/BTree.h>
#include <QueryExecutor/OuterSchema.h>
//#include <TypeCheck/DecisionTable.h>

//using namespace std;

namespace Indexes {
class Constraints;
};

using namespace Indexes;
namespace QParser {
	class QueryParser;
//for new treeNodes - make sure this is larger than the number of their inner operators/kinds. 
#define _TN_MAX_OP		30
	
	enum CreateType { CT_CREATE, CT_UPDATE, CT_CREATE_OR_UPDATE };

// statement (the starting symbol)
    class TreeNode 
    {
#ifdef QUERY_CACHE
    private:
	bool isCacheable;
#endif
    protected:
    static long uv;
    long oid;	// preserved by clone, copying constructor should copy it
    long id;	// unique object id, not preserved by clone and copying constructor
   
	TreeNode* parent;
	bool needed;		// is not death
	string card;		// cardinality
	bool coerceFlag;
	vector<int> coerceActions;
	Signature *coerceSig;
      string getPrefixForLevel( int level, string name ) {
        string result = "";

        if( level > 0 ) {
          for( int i = 0; i < level; i++ ) {
            result += " |";
          }
          result += name + "\n";
          for( int i = 0; i < level-1; i++ ) {
            result += " |";
          }
          result += " +-";
        }

        return result;
      }


    public:
	enum TreeNodeType { TNINT, TNSTRING, TNDOUBLE, TNVECTOR, TNNAME, 
	    TNAS, TNUNOP, TNALGOP, TNNONALGOP, TNTRANS, TNCREATE, TNCOND, TNLINK, TNPARAM, TNFIX, 
	    TNPROC, TNCALLPROC, TNRETURN, TNREGPROC, TNVIEW, TNREGVIEW, TNVALIDATION, TNCREATEUSER, 	
	    TNREMOVEUSER, TNPRIVLIST, TNNAMELIST, TNGRANTPRIV, TNREVOKEPRIV, TNREMOTE, TNINDEXDDL, TNINDEXDML,
	    TNINTERFACENODE, TNINTERFACESTRUCT, TNINTERFACEATTRIBUTES, TNINTERFACEATTRIBUTE, 
	    TNINTERFACEMETHODS, TNINTERFACEMETHOD, TNINTERFACEMETHODPARAMS, TNINTERFACEMETHODPARAM,
	    TNREGINTERFACE, TNREGCLASS, TNCLASS, TNDML, TNINCLUDES, TNEXCLUDES, TNCAST, TNINSTANCEOF, TNSYSTEMVIEWNAME,
	 	TNINTERFACEBIND, TNINTERFACEINNERLINKAGELIST, TNINTERFACEINNERLINKAGE, TNSIGNATURE, TNOBJDECL, TNSTRUCTTYPE,
		TNTYPEDEF, TNCOERCE, TNCASTTO, TNTHROWEXCEPTION, TNVIRTUALIZEAS, TNCRUD, TNREGSCHEMA, TNSCHEMANODE,
		TNSCHEMABINDS, TNSCHEMAAPS, TNSCHEMAEXPIMP};
	 
	TreeNode() : parent(NULL) { 
		this->needed = false;
		this->setId(TreeNode::getUv());
		this->setOid(TreeNode::getUv());
		#ifdef QUERY_CACHE
			this->isCacheable = false;
		#endif
		this->coerceFlag = false;
		coerceSig = NULL;
	}
	
	#ifdef QUERY_CACHE
		markCacheable() { this->isCacheable = true;}
	#endif
	
	long getOid(){return this->oid;};
	long getId(){return this->id;};
	void setOid(long _oid){this->oid = _oid;};
	void setId(long _id){this->id = _id;};
	static long getUv();
	
	TreeNode* getParent() { return parent; }
	void setParent(TreeNode* _parent) { parent = _parent; }
	virtual ~TreeNode() {};

	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual int putToString()=0;
	virtual void serialize(){this->putToString();};
	

    // For QExecutor:
	virtual string getName() {return (string) NULL;}
	virtual TreeNode* getArg() {return (TreeNode *) NULL;}
	virtual TreeNode* getLArg() {return (TreeNode *) NULL;}
	virtual TreeNode* getRArg() {return (TreeNode *) NULL;}
	virtual int getSectionNum(){ return -1;};  /* Nodes other than nonalgops don't hold this info. */
	virtual TreeNode* factorSubQuery (TreeNode *subT, string newName);
//	virtual TreeNode* optimizeByFactoring ();
	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
	/*should call an error. Subclasses that have at least 1 arg override this method.*/
	    return -1;}
	virtual string randomString();
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs) {return -1;}	
	virtual int optimizeTree () {Deb::ug( "type: %d.", type());	return 0;}
	/* should be overridden in subclasses...*/
	/* AND should have an argument for the data scheme ... ??? !!! */
	virtual string getCard(){return this->card;}
	virtual void setCard(string _card){this->card = _card;}
	virtual void evalCard(){this->setCard("1..1");}
	virtual string mulCard(string leftCard, string rightCard);
	virtual int minCard(string card);
	virtual int maxCard(string card);
	virtual string int2card(int card);
	virtual bool getNeeded(){return this->needed;}
	virtual void setNeeded(bool _needed) {this->needed = _needed;}
	virtual void markNeeded(){
		if (Deb::ugOn()) cerr << "jsi unimplemented markNeeded" << this->getName() << endl;
	};
	virtual void markNeeded2(){;};
	virtual void markNeededUp(){
		this->setNeeded(true);
		if (this->getParent() != NULL){
			this->getParent()->markNeededUp();	
		}	
	}
	virtual TreeNode * getDeath(){	
		return NULL;
	}
	
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		
	}
    virtual string toString( int level = 0, bool recursive = false, string name = "" ) { return ""; }
      
	// adds to boundVec all nodes from treeVec, that are bound in node
	static void getBoundIn(TreeNode *node, vector<TreeNode*> *treeVec, vector<TreeNode*> *boundVec);
	// 
	static void resetUsedNeeded(TreeNode *qtree); 
	
	static TreeNode* getNodeByOid(vector<TreeNode*>* listVec, long oid);
	
	virtual bool containsOid(long oid);
	
	//in case restrictive typechecking options are selected - some queries must be ommitted
	virtual bool skipPreprocessing() {
		int tp = type();
		return (tp == TNTRANS || tp == TNDML || tp == TNOBJDECL || tp == TNTYPEDEF || tp == TNVALIDATION);
	}
	virtual void qpLocalAction(QueryParser *qp);
	virtual int augmentTreeDeref(bool derefLeft, bool derefRight) {Deb::ug("TreeNode::augmtDeref(2arg)"); return 0;};
	virtual int augmentTreeDeref() {Deb::ug("TreeNode;:augmtDeref()"); return 0;};
	virtual int augmentTreeCoerce(int coerceType, bool augLeft, bool augRight) {Deb::ug("augmtCoerce(l,r) at TN");return 0;}
	virtual int augmentTreeCoerce(int coerceType) {Deb::ug("augmentCoerce(arg) at TN"); return 0;}
	virtual bool canDerefSon() {return true;}
	virtual bool canDerefNode() {return true;}
	virtual void canDerefSons(bool &canDerefL, bool &canDerefR){};
	virtual int markTreeCoerce(int actionId, Signature *coerceSig);
	virtual bool isCoerced() {return coerceFlag;}
	virtual int nrOfCoerceActions() {return coerceActions.size();}
	virtual int getCoerceAction(int pos) {return coerceActions.at(pos);}
	virtual Signature *getCoerceSig() {return coerceSig;}
	virtual void setCoerceSig(Signature *sig) {coerceSig = sig;}
	virtual bool needsCoerce(int actionId);
	virtual void copyAttrsOf(TreeNode *tn);
    };
	

// statement := query
    class QueryNode : public TreeNode 
    {
    public:
	virtual TreeNode* clone()=0;
	virtual int type()=0;
	virtual int putToString()=0;
	virtual string deParse()=0;
	
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		auxVec->push_back(this);
	}
    };
    
    class NameNode;
    
    class ValidationNode: public TreeNode {
	protected:
	    string login;
	    string passwd;
	public:

	    ValidationNode(string login, string passwd);

	    /* getters */
	    virtual string get_login();
	    virtual string get_passwd();
	    
	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Validation] login='" + login + "', passwd='" + passwd + "'\n";
      }
    };
    
    
    


    
   
    class SignatureNode;    
    class NameListNode;
    class InterfaceAttribute;
    
    typedef std::list<InterfaceAttribute> TAttributes;
    typedef std::list<string> TSupers;
    
    
    class PrintHandler
    {	//Abstract class
	public:
	    static string vectorString(const TAttributes &vec);
	    static string vectorString(const TSupers &vec);
		static string mapString(const Schemas::TNameToAccess &aps);
		static string mapString(const Schemas::TDict &binds);
	    int defaultPutToString() const {cout << simpleString(); return 0;}
	    virtual string simpleString() const = 0;
    };
    
    class PrintableTreeNode : public TreeNode, public PrintHandler
    {	//Abstract class
	public:
	    virtual int putToString() {return defaultPutToString();}
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {return getPrefixForLevel(level, name) + simpleString() + "\n";}
    };
    
    class PrintableQueryNode : public QueryNode, public PrintHandler
    {	//Abstract class
	public:
	    virtual int putToString() {return defaultPutToString();}
	    virtual string toString(int level = 0, bool recursive = false, string name = "") {return getPrefixForLevel(level, name) + simpleString() + "\n";}
	    virtual string deParse() {return simpleString();}
    };

	/* Schema Nodes */
	class SchemaExpImpNode : public PrintableTreeNode
	{
		private:
			bool m_isExport;
			string m_schemaName;
			
		public:
			SchemaExpImpNode(string schemaName, bool isExport) : m_schemaName(schemaName), m_isExport(isExport) {}
			string getSchemaName() const {return m_schemaName;}
			bool getIsExport() const {return m_isExport;}
			
			int type() {return TNSCHEMAEXPIMP;}
			TreeNode* clone() {return new SchemaExpImpNode(*this);}
			string simpleString() const {string res = m_isExport ? "export" : "import"; res += " " + m_schemaName; return res;}	
	};
	
	class Crud : public PrintableTreeNode
	{
		private:
			int m_crud;
			bool m_default;

		public:
			Crud (int crud = Schemas::OuterSchema::defaultCrud) {m_crud = crud; m_default = (crud == Schemas::OuterSchema::defaultCrud);}		
			void addAccessType(int priv);
			int getCrud() const {return m_crud;}

			int type() {return TNCRUD;}
			TreeNode *clone();
			string simpleString() const;
	};

	class SchemaAPs : public PrintableTreeNode
	{
		private:
			Schemas::TNameToAccess m_accessPoints;

		public:
			SchemaAPs() {}	
			Schemas::TNameToAccess getAccessPoints() const {return m_accessPoints;}
			void addAccessPoint(string name, int crud) {m_accessPoints[name] = crud;}	

			int type() {return TNSCHEMAAPS;}
			TreeNode *clone();
			string simpleString() const;
	};

	class SchemaBinds : public PrintableTreeNode
	{
		private:
			Schemas::TDict m_binds;

		public:
			SchemaBinds() {}
			void addBind(string name, string boundName) {m_binds[name] = (boundName);}
			Schemas::TDict getBinds() const {return m_binds;}

			TreeNode *clone();	    
			int type() {return TNSCHEMABINDS;}
			string simpleString() const;
	};

	class SchemaNode :public PrintableQueryNode
	{
		private:
			string m_name;
			Schemas::TNameToAccess m_accessPoints;

		public:
			SchemaNode() {}
			SchemaNode(string name, Schemas::TNameToAccess aps);
			string getName() const {return m_name;}
			Schemas::TNameToAccess getAccessPoints() const {return m_accessPoints;}
			
			void setName(string name) {m_name = name;}
			void setAccessPoints(Schemas::TNameToAccess aps) {m_accessPoints = aps;}

			TreeNode *clone();	    
			int type() {return TNSCHEMANODE;}
			string simpleString() const;
	};

	class RegisterSchemaNode : public PrintableQueryNode
	{
		protected: 
			SchemaNode m_schemaQuery;
		
		public:
			RegisterSchemaNode(SchemaNode *query) {m_schemaQuery = *query;}		
			SchemaNode getSchemaQuery() const {return m_schemaQuery;}
			
			TreeNode *clone();	    
			int type() {return TNREGSCHEMA;}
			string simpleString() const;
	};
    
	/* Interface Nodes */
    class InterfaceAttribute: public PrintableTreeNode
    {
	private:
	    string m_name;
	public:
	    InterfaceAttribute() {}
	    InterfaceAttribute(string name);
	    string getName() const {return m_name;}
	    
	    TreeNode* clone();
	    int type() {return TNINTERFACEATTRIBUTE;}
	    string simpleString() const {string res = "InterfaceAttribute: " + m_name; return res;}
    };
    
    class InterfaceFields: public PrintableTreeNode
    {	//Abstract class
	protected:
	    InterfaceFields() {}
	    TAttributes m_fields;
	public:
	    TAttributes getUniqueFields() const;
	    void prependField(InterfaceAttribute *field) {m_fields.push_front(*field);}
	    
	    virtual TreeNode* clone() = 0;
	    virtual int type() = 0;
	    string simpleString() const;
    };

    class InterfaceAttributes: public InterfaceFields 
    {
	public:
	    InterfaceAttributes(InterfaceAttribute *attribute) {prependField(attribute);}
	    virtual int type() {return TNINTERFACEATTRIBUTES;}
	    TreeNode* clone();
    };

    class InterfaceMethodParams: public InterfaceFields 
    {
	public:
	    InterfaceMethodParams(InterfaceAttribute *methodParam) {prependField(methodParam);}
    	virtual int type() {return TNINTERFACEMETHODPARAMS;}
	    TreeNode* clone();
	};

    class InterfaceMethod: public PrintableTreeNode
    {
	private:
	    string m_name;
	    TAttributes m_params;
	public:
	    InterfaceMethod() {};
	    InterfaceMethod(string name);
	    string getName() const {return m_name;}
	    TAttributes getParams() const {return m_params;}
	    void addParams(TAttributes params) {m_params = params;}
	    
	    TreeNode* clone();
	    int type() {return TNINTERFACEMETHOD;};
	    string simpleString() const;
    };
    typedef std::list<InterfaceMethod> TMethods;
    
    class InterfaceMethods: public PrintableTreeNode
    {
	private:
	    TMethods m_methods;
	public:
	    InterfaceMethods(InterfaceMethod *method) {prependMethod(method);}
	    TMethods getUniqueMethods() const;
	    void prependMethod(InterfaceMethod *method) {m_methods.push_front(*method);}
	    
	    TreeNode* clone();
	    int type() {return TNINTERFACEMETHODS;}
	    string simpleString() const;
    };
        
    class InterfaceNode : public PrintableQueryNode
    {
	private:
	    string m_interfaceName;
	    string m_objectName;
	    TAttributes m_attributes;
	    TMethods m_methods;
	    TSupers m_supers; 
	public:
	    InterfaceNode() {};
	    string getInterfaceName() const {return m_interfaceName;}
	    string getObjectName() const {return m_objectName;}
	    TAttributes getAttributes() const {return m_attributes;}
	    TMethods getMethods() const {return m_methods;}
	    TSupers getSupers() const {return m_supers;}
	
	    void setInterfaceName(string interfaceName) {m_interfaceName = interfaceName;}
	    void setObjectName(string objectName) {m_objectName = objectName;}
	    void setAttributes(TAttributes attributes) {m_attributes = attributes;}
	    void setMethods(TMethods methods) {m_methods = methods;}
	    void setSupers(NameListNode *supers);
	
	    TreeNode* clone();
	    int type() {return TNINTERFACENODE;}
	    string simpleString() const;	    
    };
    
    class RegisterInterfaceNode : public PrintableQueryNode
    {
		protected:
			QueryNode* m_query;
		public:
			RegisterInterfaceNode(QueryNode *query) {m_query = query;}
			~RegisterInterfaceNode() {if (m_query) delete m_query; m_query=NULL;} 
			QueryNode *getQuery() {return m_query;}
			
			TreeNode *clone();
			int type() {return TNREGINTERFACE;}
			string simpleString() const;	    

    };

    class InterfaceBind: public PrintableQueryNode 
    {
        private:
    	    string m_interfaceName;
	    string m_implementationName;
	public:
	    InterfaceBind(string interfaceName, string implementationName);
	    string getInterfaceName() const {return m_interfaceName;}
	    string getImplementationName() const {return m_implementationName;}

	    TreeNode *clone();	    
	    int type() {return TNINTERFACEBIND;}
	    string simpleString() const {string res = "InterfaceBind: " + m_interfaceName + " -> " + m_implementationName; return res;}
    }; 


    
    /* Privilige Nodes */
    class PriviligeListNode: public TreeNode {
	private:
	    Privilige *priv;
	    PriviligeListNode *priv_list;
	public:
	    PriviligeListNode(Privilige *priv, PriviligeListNode *priv_list = NULL);
	    virtual ~PriviligeListNode();
	    
	    /* getters */
	    virtual Privilige* get_priv();
	    virtual PriviligeListNode* try_get_priv_list();

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string privVal = priv ? priv->get_priv_name() : "<null>";
        string result = getPrefixForLevel( level, name ) + "[PriviligeList] priv=" + privVal + "\n";

        if( recursive ) {
          if( priv_list )
            result += priv_list->toString( level+1, true, "priv_list" );
        }

        return result;
      }
    };
    
    class NameListNode: public TreeNode {
	private:
	    string name;
	    NameListNode *name_list;
	public:
	    NameListNode(string name, NameListNode *list = NULL);
	    virtual ~NameListNode();
	    
	    /* getters */
	    virtual string get_name();
	    virtual NameListNode* try_get_name_list();
	    
	    virtual int namesFromUniqueList( set<string>*& names );

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string result = getPrefixForLevel( level, n ) + "[NameList] name='" + name + "'\n";

        if( recursive ) {
          if( name_list )
            result += name_list->toString( level+1, true, "name_list" );
        }

        return result;
      }
    };
    
    class GrantPrivNode: public TreeNode {
	private:
	    PriviligeListNode 	*priv_list;
	    NameListNode	*name_list;
	    string 		user;
	    bool		with_grant_option;
	
	public:
	    GrantPrivNode(PriviligeListNode *priv_list, NameListNode *name_list, string user,bool with_grant_option);
	    virtual ~GrantPrivNode();
	    
	    /* getters */
	    virtual PriviligeListNode*  get_priv_list();
	    virtual NameListNode*	get_name_list();
	    virtual string		get_user();
	    virtual bool		get_with_grant_option();

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[GrantPriv] user='" + user +
          "', with_grant_option=" + (with_grant_option ? "true" : "false" ) + "\n";

        if( recursive ) {
          if( priv_list )
            result += priv_list->toString( level+1, true, "priv_list" );
          if( name_list )
            result += name_list->toString( level+1, true, "name_list" );
        }

        return result;
      }
    };
    
    class RevokePrivNode: public TreeNode {
	private:
	    PriviligeListNode 	*priv_list;
	    NameListNode	*name_list;	
	    string 		user;
	    
	public:
	    RevokePrivNode(PriviligeListNode *priv_list, NameListNode *name_list, string user );
	    virtual ~RevokePrivNode();
	    
	    /* getters */
	    virtual PriviligeListNode* 	get_priv_list();
	    virtual NameListNode*	get_name_list();
	    virtual string		get_user();	    

	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RevokePriv] user='" + user + "'\n";

        if( recursive ) {
          if( priv_list )
            result += priv_list->toString( level+1, true, "priv_list" );
          if( name_list )
            result += name_list->toString( level+1, true, "name_list" );
        }

        return result;
      }
    };

    class CreateUserNode: public TreeNode {
	private:
	    string user;
	    string passwd;
	
	public:
	    CreateUserNode(string user, string passwd);
	    
	    /* getters */
	    virtual string get_user();
	    virtual string get_passwd();
	    
	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();	    

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[CreateUser] user='" + user + "', passwd='" + passwd + "'\n";
      }
    };
    
    class RemoveUserNode: public TreeNode {
	private:
	    string user;
	
	public:
	    RemoveUserNode(string user);
	    
	    /* getters */
	    virtual string get_user();
	    
	    /* inherited functions */
	    virtual TreeNode* clone();
	    virtual int type();
	    virtual int putToString();	    
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[RemoveUser] user='" + user + "'\n";
      }
    };

// nodes that have two sons (one may be a NULL) - both alg. and non-alg.
    class TwoArgsNode : public QueryNode
    {

    protected:
	QueryNode* larg;
	QueryNode* rarg;
    public:
	virtual int putToString()=0;
	virtual QueryNode* getLArg() { return larg; }
	virtual QueryNode* getRArg() { return rarg; }

	virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
	virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}

	virtual void markNeeded(){
			this->setNeeded(true);
			this->getLArg()->markNeeded();
			this->getRArg()->markNeeded();
	}
	virtual void markNeeded2(){
			this->setNeeded(true);
			this->getLArg()->markNeeded2();
			this->getRArg()->markNeeded2();
	}
	
	virtual TreeNode * getDeath(){
		TreeNode *pom = this->getLArg()->getDeath();
		if (pom != NULL) 
			return pom;
		return this->getRArg()->getDeath();
	}

	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
    	    if (this->getLArg() == oldSon) {
    		this->setLArg ((QueryNode *) newSon);	return 0;} 
	    else if (this->getRArg() == oldSon) {
    		this->setRArg ((QueryNode *) newSon); return 0;} 
	    else { /*an error should be reported - oldSon is not my son!*/ 
    		return -1;}        
    }	
    virtual void getInfixList(vector<TreeNode*> *auxVec){
		this->getLArg()->getInfixList(auxVec);
		auxVec->push_back(this);
		this->getRArg()->getInfixList(auxVec);
	}
	virtual int augmentTreeDeref(bool derefLeft, bool derefRight);
	virtual int augmentTreeCoerce(int coerceType, bool augLeft, bool augRight);
	virtual void canDerefSons(bool &canDerefL, bool &canDerefR);
    };
    
// atomic nodes with string/int/double/bool value
    class ValueNode : public QueryNode 
    {

	public:
	virtual int putToString() {
	    cout << "(_value_)";    
	    return 0;
	}	
	virtual void serialize(){cout << "'value'";};
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Value]\n";
      }
      virtual void markNeeded(){
      		this->setNeeded(true);
      }
      virtual void markNeeded2(){
      		this->setNeeded(true);
      }
    };    

// query := name
    class NameNode : public QueryNode 
    {
    protected:
	string name;
	int bindSect;
	int stackSize;
	TreeNode *dependsOn;		// for removing death subqueries
			// can be either other name node or group as node
			// node in which static nested result this NameNode is binded
			// in dependsOn is NULL means that this NameNode is binded in base sections
	vector<TreeNode*> *boundIn;	// wezly drzewa w ktorych jest wiazane np(EMP group as 'aaa', DEPT group as 'aaa').aaa - aaa jest wiazane 2 razy
	vector<TreeNode*> *usedBy;	// wezly drzewa ktore sa w nim wiazane
	int statEnvSecSize;			// rozmiar sekcji w ktorej jest wiazany, co jezeli 1, a elementem jest sigcoll?
								// moze trzeba dorobic metode size w sygnaturze zwracajaca 1, przedef w sigcoll zwracajaca sume po size dla wszystkich elem na jej liscie
    public:
	NameNode(string _name) : name(_name) {bindSect = 0; stackSize = 0; dependsOn = NULL; this->usedBy = new vector<TreeNode*>; this->boundIn = new vector<TreeNode*>;}
	virtual TreeNode* getDependsOn(){return this->dependsOn;}
	virtual void setDependsOn(TreeNode *_dependsOn){this->dependsOn = _dependsOn;}
	virtual int getBindSect() { return this->bindSect;}
	virtual int getStackSize() {return this->stackSize;}
	virtual void setBindSect(int newBSect) { this->bindSect = newBSect;}
	virtual void setStackSize(int newSize) { this->stackSize = newSize;}
	virtual vector<TreeNode*> *getBoundIn(){return this->boundIn;}
	virtual void setBoundIn(vector<TreeNode*>* _boundIn) {this->boundIn = _boundIn;}	
	virtual vector<TreeNode*> *getUsedBy(){return this->usedBy;}
	virtual void setUsedBy(vector<TreeNode*>* _usedBy) {this->usedBy = _usedBy;}	
	void calculateBoundUsed(vector<BinderWrap*> *vec);	// wstawia do this->boundIn i dodaje sie do wektorow usedBy w ktorych jest wiazany
														// nie zmienia wlasnego usedBy
	virtual void setStatEnvSecSize(int size){statEnvSecSize = size;};
	virtual int  getStatEnvSecSize(){return statEnvSecSize;};
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNNAME; }
	virtual string getName() { return name; }
	virtual int putToString() {
	    cout << "("<< this->getName() <<"["<<bindSect << "," << stackSize << "]";   
	    cout << this->getCard()<<"depOn:" ;
	    if (this->dependsOn != NULL)
	    	cout << (this->dependsOn->getName()); 
	    else cout <<"NULL";
	    cout << ":" << (this->getNeeded()?"needed":"notNeeded");
	    cout << ")";
	    return 0;
	}
	virtual void serialize(){cout << this->getName();};
	virtual void markNeeded(){
		this->setNeeded(true);
		if (this->getDependsOn() != NULL){
			this->getDependsOn()->markNeededUp();
		}
	}
	virtual void markNeeded2(){
		this->setNeeded(true);
		
		for (vector<TreeNode*>::iterator iter = this->getBoundIn()->begin(); iter != this->getBoundIn()->end(); iter++){
				if ((*iter != NULL)){		// zalezy od null - czyli jest wiazany w sekcji bazowej, 
					(*iter)->markNeededUp();
				}	
			
		}
	}
	virtual void markNeededUp(){
		this->setNeeded(true);
		if (this->getParent() != NULL){
			this->getParent()->markNeededUp();	
		}	
		if (this->getDependsOn() != NULL){
			this->getDependsOn()->markNeededUp();
		}
	}
	
//	virtual void evalCard(){;}	// w nameNode biore to z bindera - przy wiazaniu
	
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	virtual int staticEval2 (StatQResStack *&qres, StatEnvStack *&envs);
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string bindSectS, stackSizeS;
        c << bindSect; c >> bindSectS;
        c << stackSize; c >> stackSizeS;
        return getPrefixForLevel( level, n ) + "[Name] name='" + name + "', bindSect=" + bindSectS  + ", stackSize=" + stackSizeS + "\n";
      }
	
	virtual ~NameNode() {}
	
	virtual string deParse() { string result; result = " " + name + " "; return result; };
    };  


// query := name
    class ParamNode : public QueryNode 
    {
    protected:
	string name;
	int bindSect;
	int stackSize;
    public:
	ParamNode(string _name) : name(_name) {bindSect = 0; stackSize = 0;}

	virtual int getBindSect() { return this->bindSect;}
	virtual int getStackSize() {return this->stackSize;}
	virtual void setBindSect(int newBSect) { this->bindSect = newBSect;}
	virtual void setStackSize(int newSize) { this->stackSize = newSize;}
	
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNPARAM; }
	virtual string getName() { return name; }
	virtual int putToString() {
	    cout << "("<< this->getName() <<"["<<bindSect << "," << stackSize << "])";    
	    return 0;
	}
	virtual void serialize(){cout << this->getName();};
	//virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	
	virtual ~ParamNode() {}
	
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        stringstream c;
        string bindSectS, stackSizeS;
        c << bindSect; c >> bindSectS;
        c << stackSize; c >> stackSizeS;
        return getPrefixForLevel( level, n ) + "[Param] name='" + name + "', bindSect=" + bindSectS  + ", stackSize=" + stackSizeS + "\n";
      }
      
      virtual string deParse() { string result; result = " " + name + " "; return result; };
      
    };  



// query := integer
    class IntNode : public ValueNode 
    {
    protected:
	int value;
    public:
	IntNode(int _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNINT; }
	int getValue() {return value;}
  	virtual ~IntNode() {}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string valueS;
        c << value; c >> valueS;
        return getPrefixForLevel( level, name ) + "[Int] value=" + valueS  + "\n";
      }
    virtual void serialize(){cout << this->getValue();};  
      virtual string deParse() { 
      	stringstream ss;
	string str;
	ss << value;
	ss >> str; 
	string result; 
	result = " " + str + " "; 
	return result; };
    };  

// query := string
    class StringNode : public ValueNode 
    {
    protected:
	string value;
    public:
	StringNode(string _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNSTRING; }
	string getValue() { return value; }
  	virtual ~StringNode() {}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[String] value=" + value  + "\n";
      }
      virtual void serialize(){cout << "\"" << this->getValue() <<"\"";};  
      virtual string deParse() { string result; result = " \"" + value + "\" "; return result; };
    };  

// query := double
    class DoubleNode : public ValueNode 
    {
    protected:
	double value;
    public:
	DoubleNode(double _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNDOUBLE; }
	double getValue() { return value; }        

	virtual ~DoubleNode() {}
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string valueS;
        c << value; c >> valueS;
        return getPrefixForLevel( level, name ) + "[Double] value=" + valueS  + "\n";
      }
      virtual void serialize(){cout << this->getValue();};  
      virtual string deParse() { 
      	stringstream ss;
	string str;
	ss << value;
	ss >> str; 
	string result; 
	result = " " + str + " "; 
	return result; };
    };  

// TODO: query := bool??? czy tylko powstaje w wyniku jakichs rzeczy .. ? 
// CZY TO W OGOLE POTRZEBNE GDZIEKOLWIEK ? ! 
    class BoolNode : public QueryNode 
    {
    protected:
	bool value;
    public:
	BoolNode(bool _value) : value(_value) {}
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNDOUBLE; }
	bool getValue() { return value; }        
	virtual int putToString() {
	    cout << "("<< this->getValue() <<")";    
	    return 0;
	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
	virtual ~BoolNode() {}
	virtual void serialize(){cout << this->getValue();};  
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Bool] value=" + (value ? "true" : "false")  + "\n";
      }
    };      
    
// query := query "as" name | query "group as" name
    class NameAsNode : public QueryNode 
    {
    protected:
	QueryNode* arg;
	string name;
	bool group;
	vector<TreeNode*> *usedBy;	// wezly drzewa ktore sa w nim wiazane

    public:
	NameAsNode(QueryNode* _arg, string _name, bool _group)
            : arg(_arg), name(_name), group(_group) { arg->setParent(this); this->usedBy = new vector<TreeNode*>; }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNAS; }
	string getName() { return name; }
	TreeNode* getArg() { return arg; }
	virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
	virtual vector<TreeNode*> *getUsedBy(){return this->usedBy;}
	virtual void setUsedBy(vector<TreeNode*>* _usedBy) {usedBy = _usedBy;}	
	bool isGrouped() { return group; }
	virtual int putToString() {
	    cout << "(";
	    if (arg!= NULL) arg->putToString();
	    else cout << "___";
	    if (this->isGrouped()) cout <<" group as ";
	    else cout << " as ";
	    
	    cout << this->getName() <<this->getCard()<<")";    
	    return 0;
	}
	virtual void serialize(){
	 	cout << "(";
	    if (arg!= NULL) arg->serialize();
	    else cout << "___";
	    if (this->isGrouped()) cout <<" group as '";
	    else cout << " as '";
	    
	    cout << this->getName() <<"')";    
	
	};  
	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
	    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
	    else {/*an error from errorConsole is called;*/ return -1;}
	}    	
	virtual void markNeeded(){
			this->setNeeded(true);
			this->getArg()->markNeeded();	
	}
	virtual void markNeeded2(){
			this->setNeeded(true);
			this->getArg()->markNeeded2();	
	}
	virtual void evalCard(){this->setCard(this->getArg()->getCard());}
	virtual TreeNode * getDeath(){
		return this->getArg()->getDeath();
	}
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		auxVec->push_back(this);
		this->getArg()->getInfixList(auxVec);
	}
	virtual int augmentTreeDeref();
	virtual int augmentTreeCoerce(int coerceType);
	virtual bool canDerefSon() {return this->getArg()->canDerefNode();}
	virtual int staticEval(StatQResStack *&qres, StatEnvStack *&envs);
	virtual int optimizeTree() {return arg->optimizeTree();}
	virtual ~NameAsNode() { if (arg != NULL) delete arg; }
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string result = getPrefixForLevel( level, n ) + "[NameAs] name='" + name + "', group=" + (group ? "true" : "false") + "\n";

        if( recursive ) {
          if( arg )
            result += arg->toString( level+1, true, "arg" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result, tmp_res; 
	if (group) { tmp_res = "group as "; } 
	else  { tmp_res = "as "; }
	result = " (" + arg->deParse() + tmp_res + name + ") "; 
	return result; }
    };  

// query := query InfixAlgOp query
    class UnOpNode : public QueryNode 
    {
    public:
	enum unOp { unMinus, count, sum, avg, min, max, distinct, boolNot, deleteOp, deref, ref, nameof};
    protected:
	QueryNode* arg;
	unOp op;
    public:
	UnOpNode(QueryNode* _arg, unOp _op) : arg(_arg), op(_op)
                                        { arg->setParent(this); }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNUNOP; }
	TreeNode* getArg() { return arg; }
	unOp getOp() { return op; }
	virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }
	virtual void setOp(unOp _op) { op = _op; }

	virtual void markNeeded(){
			this->setNeeded(true);
			this->getArg()->markNeeded();	
	}
	virtual void markNeeded2(){
			this->setNeeded(true);
			this->getArg()->markNeeded2();	
	}
	virtual TreeNode * getDeath(){
		return this->getArg()->getDeath();
	}
	virtual void getInfixList(vector<TreeNode*> *auxVec){
		auxVec->push_back(this);
		this->getArg()->getInfixList(auxVec);
	}

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[UnOp] op=" + opStr() + "\n";

        if( recursive ) {
          if( arg )
            result += arg->toString( level+1, true, "arg" );
        }

        return result;
      }
	virtual int putToString() {
	    cout << "(";
	    cout << this->opStr() << " ";
	    if (arg!= NULL) arg->putToString();
	    else cout << "___";
	    
	    cout << ")";    
	    return 0;
	}
	virtual void serialize(){
		cout << "(";
	    cout << this->opStr() << " ";
	    if (arg!= NULL) arg->serialize();
	    else cout << "___";
	    
	    cout << ")"; 
	}
	virtual string opStr() {
	    int op = this->getOp();
	    switch (op) {
	    	case 0:	return "unMinus";
	    	case 1: return "count";
	    	case 2: return "sum";
	    	case 3: return "avg";
	    	case 4: return "min";
	    	case 5: return "max";
	    	case 6: return "distinct";
	    	case 7: return "boolNot";
	    	case 8: return "deleteOp";
	    	case 9: return "deref";
	    	case 10: return "ref";
	    	case 11: return "nameof";
	    	default: return "~~~";
	    }
	    	    
	}

	virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
		newSon->setParent(this);
	    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
	    else {/*an error from errorConsole is called;*/ return -1;}
	}    	
	
	virtual int augmentTreeDeref();
	virtual int augmentTreeCoerce(int coerceType);
	//'ref' flag forbids derefing...
	virtual bool canDerefNode() {return (op != UnOpNode::ref);}
	virtual bool canDerefSon() {return this->getArg()->canDerefNode();}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
	virtual int optimizeTree() {return arg->optimizeTree();}
	virtual ~UnOpNode() { if (arg != NULL) delete arg; }
	
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case UnOpNode::count:
				result = " count(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::sum:
				result = " sum(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::avg:
				result = " avg(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::min:
				result = " min(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::max:
				result = " max(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::deref:
				result = " deref(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::ref: 
				result = " ref(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::distinct:
				result = " distinct(" + arg->deParse() + ") ";
				return result;
			case UnOpNode::unMinus:
				result = " -(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::boolNot:
				result = " not(" + arg->deParse() + ") ";
				return result;
			case UnOpNode::deleteOp:
				result = " delete(" + arg->deParse() + ") "; 
				return result;
			case UnOpNode::nameof:
				result = " name(" + arg->deParse() + ") ";
				return result;
		}
		return result;
	};
    };  

  class VectorNode : public QueryNode
    {
    protected:	
	vector<QueryNode*> queries;
    public:
	VectorNode (QueryNode* singleQuery, VectorNode *tail) {
	    queries = tail->getQueries();
	    queries.push_back(singleQuery);
	    //delete tail;
	}
	VectorNode (QueryNode* singleQuery) {
	    queries.push_back(singleQuery);
	}
	VectorNode () {};

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNVECTOR;};
	virtual vector<QueryNode*> getQueries() {return this->queries;}
	virtual QueryNode* getQuery(int i) {return this->queries.at(i);}
	virtual void setQueries(vector<QueryNode*> q) {queries = q;}
	virtual unsigned int howManyParts() { return queries.size(); }
	
	virtual ~VectorNode() {
		for (unsigned int i=0; i < queries.size(); i++) {
			if (queries[i] != NULL) delete queries[i]; }
		queries.clear();
	}
	virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        	string result = "";
        	return result;
      	}
	
	virtual int putToString() { return 0; }	
	virtual string deParse() {
		string result = "";
        	return result;
	}
    };

// query := query InfixAlgOp query
    class AlgOpNode : public TwoArgsNode 
    {
    public:
    /*
     * Cos tu jest nie tak z tym insert i insertInto!
     * Zdaje sie, ze insertInto nie jest uzywane i nie ma slowa kluczowego insert.
     * Zeby dobrze dzialal operator :< w procedurach, chwilowo insert
     * i insertInto beda robic w opStr i deParse to samo.
     */
	enum algOp { bagUnion, bagIntersect, bagMinus, plus, minus, times, divide,
   	         eq, refeq, neq, lt, gt, le, ge, boolAnd, boolOr, comma, insert, insert_viewproc, insertInto, assign, assign_viewproc, semicolon }; 
   	static QueryNode* newNode(VectorNode* vec, algOp _op) {
        	vector<QueryNode*> q = vec->getQueries();
        	unsigned int q_size = q.size();
        	if (q_size == 0) return NULL;
        	QueryNode* last_node = q[q_size - 1];
        	for (int i = (q_size - 2); i >= 0; i--) {
        		QueryNode* tmp_node = new AlgOpNode(q[i], last_node, _op);
        		last_node = tmp_node;
        	}
        	return last_node;
        }
    protected:
	QueryNode* larg;
	QueryNode* rarg;
	algOp op;
    public:
	AlgOpNode(QueryNode* _larg, QueryNode* _rarg, algOp _op)
		: larg(_larg), rarg(_rarg), op(_op) 
                { larg->setParent(this); rarg->setParent(this); }
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNALGOP; }
	virtual QueryNode* getLArg() { return larg; }
	virtual QueryNode* getRArg() { return rarg; }
	algOp getOp() { return op; }
	virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
	virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}
	virtual void setOp(algOp _op) { op = _op; }

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[AlgOp] op=" + opStr() + "\n";

        if( recursive ) {
          if( larg )
            result += larg->toString( level+1, true, "larg" );
          if( rarg )
            result += rarg->toString( level+1, true, "rarg" );
        }

        return result;
      }
	virtual int putToString() {
	    cout << "(";
	    if (larg!= NULL) larg->putToString();
	    else cout << "___";
	    cout << this->opStr()<<this->getCard()<<(this->getNeeded()?"Need":"Del");
	    if (rarg!= NULL) rarg->putToString();
	    else cout << "___";
	    
	    cout << ")";    
	    return 0;
	}
	virtual void serialize(){
		cout << "(";
	    if (larg!= NULL) larg->serialize();
	    else cout << "___";
	    cout << this->opStr() << " ";
	    if (rarg!= NULL) rarg->serialize();
	    else cout << "___";
	    
	    cout << ")"; 
	};  
	virtual string opStr() {
	    int op = this->getOp();
	    if (op == 0) return "union";
	    if (op == 1) return "intersect ";
	    if (op == 2) return "minus";
	    if (op == 3) return " + ";  //"plus";
	    if (op == 4) return " - ";	//"minus";
	    if (op == 5) return " * ";	//times";
	    if (op == 6) return " / ";	//divide";
	    if (op == 7) return " = ";	    
	    if (op == 8) return " == ";
	    if (op == 9) return " != ";	    
	    if (op == 10) return " < ";	    
	    if (op == 11) return " > ";	    
	    if (op == 12) return " <= ";
	    if (op == 13) return " >= ";
	    if (op == 14) return " && ";
	    if (op == 15) return " || ";
	    if (op == 16) return " , ";	    
	    if (op == 17) return " :< ";
	    if (op == 18) return " :< ";
	    if (op == 19) return " :< ";
	    if (op == 20) return " := ";
	    if (op == 21) return " := ";
	    if (op == 22) return " ; ";
	    return "~~~";
	}
	
	virtual TreeNode * getDeath(){	
		if (this->getOp() == AlgOpNode::comma){
			if((!this->getRArg()->getNeeded()&&(this->getRArg()->getCard()=="1..1"))){
				return this->getRArg();
			}
			if((!this->getLArg()->getNeeded()&&(this->getLArg()->getCard()=="1..1"))){
				return this->getLArg();
			}
		}
		return TwoArgsNode::getDeath();
	}
	virtual void evalCard(){
		if (this->opStr() != " , ")	this->setCard("1..1");
		else {
			this->setCard(this->mulCard(this->getLArg()->getCard(), this->getRArg()->getCard()));	
		};
	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	virtual int optimizeTree();
	virtual ~AlgOpNode() { if (larg != NULL) delete larg;
                         if (rarg != NULL) delete rarg; }
			 
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case AlgOpNode::bagUnion: { 
				result = " (" + larg->deParse() + "union" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::bagIntersect: { 
				result = " (" + larg->deParse() + "intersect" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::bagMinus: { 
				result = " (" + larg->deParse() + "minus" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::plus: { 
				result = " (" + larg->deParse() + "+" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::minus: { 
				result = " (" + larg->deParse() + "-" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::times: { 
				result = " (" + larg->deParse() + "*" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::divide: { 
				result = " (" + larg->deParse() + "/" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::eq: { 
				result = " (" + larg->deParse() + "=" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::neq: { 
				result = " (" + larg->deParse() + "!=" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::lt: { 
				result = " (" + larg->deParse() + "<" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::gt: { 
				result = " (" + larg->deParse() + ">" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::le: { 
				result = " (" + larg->deParse() + "<=" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::ge: { 
				result = " (" + larg->deParse() + ">=" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::boolAnd: { 
				result = " (" + larg->deParse() + "and" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::boolOr: { 
				result = " (" + larg->deParse() + "or" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::comma: { 
				result = " (" + larg->deParse() + "," + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::insertInto: { 
				result = " (" + larg->deParse() + ":<" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::assign: { 
				result = " (" + larg->deParse() + ":=" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::assign_viewproc: { 
				result = " (" + larg->deParse() + ":=" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::semicolon: { 
				result = " (" + larg->deParse() + ";" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::insert: { 
				result = " (" + larg->deParse() + ":<" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::insert_viewproc: { 
				result = " (" + larg->deParse() + ":<" + rarg->deParse() + ") "; 
				return result; 
			}
			case AlgOpNode::refeq: { 
				result = " (" + larg->deParse() + "==" + rarg->deParse() + ") "; 
				return result; 
			}
		}
		return result;
	};
    };  

// query := query InfixNonalgOp query
    class NonAlgOpNode : public TwoArgsNode 
    {
    public:
	enum nonAlgOp { dot, join, where, closeBy, closeUniqueBy, leavesBy,
                  leavesUniqueBy, orderBy, exists, forAll, forEach };
    protected:
	QueryNode* larg;
	QueryNode* rarg;
	nonAlgOp op;
	int firstOpenSect;   /* nrs of the ENV section which was open by this operator */	

	int lastOpenSect;	
    public:
	NonAlgOpNode(QueryNode* _larg, QueryNode* _rarg, nonAlgOp _op)
                : larg(_larg), rarg(_rarg), op(_op)
	{ 
		if (larg != NULL) {
			larg->setParent(this); 
		}
		if (rarg != NULL) {
			rarg->setParent(this); 
		}
		firstOpenSect = 0; lastOpenSect = 0; 
	} 
	virtual TreeNode* clone();
	virtual int type() { return TreeNode::TNNONALGOP; }
	QueryNode* getLArg() { return larg; }
	QueryNode* getRArg() { return rarg; }
	nonAlgOp getOp() { return op; }
	virtual void setLArg(QueryNode* _larg) {larg = _larg;larg->setParent(this);}
	virtual void setRArg(QueryNode* _rarg) {rarg = _rarg;rarg->setParent(this);}
	virtual void setOp(nonAlgOp _op) { op = _op; }
	
	virtual void markNeeded(){
		this->setNeeded(true);
		this->putToString();
		this->getRArg()->markNeeded();	
		if (op != 0){		// not a dot
			this->getLArg()->markNeeded();	
		}
	}
	virtual void markNeeded2(){
		this->setNeeded(true);
		this->putToString();
		this->getRArg()->markNeeded2();	
		if (op != 0){		// not a dot
			this->getLArg()->markNeeded2();	
		}
	}
	
	virtual TreeNode * getDeath(){	
		if (this->getOp() == NonAlgOpNode::join){
			if((!this->getRArg()->getNeeded())&&(this->getRArg()->getCard()=="1..1")){
				return this->getRArg();
			}
		}
		return TwoArgsNode::getDeath();
	}
	
	virtual void evalCard(){
		if (this->op == NonAlgOpNode::dot){
			this->setCard(this->mulCard(this->getLArg()->getCard(), this->getRArg()->getCard()));	
		} else if (this->op == NonAlgOpNode::join){
			this->setCard(this->mulCard(this->getLArg()->getCard(), this->getRArg()->getCard()));	
		} else if (this->op == NonAlgOpNode::where){
			this->setCard(this->getLArg()->getCard());
		} else {
			this->setCard("0..*");
		}
	}
	
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string firstOpenSectS, lastOpenSectS;

        c << firstOpenSect; c >> firstOpenSectS;
        c << lastOpenSect; c >> lastOpenSectS;

        string result = getPrefixForLevel( level, name ) + "[NonAlgOp] op=" + opStr() +
          ", firstOpenSect='" + firstOpenSectS + "', lastOpenSect='" + lastOpenSectS + "'\n";

        if( recursive ) {
          if( larg )
            result += larg->toString( level+1, true, "larg" );
          if( rarg )
            result += rarg->toString( level+1, true, "rarg" );
        }

        return result;
      }
	virtual int getFirstOpenSect() {return this->firstOpenSect;}
	virtual void setFirstOpenSect(int newSectNr) {this->firstOpenSect = newSectNr;}
	virtual int getLastOpenSect() {return this->lastOpenSect;}
	virtual void setLastOpenSect(int newNr) {this->lastOpenSect = newNr;}
	
//	virtual Signature *combine(Signature *lRes, Signature *rRes);
	
	virtual int putToString() {
	    cout << "(";
	    if (larg!= NULL) larg->putToString();
	    else cout << "___";
	    cout << this->opStr() << this->getCard() << (this->getNeeded()?"Need":"Del");
		cout << "<" << firstOpenSect << ","<< lastOpenSect << ">";
	    if (rarg!= NULL) rarg->putToString();
	    else cout << "___";	    
	    cout << ")";    
	    return 0;
	}
	virtual void serialize(){
		 cout << "(";
	    if (larg!= NULL) larg->serialize();
	    else cout << "___";
	    cout << " " << this->opStr() << " ";
	    if (rarg!= NULL) rarg->serialize();
	    else cout << "___";	    
	    cout << ")";  
	}
	virtual string opStr() {
	    int op = this->getOp();
	    if (op == 0) return ".";
	    if (op == 1) return "join";
	    if (op == 2) return "where";
	    if (op == 3) return "closeBy";
	    if (op == 4) return "closeUniqueBy";
	    if (op == 5) return "leavesBy";
	    if (op == 6) return "leavesUniqueBy";
	    if (op == 7) return "orderBy";	    
	    if (op == 8) return "exists";	    
	    if (op == 9) return "forAll";
	    if (op == 10) return "forEach";	    
	    return "~~~";	}
	virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);	
	virtual int optimizeTree();	
//	virtual TreeNode *getHighestIndependant(); nieaktualne... ?
	virtual ~NonAlgOpNode() { if (larg != NULL) delete larg;
                            if (rarg != NULL) delete rarg; }
			    
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case NonAlgOpNode::dot: { 
				result = " (" + larg->deParse() + "." + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::join: { 
				result = " (" + larg->deParse() + "join" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::where: { 
				result = " (" + larg->deParse() + "where" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::closeBy: { 
				result = " (" + larg->deParse() + "close by" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::closeUniqueBy: { 
				result = " (" + larg->deParse() + "close unique by" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::leavesBy: { 
				result = " (" + larg->deParse() + "leaves by" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::leavesUniqueBy: { 
				result = " (" + larg->deParse() + "leaves unique by" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::orderBy: { 
				result = " (" + larg->deParse() + "order by" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::exists: { 
				result = " (" + larg->deParse() + "exists" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::forAll: { 
				result = " (" + larg->deParse() + "for all" + rarg->deParse() + ") "; 
				return result; 
			}
			case NonAlgOpNode::forEach: { 
				result = " for each" + larg->deParse() + "do" + rarg->deParse() + "od "; 
				return result; 
			}
		}
		return result;
	}
    };  

    class TransactNode : public TreeNode 
    {
    public:
		enum transactionOp { begin, end, abort }; 
    protected:
		transactionOp op;
    public:
		TransactNode (transactionOp _op) : op(_op) {}
		virtual TreeNode* clone();
		virtual int type() { return TreeNode::TNTRANS; }
		transactionOp getOp() { return op; }
		virtual void setOp(transactionOp _op) { op = _op; }

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        return getPrefixForLevel( level, name ) + "[Transact] op=" + ((op == begin) ? "begin" : ((op == end) ? "end" : "abort")) + "\n";
      }
        virtual int putToString() {
    	    cout << "(begin/end/abort)";
    	    return 0;
		}
		virtual ~TransactNode() {}
    };  

// query := CREATE query 
    class CreateNode : public QueryNode 
    {
    private:
        bool classMember;
    protected:
		QueryNode* arg;
		string name;
    public:
		CreateNode(string _name, QueryNode* _arg = NULL)
	            : arg(_arg), name(_name)  {this->classMember = false;}
		
		CreateNode(QueryNode* _arg) : arg(_arg){ this->classMember = false; name = "";}
		CreateNode(QueryNode* _arg, bool classMember) : arg(_arg) { this->classMember = classMember; name = "";}
		virtual TreeNode* clone();
		virtual int type() { return TreeNode::TNCREATE; }
		virtual bool isClassMember() { return classMember; }
		string getName() { return name; }
		TreeNode* getArg() { return arg; }
		virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
		virtual int putToString() {
		    cout << "(create " << this->getName();
		    if (arg != NULL) arg->putToString();
			cout << ")"; return 0;
		}
      virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
        string result = getPrefixForLevel( level, n ) + "[Create] name='" + name + "'\n";

        if( recursive ) {
          if( arg )
            result += arg->toString( level+1, true, "arg" );
        }

        return result;
      }
	  virtual int augmentTreeCoerce(int coerceType);
		virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
			newSon->setParent(this);
		    if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
		    else {/*an error from errorConsole is called;*/ return -1;}
		}    		
	
        virtual ~CreateNode() { if (arg != NULL) delete arg; }
	
	virtual string deParse() { string result; result = " (create " + arg->deParse() + ") "; return result; };
    };  


// query := WHILE q DO q  |  IF-THEN  |  IF-THEN-ELSE ...
    class CondNode : public QueryNode
    {
    public:
	enum condOp { ifElsee, iff, whilee };

    protected: 
	QueryNode* condition;
	QueryNode* larg;   /* if c then __QUERY__ (else q2) fi | while c do __QUERY__ od */
	QueryNode* rarg;   /* if c then q else __QUERY__ fi   (w p.p. rarg== NULL) */
	condOp op;
    public:
	CondNode (QueryNode* _condition, QueryNode* _larg, QueryNode* _rarg, condOp _op) 
	    : condition(_condition), larg(_larg), rarg(_rarg), op(_op)    
    	    {larg->setParent(this); rarg->setParent(this); }

	CondNode (QueryNode* _condition, QueryNode* _larg, condOp _op)
	    : condition(_condition), larg(_larg), op(_op)
	    {larg->setParent(this); rarg = NULL; }    
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNCOND;}
	virtual QueryNode* getCondition() { return condition; }
	QueryNode* getLArg() { return larg; }
	QueryNode* getRArg() { return rarg; }
	condOp getOp() { return op; }
	
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[Cond] op=" + ((op == ifElsee) ? "ifElses" : ((op == iff) ? "iff" : "whilee")) + "\n";

        if( recursive ) {
          if( condition )
            result += condition->toString( level+1, true, "condition" );
          if( larg )
            result += larg->toString( level+1, true, "larg" );
          if( rarg )
            result += rarg->toString( level+1, true, "rarg" );
        }

        return result;
      }
	virtual int putToString() {
	    cout << "(";
	    if (op == 0 || op == 1) {
		cout << "if ";
		if (condition != NULL) condition->putToString(); else cout << "_/conditiion/_";
		cout << " then ";
		if (larg != NULL) larg->putToString(); else cout << "_/query1/_";
		if (op == 0) {	cout << " else ";
    		    if (rarg != NULL) rarg->putToString(); else cout << "_/query2/_";
		}
		cout << " fi";
	    } else if (op == 2) { /*while*/
		cout << "while ";
		if (condition != NULL) condition->putToString(); else cout << "_/conditiion/_";
		cout << " do ";
		if (larg != NULL) larg->putToString(); else cout << "_/query1/_";
		cout << " od";	    
	    }
	    cout << ")";
	    return 0;
	}
	
	virtual ~CondNode() {if (larg != NULL) delete larg; if (rarg != NULL) delete rarg; if (condition != NULL) delete condition; }
	
	virtual string deParse() { 
		string result = ""; 
		switch (op) {
			case CondNode::iff: { 
				result = " if" + condition->deParse() + "then" + larg->deParse() + "fi "; 
				return result; }
			case CondNode::ifElsee: { 
				result = " if" + condition->deParse() + "then" + larg->deParse() + "else" + rarg->deParse() + "fi "; 
				return result; }
			case CondNode::whilee: { 
				result = " while" + condition->deParse() + "do" + larg->deParse() + "od "; 
				return result; }
		}
		return result;
	};
    };

// query := np.: link "Gdansk" "violet255" 3360 
    class LinkNode : public QueryNode
    {
    protected:
	string nodeName;	/* np. "Gdansk"*/
	string ip;		/* np. "125.100.100..." albo "yellow12"*/
	int port;		
    
    public: 
	LinkNode (string _nodeName, string _ip, int _port)
	    : nodeName(_nodeName), ip(_ip), port(_port) {}
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNLINK;};
	virtual string getNodeName() {return this->nodeName;}
	virtual string getIp() {return this->ip;}
	virtual int getPort() {return this->port;}	
	
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string portS;

        c << port; c >> portS;
        return getPrefixForLevel( level, name ) + "[Link] nodeName='" + nodeName + "', ip='" + ip + "', port=" + portS + "\n";
      }

	virtual int putToString() {
	    cout << "( link " << this->getNodeName() << " - " << this->getIp()<< " - " << this->getPort() << " )";
	    return 0;	
	}
	virtual ~LinkNode() {}
	
	virtual string deParse() { 
		stringstream ss;
		string str;
		ss << port;
		ss >> str; 
		string result; 
		result = " (link \"" + nodeName + "\" \"" + ip + "\" " + str + ") "; 
		return result; }
    };
    
    class RemoteNode : public TreeNode
    {
    	private:
    		LogicalID* lid;
    		bool deref;
    		
    	public:
    		virtual int type() {return TreeNode::TNREMOTE;}
    		virtual TreeNode* clone() {return new RemoteNode();}
    		virtual int putToString() {cout << "TreeNode" << endl; return 0;}
    		LogicalID* getLogicalID() {return lid;}
    		void setLogicalID(LogicalID* l) {lid = l;}
    		bool isDeref() {return deref;}
    		void setDeref(bool b) {deref = b;}
    };

    class FixPointNode : public QueryNode
    {
    protected:	
	vector<QueryNode*> queries;
	vector<string>  names;
	int partsNumb;
    public:
	FixPointNode () {}
	FixPointNode ( string singleName, QueryNode* singleQuery) {
	    queries.push_back(singleQuery);
	    names.push_back(singleName);
	    partsNumb = 1;
	}
	
	FixPointNode (string singleName, QueryNode* singleQuery, FixPointNode *tail) {
	    queries = tail->getQueries();
	    names = tail->getNames();
	    queries.push_back(singleQuery);
	    names.push_back(singleName);
	    partsNumb = 1 + tail->howManyParts();
	    	    // ... should I delete tail here...?
	}

      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        stringstream c;
        string partsNumbS;
        string namesS = "[";

        for( unsigned i = 0; i < names.size(); i++ ) {
          namesS += ((i > 0) ? ", " : "") + names[i];
        }
        namesS += "]";

        c << partsNumb; c >> partsNumbS;
        string result = getPrefixForLevel( level, name ) + "[FixPoint] partsNumb=" + partsNumbS + ", names=" + namesS + "\n";

        if( recursive ) {
          for( unsigned int i = 0; i < queries.size(); i++ ) {
            result += (queries[i])->toString( level+1, true, "query" );
          }
        }

        return result;
      }

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNFIX;};
	virtual vector<QueryNode*> getQueries() {return this->queries;}
	virtual QueryNode* getQuery(int i) {return this->queries.at(i);}
	virtual vector<string> getNames() {return this->names;}
	virtual string getName(int i) {return this->names.at(i); }
	virtual int howManyParts() {return this->partsNumb;}
	virtual void setQueries(vector<QueryNode*> q) {queries = q;}
	virtual void setNames(vector<string> n) {names = n;}
	virtual void setPartsNumb(int n) {partsNumb = n;}
	virtual int putToString() {
	    cout << " Fixpoint {";
	    for (int i = 0; i < partsNumb; i++) {
		cout << "[" << names.at(i) << " :- ";
		queries.at(i)->putToString();
		cout << "] ";
	    }
	    cout << "} ";
	    return 0;
	}
	
	virtual ~FixPointNode() { 
		for (unsigned int i=0; i < queries.size(); i++) {
			if (queries[i] != NULL) delete queries[i]; } 
		queries.clear(); 
	}
    
	virtual string deParse() { 
		string result = " fixpoint ( "; 
		for (int i = 0; i < partsNumb; i++) {
			result = result + names[i] + " :-" + queries[i]->deParse();
		}
		result = result + ") ";
		return result; };
    };
    
    class ReturnNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
    	ReturnNode() { this->query = NULL; }
	ReturnNode(QueryNode *q) { this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNRETURN;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " return < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~ReturnNode() {if (query != NULL) delete query;} 
      
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[Return]\n";

        if( recursive ) {
          if( query != NULL )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result;
      	if( query != NULL )
      		result = " (return " + query->deParse() + ") ";
      	else
      		result = " break "; 
      	return result; 
      	};
    };
    
    class ThrowExceptionNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
	ThrowExceptionNode(QueryNode *q) { this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNTHROWEXCEPTION;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " throw < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~ThrowExceptionNode() {if (query != NULL) delete query;} 
      
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[Throw]\n";

        if( recursive ) {
          if( query != NULL )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result;
      	if( query != NULL )
      		result = " (throw " + query->deParse() + ") ";
      	else
      		result = " "; 
      	return result; 
      	};
    };
    
    class RegisterProcNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
	RegisterProcNode(QueryNode *q) {this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNREGPROC;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " RegisterProc < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~RegisterProcNode() {if (query != NULL) delete query;} 
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RegisterProc]\n";

        if( recursive ) {
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { string result; result = " (create" + query->deParse() + ") "; return result; };
    };
    
    
    
    class ProcedureNode : public QueryNode
    {
    protected:
	vector<string> params;
	string name;
	QueryNode* code;
	unsigned int paramsNumb;
    public:
	ProcedureNode() { paramsNumb = 0; }
	ProcedureNode(string single_param) {
		params.push_back(single_param);
		paramsNumb = 1;
	}
	ProcedureNode(string single_param, ProcedureNode *tail) {
		params = tail->getParams();
		params.push_back(single_param);
		paramsNumb = 1 + tail->getParamsNumb();
		//delete tail;
	}
	ProcedureNode(string n, QueryNode* c, vector<string> p, unsigned int pN) {
		name = n;
		code = c;
		params = p;
		paramsNumb = pN;
	}
	virtual void completeStaticName(string className) {
		name = className + "::" + name;
	}
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNPROC;}
	virtual ~ProcedureNode() {if (code != NULL) delete code; } 
	
	virtual void addContent(string n, QueryNode* c) {name = n; code = c;}
	virtual void addParam(string p) { params.push_back(p); paramsNumb++; }
	virtual string getName() { return name; }
	virtual QueryNode* getCode() { return code; }
	virtual vector<string> getParams() { return params; }
	virtual unsigned int getParamsNumb() { return paramsNumb; }
      
      	virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        	string result = getPrefixForLevel( level, _name ) + "Procedure name - " + name + "\n";
		result += getPrefixForLevel( level + 1, _name ) + "ProcBody - " + code->deParse() + "\n";
        	for (unsigned int i = 0; i < paramsNumb; i++) {
			result += getPrefixForLevel( level + 1, _name ) + " Param - " + params[i] + "\n";
		}
        	return result;
      	}
	
	virtual int putToString() {
	    cout << "Procedure name - " << name << endl;
	    cout << "  ProcBody - " << code->deParse() << endl;
        for (unsigned int i = 0; i < paramsNumb; i++) {
		cout << "  Param - " << params[i] << endl;
	}	    
	    return 0;
	}
	
	virtual string deParse() { 
		string result = " procedure " + name + "("; 
		for (unsigned int i = 0; i < paramsNumb; i++) {
			result = result + params[i];
			if (i < (paramsNumb - 1)) result = result + ",";
		}
		result = result + ") {" + code->deParse() + "} ";
		return result; };
    }; 
    
    class CallProcNode : public QueryNode
    {
    protected:	
	vector<QueryNode*> queries;
	string name;
	unsigned int partsNumb;
    public:
	CallProcNode (string singleName) {
	    name = singleName;
	    partsNumb = 0;
	}
	CallProcNode (string singleName, VectorNode *vec) {
	    queries = vec->getQueries();
	    name = singleName;
	    partsNumb = vec->howManyParts();
	    //delete vec;
	}

      virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        stringstream c;
        string partsNumbS;
        string namesS = "[" + name + "]";

        c << partsNumb; c >> partsNumbS;
        string result = getPrefixForLevel( level, name ) + "[CallProc] partsNumb=" + partsNumbS + ", name=" + namesS + "\n";

        if( recursive ) {
          for( unsigned int i = 0; i < queries.size(); i++ ) {
            result += (queries[i])->toString( level+1, true, "query" );
          }
        }

        return result;
      }

	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNCALLPROC;};
	virtual vector<QueryNode*> getQueries() {return this->queries;}
	virtual QueryNode* getQuery(int i) {return this->queries.at(i);}
	virtual string getName() {return this->name;}
	virtual unsigned int howManyParts() {return this->partsNumb;}
	virtual void setQueries(vector<QueryNode*> q) {queries = q;}
	virtual void setName(string n) {name = n;}
	virtual void setPartsNumb(int n) {partsNumb = n;}
	virtual int putToString() {
	    cout << " CallProc " << name << " (";
	    for (unsigned int i = 0; i < partsNumb; i++) {
		queries.at(i)->putToString();
		if (i < (partsNumb - 1)) cout << " , ";
	    }
	    cout << ") ";
	    return 0;
	}
	
	virtual ~CallProcNode() {
		for (unsigned int i=0; i < queries.size(); i++) {
			if (queries[i] != NULL) delete queries[i]; }
		queries.clear();
	}
	
    	virtual string deParse() { 
		string result = " " + name + "("; 
		for (unsigned int i = 0; i < partsNumb; i++) {
			result = result + queries[i]->deParse();
			if (i < (partsNumb - 1)) result = result + ",";
		}
		result = result + ") ";
		return result; };
    };



class RegisterViewNode : public QueryNode
    {
    protected:
	QueryNode* query;
    public:
	RegisterViewNode(QueryNode *q) {this->query = q; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNREGVIEW;}
	virtual QueryNode *getQuery() {return this->query;}
	virtual int putToString() {
	    cout << " RegisterView < ";
	    if (query != NULL) query->putToString();
	    else cout << "_no_query_";
	    cout << ">";
	    return 0;
	}
	
	virtual ~RegisterViewNode() {if (query != NULL) delete query;} 
      virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RegisterView]\n";

        if( recursive ) {
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
      }
      
      virtual string deParse() { 
      	string result; 
      	result = " (create" + query->deParse() + ") "; 
      	cout << result;
      	return result; };
    };


class ViewNode : public QueryNode
    {
    protected:
	string name;
	ProcedureNode* virtual_objects;
	vector<QueryNode*> procedures;
	vector<QueryNode*> subviews;
	vector<QueryNode*> objects;
    public:
	ViewNode(ViewNode *v) { subviews.push_back(v); }
	ViewNode(ProcedureNode *p) { procedures.push_back(p); }
	ViewNode(QueryNode *q) { objects.push_back(q); }
	ViewNode(string n) { name = n; }
	
	virtual TreeNode* clone();
	virtual int type() {return TreeNode::TNVIEW;}
	virtual ~ViewNode() {
		for (unsigned int i=0; i < procedures.size(); i++) {
			if (procedures[i] != NULL) delete procedures[i]; }
		procedures.clear();
		for (unsigned int i=0; i < subviews.size(); i++) {
			if (subviews[i] != NULL) delete subviews[i]; }
		subviews.clear();
		for (unsigned int i=0; i < objects.size(); i++) {
			if (objects[i] != NULL) delete objects[i]; }
		objects.clear();
		if (virtual_objects != NULL) delete virtual_objects;
		
	} 
      
	virtual void setName(string n) { name = n; }
	virtual void setVirtual(string n, QueryNode* c) { 
		virtual_objects = new ProcedureNode();
		virtual_objects->addContent(n, c);
	} 
	virtual void addContent(ViewNode *v) {
		for (unsigned int i=0; i < v->procedures.size(); i++) {
			procedures.push_back(v->procedures[i]);
		}
		for (unsigned int i=0; i < v->subviews.size(); i++) {
			subviews.push_back(v->subviews[i]);
		}
		for (unsigned int i=0; i < v->objects.size(); i++) {
			objects.push_back(v->objects[i]);
		}
	}
	virtual vector<QueryNode*> getProcedures() { return this->procedures; }
	virtual vector<QueryNode*> getSubviews() { return this->subviews; }
	virtual vector<QueryNode*> getObjects() { return this->objects; }
	virtual string getName() { return name; }
	virtual QueryNode* getVirtualObjects() { return virtual_objects; }
      
      	virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
        	string result = getPrefixForLevel( level, _name ) + "View name - " + name + "\n";
        	result = result + virtual_objects->toString( level+1, true, "query" );
		for (unsigned int i=0; i < procedures.size(); i++) {
			result = result + (procedures[i])->toString( level+1, true, "query" );
		}
		for (unsigned int i=0; i < subviews.size(); i++) {
			result = result + (subviews[i])->toString( level+1, true, "query" );
		}
		for (unsigned int i=0; i < objects.size(); i++) {
			result = result + " create " + (objects[i])->toString( level+1, true, "query" );
		}
        	return result;
      	}
	
	virtual int putToString() {
	    cout << "View name - " << name << endl;
	    virtual_objects->putToString();
	    for (unsigned int i=0; i < procedures.size(); i++) {
		procedures.at(i)->putToString();
	    }
	    for (unsigned int i=0; i < subviews.size(); i++) {
		subviews.at(i)->putToString();
	    }
	    for (unsigned int i=0; i < objects.size(); i++) {
		cout << " create ";
		objects.at(i)->putToString();
	    }
	    return 0;
	}
	
	virtual string deParse() { 
		string result = " view " + name + " { virtual objects " + virtual_objects->getName();
		result = result + " { " + virtual_objects->getCode()->deParse() + " } ";
		for (unsigned int i=0; i < procedures.size(); i++) {
			result = result + procedures[i]->deParse();
		}
		for (unsigned int i=0; i < subviews.size(); i++) {
			result = result + subviews[i]->deParse();
		}
		for (unsigned int i=0; i < objects.size(); i++) {
			result = result + " create " + objects[i]->deParse();
		}
		result = result + " } ";
		cout << result;
		return result; };
    }; 


class VirtualizeAsNode : public QueryNode {
protected:
	QueryNode* query;
	QueryNode* sub_query;
	string name;
	
public:
	VirtualizeAsNode() { }
	VirtualizeAsNode(QueryNode* q, string n) { query = q; name = n; sub_query = NULL; }
	VirtualizeAsNode(QueryNode* q, QueryNode *v, string n) { query = q; sub_query = v, name = n; }
	virtual ~VirtualizeAsNode() { 
		if (query != NULL) delete query;
		if (sub_query != NULL) delete sub_query;
	} 
	virtual TreeNode* clone();
	
	virtual int type() {
		return TreeNode::TNVIRTUALIZEAS;
	}
	
	virtual QueryNode* getQuery() {
		return query;
	}
	
	virtual QueryNode* getSubQuery() {
		return sub_query;
	}
	
	virtual string getName() {
		return name;
	}
	
	virtual int putToString() {
		if (query != NULL) {
			query->putToString();
			cout << endl;
		}
		else cout << " no query " << endl;
		cout << " virtualize as ";
		if (sub_query != NULL) {
			cout << "(" << endl;
			sub_query->putToString();
			cout << ")." << endl;
		}
		cout << name;
		return 0;
	}
	
	virtual string toString(int level = 0, bool recursive = false, string _name = "") {
		string result;
		if (query != NULL) result = query->toString(level+1, true, "query" ) + "\n";
		else result = getPrefixForLevel(level, _name) + " no query " + "\n";
		result = result + getPrefixForLevel(level, _name) + " virtualize as ";
		if (sub_query != NULL) {
			result = result + getPrefixForLevel(level, _name) + "(" + "\n";
			result = result + query->toString(level+1, true, "query" ) + "\n";
			result = result + getPrefixForLevel(level, _name) + ").";
		}
		result = result + getPrefixForLevel(level, _name) + name + "\n"; 
		return result;
	}

	virtual string deParse() { 
		string result;
		string tmp_res;
		if (sub_query != NULL) tmp_res = "(" + sub_query->deParse() + ").";
		else tmp_res = "";
		if( query != NULL ) result = query->deParse() + "virtualize as " + tmp_res + name + " ";
		else result = " "; 
		return result; 
	};
};




class ClassesObjectsNode : public TwoArgsNode {
protected:
	virtual void init(QueryNode* larg, QueryNode* rarg) {
		if(larg != NULL) this->setLArg(larg);
		if(rarg != NULL) this->setRArg(rarg);
	}
	virtual string getOperationName() = 0;
	virtual string getKeyWord() = 0;
	
public:
	virtual ~ClassesObjectsNode() { delete larg; delete rarg; }
	ClassesObjectsNode(QueryNode* classesQuery, QueryNode* objectsQuery) {
		init(classesQuery, objectsQuery);
	}
	virtual QueryNode *getObjectsQuery() {return this->rarg;}
    virtual QueryNode *getClassesQuery() {return this->larg;}
    
    virtual int putToString() {
        cout << " " << getOperationName() << " < ";
        if (getObjectsQuery() != NULL) getObjectsQuery()->putToString();
        else cout << "_no_objects_query_";
        cout<< ", ";
        if (getClassesQuery() != NULL) getClassesQuery()->putToString();
        else cout << "_no_class_query_";
        cout << ">";
        return 0;
    }
    
	virtual string toString(int level = 0, bool recursive = false, string name = "" ) {
		string result = getPrefixForLevel( level, name ) + "[" + getOperationName() + "]\n";
		if( recursive ) {
  			if( larg != NULL )
    			result += larg->toString( level+1, true, "query" );
    		if( rarg != NULL )
    			result += rarg->toString( level+1, true, "query" );
    	}
    	return result;
	}
      
	virtual string deParse() { 
        string result; 
        result = " (" + larg->deParse() + ") " + getKeyWord() + " (" + rarg->deParse() + ") "; 
        cout << result;
        return result;
    }
};

class ExcludesNode : public ClassesObjectsNode {

protected:
	virtual string getKeyWord() { return "excludes"; }
	virtual string getOperationName() { return "Excludes"; }
public:
	ExcludesNode(QueryNode* classesQuery, QueryNode* objectsQuery)
		: ClassesObjectsNode(classesQuery, objectsQuery) {}
    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNEXCLUDES;}
};

class IncludesNode : public ClassesObjectsNode {
protected:
	virtual string getKeyWord() { return "includes"; }
	virtual string getOperationName() { return "Includes"; }
public:
	IncludesNode(QueryNode* classesQuery, QueryNode* objectsQuery)
		: ClassesObjectsNode(classesQuery, objectsQuery) {}
    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNINCLUDES;}
};

class ObjectsClassesNode : public ClassesObjectsNode {
public:
	ObjectsClassesNode(QueryNode* objectsQuery, QueryNode* classesQuery)
		: ClassesObjectsNode(objectsQuery, classesQuery) {}
	virtual QueryNode *getObjectsQuery() {return this->larg;}
    virtual QueryNode *getClassesQuery() {return this->rarg;}
};

class InstanceOfNode : public ObjectsClassesNode {
protected:
	virtual string getKeyWord() { return "instanceof"; }
	virtual string getOperationName() { return "Instanceof"; }
public:
	InstanceOfNode(QueryNode* objectsQuery, QueryNode* classesQuery)
		: ObjectsClassesNode(objectsQuery, classesQuery) {}
    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNINSTANCEOF;}
};

class ClassCastNode : public ObjectsClassesNode {
protected:
	virtual string getKeyWord() { return "cast"; }
	virtual string getOperationName() { return "Cast"; }
public:
	ClassCastNode(QueryNode* objectsQuery, QueryNode* classesQuery)
		: ObjectsClassesNode(objectsQuery, classesQuery) {}
    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNCAST;}
	/*virtual string deParse() {
		 return "((" + getClassesQuery()->deParse() +")" + getObjectsQuery()->deParse() + ")";
	}*/
};
 
class ClassNode : public QueryNode {
protected:
	CreateType createType;
    string name;
    string invariant;
    NameListNode* extends;
    NameListNode* fields;
    NameListNode* staticFields;
    //ProcedureNode* virtual_objects;
    vector<QueryNode*> procedures;
    vector<QueryNode*> staticProcedures;
    //vector<QueryNode*> subviews;

	virtual void deleteFromVector(vector<QueryNode*>& vec) {
		for (unsigned int i=0; i < vec.size(); i++) {
           delete vec[i];
        }
        vec.clear();
	}
    virtual void emptyInit(){
        extends = NULL; 
        fields = NULL;
        staticFields = NULL;
    }
    virtual void proceduresInit(ProcedureNode *p, vector<QueryNode*>& procVec){
        emptyInit();
        procVec.push_back(p);
    }
    
    virtual void completeStaticNames() {
    	for (unsigned int i=0; i < staticProcedures.size(); i++) {
            ((ProcedureNode*)staticProcedures[i])->completeStaticName(name);
        }
    }
    
public:

	virtual CreateType getCreateType() { return createType;}
	virtual void setCreateType(CreateType createType) { this->createType = createType; }

    //ViewNode(ViewNode *v) { subviews.push_back(v); }
    ClassNode() {emptyInit();}
    ClassNode(ProcedureNode *p) { proceduresInit(p, procedures); }
    ClassNode(ProcedureNode *p, bool isStatic) {
    	proceduresInit(p, isStatic?staticProcedures:procedures); 
	}
    ClassNode(string n) { setName(n); emptyInit(); }

    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNCLASS;}
    virtual ~ClassNode() {
        deleteFromVector(procedures);
        deleteFromVector(staticProcedures);
        delete extends;
        delete fields;
        delete staticFields;
        /*for (unsigned int i=0; i < subviews.size(); i++) {
            if (subviews[i] != NULL) delete subviews[i]; }
        subviews.clear();
        if (virtual_objects != NULL) delete virtual_objects;
        */
    } 

    virtual void setExtends(NameListNode* extends) { this->extends = extends; }
    virtual void setFields(NameListNode* fields) { this->fields = fields; }
    virtual void setStaticFields(NameListNode* staticFields) { this->staticFields = staticFields; }
    virtual void setName(string n) { name = n; completeStaticNames();  }
    virtual void setInvariant(string invariant) { this->invariant = invariant; }
    /*virtual void setVirtual(string n, QueryNode* c) { 
        virtual_objects = new ProcedureNode();
        virtual_objects->addContent(n, c);
    } */
    virtual void addContent(ClassNode *cn) {
        for (unsigned int i=0; i < cn->staticProcedures.size(); i++) {
            staticProcedures.push_back(cn->staticProcedures[i]);
        }
        cn->staticProcedures.clear();
        for (unsigned int i=0; i < cn->procedures.size(); i++) {
            procedures.push_back(cn->procedures[i]);
        }
        cn->procedures.clear();
        /*for (unsigned int i=0; i < v->subviews.size(); i++) {
            subviews.push_back(v->subviews[i]);
        }*/
    }
    virtual vector<QueryNode*> getProcedures() { return this->procedures; }
    virtual vector<QueryNode*> getStaticProcedures() { return staticProcedures; }
    virtual string getName() { return name; }
    virtual string getInvariant() { return invariant; }
    virtual NameListNode* getFields() { return fields; }
    virtual NameListNode* getStaticFields() { return staticFields; }
    virtual NameListNode* getExtends() {return extends;}
    
    /*virtual QueryNode* getVirtualObjects() { return virtual_objects; }

        virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
            string result = getPrefixForLevel( level, _name ) + "View name - " + name + "\n";
            result = result + virtual_objects->toString( level+1, true, "query" );
        for (unsigned int i=0; i < procedures.size(); i++) {
            result = result + (procedures[i])->toString( level+1, true, "query" );
        }
        for (unsigned int i=0; i < subviews.size(); i++) {
            result = result + (subviews[i])->toString( level+1, true, "query" );
        }
            return result;
        }
    */
    virtual int putToString() {
        cout << "Class name - " << name;
        if(extends){
            cout << " extends ";
            extends->putToString();
            cout << endl;
        }
        if(!invariant.empty()){
            cout << " invariant " << invariant << endl;
        }
        if(fields){
            cout << " fields ";
            fields->putToString();
            cout << endl;
        }
        if(staticFields != NULL) {
        	cout << " static fields ";
        	staticFields->putToString();
        	cout << endl;
        }
        //virtual_objects->putToString();
        for (unsigned int i=0; i < procedures.size(); i++) {
            procedures.at(i)->putToString();
        }
        
        for (unsigned int i=0; i < staticProcedures.size(); i++) {
        	cout << " static ";
            staticProcedures.at(i)->putToString();
        }
        /*for (unsigned int i=0; i < subviews.size(); i++) {
        subviews.at(i)->putToString();
        }*/
        return 0;
    }

    virtual string deParse() { 
        string result = " class " + name + " ";
        for (unsigned int i=0; i < procedures.size(); i++) {
            result = result + procedures[i]->deParse();
        }
        for (unsigned int i=0; i < staticProcedures.size(); i++) {
            result = result + " static " + staticProcedures[i]->deParse();
        }
        /*for (unsigned int i=0; i < subviews.size(); i++) {
            result = result + subviews[i]->deParse();
        }*/
        result = result + " } ";
        cout << result;
        return result; 
    }
}; 

class RegisterClassNode : public QueryNode {
protected:
    QueryNode* query;
    CreateType createType;
public:
    RegisterClassNode(QueryNode *q, CreateType createType):createType(createType) {
    	this->query = q;
    	(dynamic_cast<ClassNode*>(q))->setCreateType(createType);
    }
    
    virtual CreateType getCreateType() { return createType;}
    virtual TreeNode* clone();
    virtual int type() {return TreeNode::TNREGCLASS;}
    virtual QueryNode *getQuery() {return this->query;}
    virtual int putToString() {
        cout << " RegisterClass < ";
        if (query != NULL) query->putToString();
        else cout << "_no_query_";
        cout << ">";
        return 0;
    }
    
    virtual ~RegisterClassNode() {if (query != NULL) delete query;} 
  	virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
        string result = getPrefixForLevel( level, name ) + "[RegisterClass]\n";

        if( recursive ) {
          if( query )
            result += query->toString( level+1, true, "query" );
        }

        return result;
  	}
      
	virtual string deParse() { 
        string result; 
        result = " (create" + query->deParse() + ") "; 
        cout << result;
        return result; 
    }
};



class DMLNode : public TreeNode 
    {
    public:
		enum dmlInst { reload, tcoff, tcon }; 
    protected:
		dmlInst inst;
    public:
		DMLNode (dmlInst _inst);
		virtual TreeNode* clone();
		virtual int type() { return TreeNode::TNDML; }
		dmlInst getInst() { return inst; }
		virtual void setInst(dmlInst _inst) { inst = _inst; }
		virtual void qpLocalAction(QueryParser *qp);
        virtual int putToString() {
			switch (inst) {
				case DMLNode::reload : 	cout << "dml reloadScheme"; break;
				case DMLNode::tcoff : 	cout << "dml typechecker-off"; break;
				case DMLNode::tcon : 	cout << "dml typechecker-on"; break;
			}
    	    return 0;
		}
		virtual ~DMLNode() {}
    };  
    
	/** TypeCheck TreeNode class declarations... */
	class StructureTypeNode; 
    
	class SignatureNode : public TreeNode
	{
	
		public:
			enum SignatureKind {atomic, ref, defType, complex};
		private:
			SignatureKind sigKind;
			string atomType;		//for atomic
			string typeName;		//for ref, defType
			StructureTypeNode *arg;	//for complex
		public: 
			SignatureNode (SignatureKind _sigKind, string detail) {
				cout << "doing sig Node, with detail: " << detail << endl;
				this->sigKind = _sigKind;
				this->arg = NULL;
				if (this->sigKind == SignatureNode::atomic) { 
					atomType = detail; typeName = "";
				} else {
					typeName = detail; atomType = "";
				}
			}
			SignatureNode (StructureTypeNode *_arg) {
				this->sigKind = SignatureNode::complex;
				this->arg = _arg;
				this->atomType = ""; this->typeName = "";
			}
			SignatureNode() {};
			virtual TreeNode* clone();
			virtual int type() { return TreeNode::TNSIGNATURE; }
			virtual int getSigKind() {return sigKind;}
			virtual StructureTypeNode *getStructArg() {return arg;}
			virtual string getTypeName() {return typeName;}
			virtual Signature *createSignature();
			virtual Signature *recCreateSig(BinderWrap *bwObj, BinderWrap *bwTypes);
			virtual int putToString();
			virtual string getHeadline();
			virtual string deParse();
			virtual ~SignatureNode();
	};
	
	class DeclareDefineNode : public TreeNode
	{
		protected:
			string name;
			SignatureNode *signature;
		public:
			virtual TreeNode* clone() = 0;
			virtual int type() = 0;
			virtual string getName() {return name;}
			virtual SignatureNode *getSigNode() {return signature;}
			virtual int putToString() = 0;
			virtual string deParse() = 0;
	};
	
	class ObjectDeclareNode : public DeclareDefineNode
	{
		private:
			string card;
		public:
			ObjectDeclareNode (string _name, SignatureNode *sig) {
				this->name = _name;
				this->card = "1..1"; //the default cardinality... 
				this->signature = sig;
			}
			ObjectDeclareNode (string _name, SignatureNode *sig, string cardinality) {
				this->name = _name;
				this->card = cardinality;
				this->signature = sig;
			}
			ObjectDeclareNode() {};
			virtual TreeNode* clone();
			virtual int type() { return TreeNode::TNOBJDECL; }
			virtual string getName() {return name;}
			virtual string getCard() {return card;}
			virtual SignatureNode *getSigNode() {return signature;}
			virtual int putToString() {
				cout << "<" << name << "[" << card << "] : ";
				signature->putToString();
				cout << ">";
				return 0;
			}	
			virtual string deParse();
			virtual ~ObjectDeclareNode ();
	};
		
	class StructureTypeNode : public TreeNode
	{
		protected:	
			vector<ObjectDeclareNode*> *subDeclarations; //sub-declarations... 
		public:
			StructureTypeNode (ObjectDeclareNode* singleDecl, StructureTypeNode *tail) {
				subDeclarations = tail->getSubDeclarations();
			
				subDeclarations->push_back(singleDecl);
			//delete tail;
			}
			StructureTypeNode (ObjectDeclareNode* singleDecl) {
				subDeclarations = new vector<ObjectDeclareNode*>();
				subDeclarations->push_back(singleDecl);
			}
			StructureTypeNode () {subDeclarations = new vector<ObjectDeclareNode*>();};
	
			virtual TreeNode* clone();
			virtual int type() {return TreeNode::TNSTRUCTTYPE;};
			virtual vector<ObjectDeclareNode*> *getSubDeclarations() {return this->subDeclarations;}
			virtual ObjectDeclareNode* getSubDecl(int i) {return this->subDeclarations->at(i);}
			virtual void setSubDeclarations(vector<ObjectDeclareNode*> *odns);
			virtual unsigned int howManyParts() { return subDeclarations->size(); }

			virtual string toString( int level = 0, bool recursive = false, string _name = "" ) {
				string result = "";
				return result;
			}
			virtual string deParse();
			virtual int putToString();
			virtual ~StructureTypeNode();
	};	
	
	class TypeDefNode : public DeclareDefineNode
	{
		private:
			bool isDistinct;
		public:
			TypeDefNode() {};
			TypeDefNode(string _name, SignatureNode *sig, bool distinct) {
				this->name = _name;
				this->signature = sig;
				this->isDistinct = distinct;
			}

			virtual TreeNode* clone();
			virtual int type() { return TreeNode::TNTYPEDEF; }
			virtual bool getIsDistinct() {return isDistinct;}
			virtual int putToString() {
				cout << "<typedef ";
				if (this->isDistinct) cout << "distinct ";
				cout << this->name << " = ";
				this->signature->putToString();
				cout << " >";
				return 0;
			}
			virtual string deParse();
			virtual ~TypeDefNode() {if (this->signature != NULL) delete this->signature;};
	};
	
	
	class CoerceNode : public QueryNode 
	{
		public:
			enum CoerceType {to_string, to_double, to_bool, to_int, element, to_bag, to_seq, can_del, can_crt,ext_crt};
		protected:
			QueryNode* arg;
			int cType;
		public:
			CoerceNode(QueryNode* _arg, int _ctp) : arg(_arg), cType(_ctp) {arg->setParent(this);}
			virtual TreeNode* clone();
			virtual int type() { return TreeNode::TNCOERCE; }
			virtual TreeNode* getArg() { return arg; }
			int getCType() {return cType;}
			void seCType(int ctp) { cType = ctp; }
			virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }
			

			virtual void markNeeded(){
				this->setNeeded(true);
				this->getArg()->markNeeded();	
			}
			virtual void markNeeded2(){
				this->setNeeded(true);
				this->getArg()->markNeeded2();	
			}
			virtual TreeNode * getDeath(){
				return this->getArg()->getDeath();
			}
			virtual void getInfixList(vector<TreeNode*> *auxVec){
				auxVec->push_back(this);
				this->getArg()->getInfixList(auxVec);
			}

			virtual string toString( int level = 0, bool recursive = false, string name = "" ) {
				string result = getPrefixForLevel( level, name ) + "[Coerce] cType=" + ctStr() + "\n";
				if( recursive ) {
					if( arg ) result += arg->toString( level+1, true, "arg" );
				}
				return result;
			}
			virtual int putToString() {
				cout << "(" << ctStr() << " ";
				if (arg!= NULL) arg->putToString();
				else cout << "___";
				cout << ")";    
				return 0;
			}
			virtual void serialize(){
				cout << "(" << ctStr();
				if (arg!= NULL) arg->serialize();
				else cout << "___";
				cout << ")"; 
			}
			virtual string ctStr() {
				switch (cType) {
					case to_string : return "toString ";
					case to_double : return "toDouble ";
					case to_bool : return "toBoolean ";
					case element : return "element ";
					case to_bag : return "toBag ";
					case to_seq : return "toSequence ";
					case to_int : return "toInteger ";
					case can_del: return "canDelete ";
					case can_crt: return "createCheck ";
					case ext_crt: return "canCreateExt ";
				}
				return "_";
			}

			virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
				if (arg != oldSon) return -1;
				newSon->setParent(this);
				this->setArg((QueryNode *) newSon); 
				return 0;
			}    	
	
			virtual int staticEval (StatQResStack *&qres, StatEnvStack *&envs);		
			virtual int optimizeTree() {return arg->optimizeTree();}
			virtual ~CoerceNode() { if (arg != NULL) delete arg; }
	
			//coerces are hidden nodes - not seen through deparse - parser can't handle them.
			virtual string deParse() { 
				return arg->deParse();
			}

	};  
	
	
	class SimpleCastNode : public QueryNode 
	{
		protected:
			QueryNode* arg;
			SignatureNode* sig;
			//string name;
			//bool group;
			//vector<TreeNode*> *usedBy;	// wezly drzewa ktore sa w nim wiazane

		public:
			SimpleCastNode(QueryNode* _arg, SignatureNode* _sig)
			: arg(_arg), sig(_sig) { arg->setParent(this); }
			virtual TreeNode* clone();
			virtual int type() { return TreeNode::TNCASTTO; }
			//string getName() { return name; }
			TreeNode* getArg() { return arg; }
			virtual void setArg(QueryNode* _arg) { arg = _arg; arg->setParent(this); }    
			virtual SignatureNode* getSig() {return sig;}
			virtual void setSig(SignatureNode *_sig) {sig = _sig;}
			
			virtual int putToString() {
				cout << "( cast(";
				if (arg!= NULL) arg->putToString();
				else cout << "___";
				cout << " to ";
				if (sig != NULL) sig->putToString();
				else cout << "___";
				cout << "))";
				return 0;
			}
			virtual void serialize(){ this->putToString();};  
			
			virtual int swapSon (TreeNode *oldSon, TreeNode *newSon) {
				newSon->setParent(this);
				if (arg == oldSon) {this->setArg((QueryNode *) newSon); return 0;}
				else {return -1;}
			}    	
			virtual void markNeeded(){
				this->setNeeded(true);
				this->getArg()->markNeeded();	
			}
			virtual void markNeeded2(){
				this->setNeeded(true);
				this->getArg()->markNeeded2();	
			}
			virtual void evalCard(){this->setCard(this->getArg()->getCard());}
			virtual TreeNode * getDeath(){
				return this->getArg()->getDeath();
			}
			virtual void getInfixList(vector<TreeNode*> *auxVec){
				//auxVec->push_back(this);
				this->getArg()->getInfixList(auxVec);
			}
			//virtual int augmentTreeDeref();
			virtual int augmentTreeDeref(bool derefLeft, bool derefRight);
			virtual bool canDerefSon() {return this->getArg()->canDerefNode();}
			virtual int staticEval(StatQResStack *&qres, StatEnvStack *&envs);
			virtual Signature *createSignature() {return sig->createSignature();};
			virtual int optimizeTree() {return arg->optimizeTree();}
			virtual ~SimpleCastNode() { if (arg != NULL) delete arg; if (sig != NULL) delete sig;}
			virtual string toString( int level = 0, bool recursive = false, string n = "" ) {
				stringstream ss;
				ss << (sig == NULL ? -1 : sig->getSigKind());
				string result = getPrefixForLevel( level, n ) + "[SimpleCast] to '" + ss.str() + "'\n";
				if( recursive ) {
					if( arg )
						result += arg->toString( level+1, true, "arg" );
				}
				return result;
			}
      
			virtual string deParse() { 
				string result, tmp_res = ""; 
				result = " (cast (" + arg->deParse() + " to " + tmp_res + ")) "; 
				return result; 
			}
	};  

	
	/** End of TypeCheck TreeNode class declarations... */
	
	// Index Data Manipulation Language
    class IndexDMLNode : public QueryNode {
    	
    	private:
    		string name;
    		
    	public:
    		IndexDMLNode() {}
    		//virtual int execute(QueryResult **result)=0;//{return 0;};
	    	virtual int type() {return TreeNode::TNINDEXDML;}
	    	virtual void setIndexName(string name) {this->name = name;}
	    	virtual string getIndexName() {return name;}
	    	//IndexDMLNode(bool inclusiveLeft, bool inclusiveRight) : inclusiveLeft(inclusiveLeft) : inclusiveRight(inclusiveRight) {}
    };
    
    class IndexBoundaryNode {
    	friend class IndexSelectNode;
    	private:
    		bool inclusive;
    		QueryNode* value;	
    	public:
    		IndexBoundaryNode(bool inclusive, QueryNode* value) : inclusive(inclusive), value(value) {}
    };
    
    class IndexSelectNode : public IndexDMLNode {
    	protected:
    		Indexes::Constraints* c;
    		IndexSelectNode(Constraints* c) : c(c){};
    	public:
	    	IndexSelectNode(IndexBoundaryNode *left, IndexBoundaryNode *right);
	    	IndexSelectNode (QueryNode *exact);
	    	IndexSelectNode(IndexBoundaryNode *left, bool right);
	    	IndexSelectNode(bool left, IndexBoundaryNode *right);
	    	
	    	virtual ~IndexSelectNode(){}
	    	
	    	virtual Constraints* getConstraints(){return c;}
	    	
	    	virtual int putToString() {cout << "index select " << endl; return 0;}
	    	virtual TreeNode* clone() {return new IndexSelectNode(c);}//TODO moze trzeba bedzie poprawic
	    	//virtual int execute(QExecutor::QueryResult **result);//{return 0;};	
	    	virtual string deParse();
    };  
        
    class ComparatorNode {
    	public:
    		Indexes::Comparator* comp;
    		ComparatorNode(Indexes::Comparator* comp) : comp(comp) {};
    };
    
}
#endif
