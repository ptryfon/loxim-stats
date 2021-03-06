#include <stdio.h>
#include <set>
#include <iostream>
#include <fstream>
#include <ios>
#include <list>

#include <QueryExecutor/QueryExecutor.h>
#include <QueryExecutor/EnvironmentStack.h>
#include <QueryExecutor/ExecutorViews.h>
#include <QueryExecutor/ClassGraph.h>
#include <QueryExecutor/ResultStack.h>
#include <QueryExecutor/QueryBuilder.h>
#include <QueryExecutor/InterfaceMatcher.h>
#include <QueryExecutor/InterfaceMaps.h>
#include <QueryExecutor/BindNames.h>
#include <QueryExecutor/CommonOperations.h>
#include <QueryExecutor/AccessMap.h>
#include <TransactionManager/Transaction.h>
#include <Store/Store.h>
#include <QueryParser/ClassNames.h>
#include <QueryParser/QueryParser.h>
#include <QueryParser/TreeNode.h>
#include <QueryParser/IndexNode.h>
#include <QueryParser/Privilige.h>
#include <Errors/Errors.h>
#include <Errors/ErrorConsole.h>
#include <TypeCheck/DecisionTable.h>
#include <TypeCheck/TCConstants.h>
#include <TypeCheck/TypeChecker.h>
#include <Server/Session.h>

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace Indexes;
using namespace std;
using namespace Server;
using namespace Schemas;
using namespace CommonOperations;

namespace QExecutor {

QueryExecutor::QueryExecutor(Session *session) 
{ 
	envs = new EnvironmentStack(); 
	qres = new ResultStack();
	exViews = new ExecutorViews();
	prms = NULL; 
	tr = NULL; 
	inTransaction = false;
	antyStarve = false;
	transactionNumber = 0;
	ec = &ErrorConsole::get_instance(EC_QUERY_EXECUTOR); 
	stop = 0; 
	this->session = session;
	im = &InterfaceMaps::Instance();
	os = &OuterSchemas::Instance();
	am = new AccessMap();
}

int QueryExecutor::pop_qres(QueryResult *&r)
{
	return qres->pop(r);
}
	
int QueryExecutor::executeQuery(TreeNode *tree, map<string, QueryResult*> *params, QueryResult **result) {
	int errcode;
	prms = params;
#ifdef QUERY_CACHE
	// get from cache
	errorcode = QueryCacher::getHandle()->get(tree, result);
	if (errorcode < 0)
	    return errorcode;
	else if (errorcode == 0)
	    return 0;		//query retrived
#endif
	//printf("Debug: EQ: tree type is: %d\n", tree->type());
	errcode = executeQuery(tree, result);
	debug_print(*ec,  "[QE] past inner executeQuery");
#ifdef QUERY_CACHE
	if (errorcode != 0)
	    return errorcode;
	// put to cache, if cacheable
	if (treenode->isCacheable())
	    errorcode = QueryCacher::getHandle()->put(tree, *result);
	if (errorcode < 0)
	    return errorcode;
#endif
	//delete prms;
	prms = NULL;
	return errcode;
}


int QueryExecutor::executeQuery(TreeNode *tree, QueryResult **result) {

	int errcode;
	debug_print(*ec,  "[QE] executeQuery()");
	am->reset();

	if (tree != NULL) {
		int nodeType = tree->type();
		//tworzenie i usuwanie indeksow nie podlega transakcjom
		if (nodeType == (TreeNode::TNINDEXDDL)) {
			errcode = (dynamic_cast< IndexDDLNode*>(tree))->execute(session->get_id(), result);
			return errcode;
		} else
		if (! inTransaction) {
			if (nodeType == TreeNode::TNTRANS) {
				if (((TransactNode *) tree)->getOp() == TransactNode::begin) {
					if (antyStarve) {
						debug_printf(*ec, "[QE] Asking TransactionManager to REOPEN transaction number : %d\n", transactionNumber);
						errcode = TransactionManager::getHandle()->createTransaction(session->get_id(), tr, transactionNumber);
						antyStarve = false;
					}
					else {
						debug_print(*ec,  "[QE] Asking TransactionManager for a NEW transaction");
						errcode = TransactionManager::getHandle()->createTransaction(session->get_id(), tr);
					}
					if (errcode != 0) {
						debug_print(*ec,  "[QE] Error in createTransaction");
						debug_print(*ec,  "[QE] Transction not opened");
						*result = new QueryNothingResult();
						debug_print(*ec,  "[QE] nothing to do: QueryNothingResult created");
						inTransaction = false;
						return errcode;
					}
					else {
						transactionNumber = tr->getId()->getId();
						debug_printf(*ec, "[QE] Transaction number : %d opened succesfully\n", transactionNumber);
						*result = new QueryNothingResult();
						debug_print(*ec,  "[QE] nothing else to do: QueryNothingResult created");
						inTransaction = true;
						return 0;
					}
				}
				else {
					debug_print(*ec,  "[QE] Transction not opened, type < begin); > first");
					*result = new QueryNothingResult();
					debug_print(*ec,  "[QE] nothing to do: QueryNothingResult created");
					return 0;
				}
			}
			else {
				debug_print(*ec,  "[QE] Transction not opened, type < begin); > first");
				*result = new QueryNothingResult();
				debug_print(*ec,  "[QE] nothing to do: QueryNothingResult created");
				return 0;
			}
		}
		if (nodeType == (TreeNode::TNTRANS)) {
			TransactNode::transactionOp op = ((TransactNode *) tree)->getOp();
			switch (op) {
				case TransactNode::begin: {
					debug_print(*ec,  "[QE] ERROR! Transaction already opened. It can't be opened once more!");
					*result = new QueryNothingResult();
					debug_print(*ec,  (ErrQExecutor | ETransactionOpened));
					return (ErrQExecutor | ETransactionOpened);
				}
				case TransactNode::end: {
					errcode = tr->commit();
					inTransaction = false;
					if (errcode != 0) {
						debug_print(*ec,  "[QE] error in transaction->commit()");
						*result = new QueryNothingResult();
						return errcode;
					}
					debug_print(*ec,  "[QE] Transaction commited succesfully");
					break;
				}
				case TransactNode::abort: {
					errcode = tr->abort();
					inTransaction = false;
					if (errcode != 0) {
						debug_print(*ec,  "[QE] error in transaction->abort()");
						*result = new QueryNothingResult();
						return errcode;
					}
					debug_print(*ec,  "[QE] Transaction aborted succesfully");
					break;
				}
				default: {
					debug_print(*ec,  "[QE] ERROR! unknown transaction operator");
					*result = new QueryNothingResult();
					debug_print(*ec,  (ErrQExecutor | EUnknownNode));
					return (ErrQExecutor | EUnknownNode);
				}
			}
			*result = new QueryNothingResult();
			debug_print(*ec,  "[QE] Nothing else to do. QueryNothingResult created");
			return 0;
		}
		else if (nodeType == (TreeNode::TNVALIDATION)) {
		    const UserData *user_data = session->get_user_data();
			bool res;
		    /* if user_data != NULL -> user id logged in, cannot log twice in one sesion */
		    if (user_data != NULL) {
				res = false;
		    }
		    else {
				ValidationNode *val_node = (ValidationNode *) tree;
				if (priviliged_mode) {
					set_user_data(val_node);
					res = true;
				}
				else {
					string pass_check_query = QueryBuilder::getHandle()->
							query_for_password(val_node->get_login(), val_node->get_passwd());

					    QueryResult *local_res = NULL;
					int local_ret = execute_locally(pass_check_query, &local_res);
					if (local_ret || local_res == NULL || local_res->isBool() == false) {
						res = false;
					}
					else {
						bool b;
						local_res->getBoolValue(b);
						if (b) 
						{
							/* user_data is now NULL value */
							set_user_data(val_node);
						}
						res = b;
						delete local_res;
					}
				}
		    }
			*result = new QueryBoolResult(res);
		}
		else if (nodeType == (TreeNode::TNGRANTPRIV)) {
		    GrantPrivNode *grant_node = (GrantPrivNode *) tree;
		    PriviligeListNode *priv_list_or_null = grant_node->get_priv_list();

		    /* check if user has privilige to grant other privilige */
		    bool grant_priv = true;
		    while (priv_list_or_null != NULL) 
			{
				Privilige *privilige = priv_list_or_null->get_priv();
				string priv_name = privilige->get_priv_name();
				priv_list_or_null = priv_list_or_null->try_get_priv_list();

				NameListNode *name_list_or_null = grant_node->get_name_list();
				while (name_list_or_null != NULL) 
				{
					string name = name_list_or_null->get_name();
					name_list_or_null = name_list_or_null->try_get_name_list();

					grant_priv = grant_priv && am->canGiveAccess(name);
					if (grant_priv == false) break;
				}
		    }
		    if (grant_priv) 
			{
				string user = grant_node->get_user();
				int grant_option = grant_node->get_with_grant_option();
				priv_list_or_null = grant_node->get_priv_list();

				if (priv_list_or_null != NULL)
				{
					Privilige *privilige = priv_list_or_null->get_priv();
					string priv_name = privilige->get_priv_name();
					
					NameListNode *name_list_or_null = grant_node->get_name_list();
					if (name_list_or_null != NULL) 
					{
						string name = name_list_or_null->get_name();
						
						//Only works if granting read access to schema (as specified)
						debug_printf(*ec, "Checking for schema by name %s", name.c_str());
						if (os->hasSchemaName(name) && priv_name == Privilige::READ_PRIV)
						{
							//remove previous access for this user
							string query = QueryBuilder::getHandle()->revoke_all_privs_query(user);
							QueryResult *local_res = NULL;
							execute_locally(query, &local_res);	
							
							//grant read access to schema
							query = QueryBuilder::getHandle()->grant_priv_query(priv_name, name, user, grant_option);
							local_res = NULL;
							execute_locally(query, &local_res);			
						}
					}
				}
				*result = new QueryBoolResult(true);
		    }
		    else 
			{
				*result = new QueryBoolResult(false);
		    }
		}
		else if (nodeType == (TreeNode::TNREVOKEPRIV)) {
		    RevokePrivNode *revoke_node = (RevokePrivNode *) tree;
		    PriviligeListNode *priv_list_or_null = revoke_node->get_priv_list();

		    /* check if user has privilige to revoke other privilige */
		    bool revoke_priv = true;
		    while (priv_list_or_null != NULL) 
			{
				Privilige *privilige = priv_list_or_null->get_priv();
				string priv_name = privilige->get_priv_name();
				priv_list_or_null = priv_list_or_null->try_get_priv_list();

				NameListNode *name_list_or_null = revoke_node->get_name_list();
				while (name_list_or_null != NULL) 
				{
					string name = name_list_or_null->get_name();
					name_list_or_null = name_list_or_null->try_get_name_list();

					revoke_priv = revoke_priv && am->canRevokeAccess();
					if (revoke_priv == false) break;
				}
		    }
		    if (revoke_priv) 
			{
				string user = revoke_node->get_user();
				priv_list_or_null = revoke_node->get_priv_list();

				if (priv_list_or_null != NULL) 
				{
					Privilige *privilige = priv_list_or_null->get_priv();
					string priv_name = privilige->get_priv_name();

					NameListNode *name_list_or_null = revoke_node->get_name_list();
					if (name_list_or_null != NULL) 
					{
						string name = name_list_or_null->get_name();
						//Only works if revoking read access to schema (as specified)
						debug_printf(*ec, "Checking for schema by name %s", name.c_str());
						if (os->hasSchemaName(name) && priv_name == Privilige::READ_PRIV)
						{
							//remove access for this user
							string query = QueryBuilder::getHandle()->revoke_all_privs_query(user);
							QueryResult *local_res = NULL;
							execute_locally(query, &local_res);		
						}
					}
				}
			*result = new QueryBoolResult(true);
		    }
		    else 
			{
				*result = new QueryBoolResult(false);
		    }
		}
		else if (nodeType == (TreeNode::TNCREATEUSER)) {
		    CreateUserNode *createUserNode = (CreateUserNode *) tree;
		    bool create_priv = am->canCreateUser();

		    if (create_priv) {
			string user   = createUserNode->get_user();
			string passwd = createUserNode->get_passwd();
			string query = QueryBuilder::getHandle()->create_user_query(user, passwd);

			QueryResult *local_res = NULL;
			execute_locally(query, &local_res);

			*result = new QueryBoolResult(true);
		    }
		    else {
			*result = new QueryBoolResult(false);
		    }
		}
		else if (nodeType == (TreeNode::TNREMOVEUSER)) {
		    RemoveUserNode *removeUserNode = (RemoveUserNode *) tree;
		    bool remove_priv = am->canRemoveUser();
				
		    if (remove_priv) {
			string user   = removeUserNode->get_user();
			string query = QueryBuilder::getHandle()->remove_user_query(user);

			QueryResult *local_res = NULL;
			execute_locally(query, &local_res);

			*result = new QueryBoolResult(true);
		    }
		    else {
			*result = new QueryBoolResult(false);
		    }
		}	/** Type declarations / definitions, TC nodes */
		else if (nodeType == TreeNode::TNOBJDECL) {
			debug_print(*ec,  "Will try execute object declaration\n");
			errcode = this->executeObjectDeclarationOrTypeDefinition((DeclareDefineNode *) tree, false);
			*result = new QueryNothingResult();
			if (errcode != 0) return errcode;
		}
		else if (nodeType == TreeNode::TNTYPEDEF ) {
			errcode = this->executeObjectDeclarationOrTypeDefinition((DeclareDefineNode *) tree, true);
			*result = new QueryNothingResult();
			if (errcode != 0) return errcode;
		}
		else if (nodeType == TreeNode::TNDML) {
			DMLNode::dmlInst inst = ((DMLNode *) tree)->getInst();
			switch (inst) {
				case DMLNode::reload :
					tr->reloadDmlStct();
					DataScheme::reloadDScheme(session->get_id(), tr);
					if (DataScheme::dScheme(session->get_id())->getIsComplete()) {
					//*result = new QueryNothingResult();
						*result = new QueryStringResult("Datascheme consistent.");
					} else {
						DataScheme::dScheme(session->get_id())->getMissedRoots();
						string client_msg = "Datascheme incomplete. Missing entries: \n";
						for (unsigned int i = 0; i < DataScheme::dScheme(session->get_id())->getMissedRoots().size(); i++) {
							if (i > 0) client_msg += ", ";
							client_msg += DataScheme::dScheme(session->get_id())->getMissedRoots().at(i);
						}
						*result = new QueryStringResult(client_msg);
					}
					break;
				case DMLNode::tcoff : *result = new QueryNothingResult(); break;
				case DMLNode::tcon : *result = new QueryNothingResult();break;
				default: break;
			}
			return 0;
		}	/** These were type declarations / definitions */
		else {
			errcode = envs->pushDBsection();
			
			if (errcode == 0) {
				errcode = this->executeRecQuery(tree);
				if (errcode == EEvalStopped) errcode = 0;
			}
			int pop_errcode = envs->popDBsection();
			if (errcode == 0) errcode = pop_errcode;

			QueryResult *tmp_result;
			if (errcode == 0) errcode = qres->pop(tmp_result);

			int next_errcode = 0;
			if (errcode == 0) next_errcode = exViews->deVirtualize(tmp_result, *result);

			int howMany = QueryResult::getCount();
			debug_printf(*ec, " QueryResults in use: %d\n", howMany);

			if (errcode != 0) return errcode;
			if (next_errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Done!");
			return 0;
		}

	} //if tree != NULL
	else { // tree == NULL; return empty result
		debug_print(*ec,  "[QE] empty tree, nothing to do");
		*result = new QueryNothingResult();
		debug_print(*ec,  "[QE] QueryNothingResult created");
	}
	return 0;
}

void QueryExecutor::set_priviliged_mode(bool mode) {
    this->priviliged_mode = mode;
};

void QueryExecutor::set_user_data(ValidationNode *val_node) {
    string login  = val_node->get_login();
    string passwd = val_node->get_passwd();

    auto_ptr<UserData> user_data(new UserData(login, passwd));
    session->set_user_data(user_data);
	am->resetForUser(login, this);
};

int QueryExecutor::execute_locally(string query, QueryResult **res) {

    QueryParser parser(session->get_id());
    TreeNode *tree = NULL;
    
    parser.parseIt(session->get_id(), query, tree);
    int errcode = envs->pushDBsection();
	bool pushed = false;
	if (errcode != 0) 
	{
		//was already pushed..
		pushed = true;
	}

    errcode = this->executeRecQuery(tree);

    if (errcode != 0) return errcode;
    errcode = qres->pop(*res);
    if (errcode != 0) return errcode;
	if (!pushed)
	{
		errcode = envs->popDBsection();
		if (errcode != 0) return errcode;
	}
		
    return 0;
};

int QueryExecutor::execute_locally(string query, QueryResult **res, QueryParser &parser) {
	TreeNode *tree = NULL;

	int parsercode = parser.parseIt(session->get_id(), query, tree);
	if (parsercode != 0) {
		debug_print(*ec,  "exec. locally, parser code aint 0, error! , query: " + query + ". \n");
	}
	debug_print(*ec,  "IN EXEC_LOCALLY: will push dbsection); \n");
	int errcode = envs->pushDBsection();
	if (errcode != 0) {
		debug_print(*ec,  "!!!!! pushDB doesn't work\n");
		return errcode;
	}
	errcode = this->executeRecQuery(tree);
	if (errcode != 0) {
		debug_print(*ec,  "!!!!! executeRecQuery ended with error, so cant popDB section properly!!! so doing it here.\n");
		envs->popDBsection();
		return errcode;
	}
	errcode = envs->popDBsection();
	if (errcode != 0) { debug_print(*ec,  "Error on popDBsection, in exec. locally"); return errcode;}
	errcode = qres->pop(*res);
	if (errcode != 0) { debug_print(*ec,  "Error on pop from qres, in exec. locally"); return errcode;}

	return 0;
}

int QueryExecutor::executeRecQuery(TreeNode *tree) {

	int errcode;
	debug_print(*ec,  "[QE] executeRecQuery()");

	if (this->evalStopped()) {
		debug_print(*ec,  "[QE] query evaluating stopped by Server, aborting transaction ");
		errcode = tr->abort();
		inTransaction = false;
		if (errcode != 0) {
			debug_print(*ec,  "[QE] error in transaction->abort()");
			return errcode;
		}
		debug_print(*ec,  "[QE] Transaction aborted succesfully");
		debug_print(*ec,  (ErrQExecutor | EEvalStopped));
		return (ErrQExecutor | EEvalStopped);
	}
	StoreManager *sm = StoreManager::theStore;
	if (tree != NULL) {
		int nodeType = tree->type();
		debug_print(*ec,  "[QE] TreeType taken");
		debug_print(*ec,  tree->toString(0, true));
		switch (nodeType)
		{
		case TreeNode::TNNAME: {
			debug_print(*ec,  "[QE] Type: TNNAME");
			string name = tree->getName();
			
			if (!am->hasAccess(READ, name))
			{
				debug_printf(*ec, "[QE] no rights to access objects of name %s", name.c_str());	
				errcode = qres->push(new QueryBagResult());
				if (errcode) return errcode;
				return 0;
			}
			else
				debug_printf(*ec, "[QE] user has read rights to object %s", name.c_str());
			
			int sectionNumber = ((NameNode *) tree)->getBindSect();
			QueryResult *result = new QueryBagResult();
			errcode = envs->bindName(name, sectionNumber, tr, this, result);
			if (errcode != 0) return errcode;
			
			debug_printf(*ec, "[QE] TNNAME: checking object name\n");
			if (im->isObjectNameBound(name))
			{
				debug_printf(*ec, "[QE] object name bound in interface maps\n");
				bool found;
				string iName = im->getInterfaceNameForObject(name, found);
				if (found)
				{
					ImplementationInfo iI = im->getImplementationForInterface(iName, found, true);
					string impName = iI.getName();
					string impObjName = iI.getObjectName();
					if (found)
					{
						debug_printf(*ec, "[QE] name %s recognized as interface object name, bound through %s->%s to %s objects\n",
									   name.c_str(), iName.c_str(), impName.c_str(), impObjName.c_str());
						am->addAccess(impObjName, am->getAccess(name));
						errcode = envs->bindName(impObjName, sectionNumber, tr, this, result, iName);
						if (errcode) return errcode;
					}
				}
			}
			debug_printf(*ec, "[QE] TNNAME: pushing result..\n");

			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Done!");
			return 0;
		}//case TNNAME

		case TreeNode::TNPROC: {
			debug_print(*ec,  "[QE] Type: TNPROC");

			string name = ((ProcedureNode *) tree)->getName();
			QueryNode* tmpQN = ((ProcedureNode *) tree)->getCode();
			vector<string> params = ((ProcedureNode *) tree)->getParams();
			unsigned int paramsNumb = ((ProcedureNode *) tree)->getParamsNumb();

			string code = tmpQN->deParse();
			QueryResult *strct = new QueryStructResult();
			    
			QueryResult *code_str = new QueryStringResult(code);
			QueryResult *code_bind = new QueryBinderResult(QE_METHOD_PROCBODY_BIND_NAME, code_str);
			strct->addResult(code_bind);
			
			for (unsigned int i=0; i<paramsNumb; i++) {
				QueryResult *param_str = new QueryStringResult(params[i]);
				QueryResult *param_bind = new QueryBinderResult(QE_METHOD_PARAM_BIND_NAME, param_str);
				strct->addResult(param_bind);
			}
			QueryResult *final_binder = new QueryBinderResult(name, strct);

			ObjectPointer *optr;
			errcode = objectFromBinder(final_binder, optr);
			if (errcode != 0) return errcode;

			optr->getValue()->setSubtype(Store::Procedure);

			errcode = tr->modifyObject(optr, optr->getValue());
			if (errcode != 0) {
				debug_print(*ec,  "[QE] TNPROC - Error in modifyObject.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			QueryResult *lidres = new QueryReferenceResult(optr->getLogicalID());

			errcode = qres->push(lidres);
			if (errcode != 0) return errcode;

			return 0;
		}//case TNPROC

		case TreeNode::TNREGPROC: {
			debug_print(*ec,  "[QE] Type: TNREGPROC");
			QueryNode *query = ((RegisterProcNode *) tree)->getQuery();
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());

			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;

			if (execution_result->type() != QueryResult::QREFERENCE) {
				debug_print(*ec,  "[QE] TNREGPROC error - execution result is not QueryReference");
				debug_print(*ec,  (ErrQExecutor | ERefExpected));
				return (ErrQExecutor | ERefExpected);
			}

			ObjectPointer *optr;
			errcode = tr->getObjectPointer (((QueryReferenceResult*)execution_result)->getValue(), Store::Read, optr, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] register proc operation - Error in getObjectPointer.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			errcode = tr->addRoot(optr);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] Error in addRoot");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;

			return 0;
		}//case TNREGPROC

		case TreeNode::TNCALLPROC: {
			debug_print(*ec,  "[QE] Type: TNCALLPROC");

			vector<QueryNode*> queries = ((CallProcNode *) tree)->getQueries();
			unsigned int queries_size = ((CallProcNode *) tree)->howManyParts();
			string name = ((CallProcNode *) tree)->getName();

			vector<string> params;
			string code = "";

			int bindSectionNumber = -1;
			LogicalID* bindClassLid;
			errcode = envs->bindProcedureName(name, queries_size, tr, this, code, params, bindSectionNumber, bindClassLid);
			if (errcode != 0) return errcode;

			if ((code == "") || (bindSectionNumber == -1)) {
				qres->push(new QueryBagResult());
				return 0;
			}

			am->enableProcedureOnly(bindSectionNumber);
			QueryResult *new_envs_sect = new QueryBagResult();
			for (unsigned int i=0; i<queries_size; i++) {
				errcode = executeRecQuery(queries[i]);
				if(errcode != 0) {am->disableProcedureOnly(bindSectionNumber); return errcode;}
				QueryResult *execution_result;
				errcode = qres->pop(execution_result);
				if (errcode != 0) {am->disableProcedureOnly(bindSectionNumber); return errcode;}
				QueryResult *new_binder = new QueryBinderResult(params[i], execution_result);
				new_envs_sect->addResult(new_binder);
			}

			// callProcedure wklada na stos sekcje w takiej kolejnosci jak sa w podanym wektorze
			// envs_sections(0) bedzie najnizej, envs_sections(n) bedzie na wierzcholku stosu
			vector<QueryBagResult*> envs_sections;
			envs_sections.push_back((QueryBagResult *)new_envs_sect);

			int oldBindSectionPrior = envs->es_priors[bindSectionNumber];
			LogicalID* oldBindClassLid =  envs->actualBindClassLid;
			envs->es_priors[bindSectionNumber] = envs->actual_prior + 1;
			envs->actualBindClassLid = bindClassLid;

			errcode = callProcedure(code, envs_sections);

			am->disableProcedureOnly(bindSectionNumber);
			envs->actualBindClassLid = oldBindClassLid;
			envs->es_priors[bindSectionNumber] = oldBindSectionPrior;
			if(errcode != 0) return errcode;

			return 0;
		}//case TNCALLPROC

		case TreeNode::TNRETURN: {
			debug_print(*ec,  "[QE] Type: TNRETURN (begin)");
			QueryNode *query = ((ReturnNode *) tree)->getQuery();
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());
			debug_print(*ec,  "[QE] Type: TNRETURN (done)");
			return EEvalStopped;
		}//case TNRETURN

		case TreeNode::TNTHROWEXCEPTION: {
			debug_print(*ec,  "[QE] Type: TN THROW EXCEPTION (begin)");
			QueryNode *query = ((ThrowExceptionNode *) tree)->getQuery();
			string exception_name = "";
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;
			if (execution_result->type() == QueryResult::QSTRING) {
				exception_name = ((QueryStringResult *)execution_result)->getValue();
			}

			if (exception_name == "WrongParameterException") {
				debug_print(*ec,  "[QE] USER EXCEPTION THROWN (done)");
				return (ErrUserProgram | EUserWrongParam);
			}
			else {
				debug_print(*ec,  "[QE] UNKNOWN EXCEPTION THROWN (done)");
				return (ErrUserProgram | EUserUnknown);
			}

			return 0;

		}//case TNTHROWEXCEPTION

		case TreeNode::TNVIEW: {
			debug_print(*ec,  "[QE] Type: TNVIEW");

			string view_name = ((ViewNode *) tree)->getName();
			QueryNode* virtual_objects = ((ViewNode *) tree)->getVirtualObjects();
			vector<QueryNode*> procs = ((ViewNode *) tree)->getProcedures();
			unsigned int procs_num = procs.size();
			vector<QueryNode*> views = ((ViewNode *) tree)->getSubviews();
			unsigned int views_num = views.size();
			vector<QueryNode*> objects = ((ViewNode *) tree)->getObjects();
			unsigned int objects_num = objects.size();

			int on_upd_num = 0;
			int on_cre_num = 0;
			int on_del_num = 0;
			int on_ret_num = 0;
			int on_ins_num = 0;
			int on_nav_num = 0;
			int on_vir_num = 0;
			int on_sto_num = 0;

			ObjectPointer *optr;
			vector<LogicalID *> vec_lid;

			if(virtual_objects != NULL) {
				errcode = executeRecQuery(virtual_objects);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());
			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;
			if (execution_result->type() != QueryResult::QREFERENCE) {
				debug_print(*ec,  "[QE] TNVIEW error - execution result is not QueryReference");
				debug_print(*ec,  (ErrQExecutor | ERefExpected));
				return (ErrQExecutor | ERefExpected);
			}
			LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();
			errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] view creating operation - Error in getObjectPointer.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string virt_obj_name = optr->getName();
			if ((virt_obj_name == "on_update") || (virt_obj_name == "on_create")
				|| (virt_obj_name == "on_delete") || (virt_obj_name == "on_retrieve")
				|| (virt_obj_name == "on_navigate") || (virt_obj_name == "on_virtualize")
				|| (virt_obj_name == "on_store") || (virt_obj_name == "on_insert")) {
				debug_print(*ec,  "[QE] TNVIEW error - virtual objects shouldn't be named like \'on_\' procedures");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}

			QueryResult *tmp_bndr =new QueryBinderResult(QE_VIEWDEF_VIRTUALOBJECTS_NAME, new QueryStringResult(virt_obj_name));
			ObjectPointer *tmp_optr;
			errcode = objectFromBinder(tmp_bndr, tmp_optr);
			if (errcode != 0) return errcode;
			LogicalID *virtObjLid = tmp_optr->getLogicalID();
			vec_lid.push_back(virtObjLid);

			vec_lid.push_back(lid);

			for (unsigned int i = 0; i < procs_num; i++) {
				if(procs[i] != NULL) {
					errcode = executeRecQuery(procs[i]);
					if(errcode != 0) return errcode;
				}
				else qres->push(new QueryNothingResult());
				errcode = qres->pop(execution_result);
				if (errcode != 0) return errcode;
				if (execution_result->type() != QueryResult::QREFERENCE) {
					debug_print(*ec,  "[QE] TNVIEW error - execution result is not QueryReference");
					debug_print(*ec,  (ErrQExecutor | ERefExpected));
					return (ErrQExecutor | ERefExpected);
				}
				lid = ((QueryReferenceResult*)execution_result)->getValue();
				errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
				if (errcode != 0) {
					debug_print(*ec,  "[QE] view creating operation - Error in getObjectPointer.");
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				string optr_name = optr->getName();
				if (optr_name == "on_update") on_upd_num++;
				else if (optr_name == "on_create") on_cre_num++;
				else if (optr_name == "on_delete") on_del_num++;
				else if (optr_name == "on_retrieve") on_ret_num++;
				else if (optr_name == "on_insert") on_ins_num++;
				else if (optr_name == "on_navigate") on_nav_num++;
				else if (optr_name == "on_virtualize") on_vir_num++;
				else if (optr_name == "on_store") on_sto_num++;
				vec_lid.push_back(lid);
			}
			if ((on_upd_num > 1) || (on_cre_num > 1) || (on_del_num > 1) || (on_ret_num > 1)
				|| (on_ins_num > 1) || (on_nav_num > 1) || (on_vir_num > 1) || (on_sto_num > 1)) {
				debug_print(*ec,  "[QE] TNVIEW error - multiple \'on_\' procedure definition found");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}

			for (unsigned int i = 0; i < views_num; i++) {
				if(views[i] != NULL) {
					errcode = executeRecQuery(views[i]);
					if(errcode != 0) return errcode;
				}
				else qres->push(new QueryNothingResult());
				errcode = qres->pop(execution_result);
				if (errcode != 0) return errcode;
				if (execution_result->type() != QueryResult::QREFERENCE) {
					debug_print(*ec,  "[QE] TNVIEW error - execution result is not QueryReference");
					debug_print(*ec,  (ErrQExecutor | ERefExpected));
					return (ErrQExecutor | ERefExpected);
				}
				lid = ((QueryReferenceResult*)execution_result)->getValue();
				vec_lid.push_back(lid);
			}

			for (unsigned int i = 0; i < objects_num; i++) {
				if (objects[i] != NULL) {
					errcode = executeRecQuery (objects[i]);
					if (errcode != 0) return errcode;
					QueryResult *tmp_result;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					QueryResult *bagRes = new QueryBagResult();
					bagRes->addResult(tmp_result);
					for (unsigned int i = 0; i < bagRes->size(); i++) {
						QueryResult *binder;
						errcode = ((QueryBagResult *) bagRes)->at(i, binder);
						if (errcode != 0) return errcode;

						if ((binder->type()) != QueryResult::QBINDER) {
							return qeErrorOccur("[QE] objectFromBinder() expected a binder, got something else", EOtherResExp);
						}

						ObjectPointer *optr;
						errcode = this->objectFromBinder(binder, optr);
						if (errcode != 0) return errcode;
						vec_lid.push_back(optr->getLogicalID());
					}
				}
			}

			DataValue *dbValue = sm->createVectorValue(&vec_lid);
			DataValue* value = dbValue;

			ObjectPointer *newObject;
			if ((errcode = tr->createObject(view_name, value, newObject)) != 0) {
				debug_print(*ec,  "[QE] Error in createObject");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			newObject->getValue()->setSubtype(Store::View);
			errcode = tr->modifyObject(newObject, newObject->getValue());
			if (errcode != 0) {
				debug_print(*ec,  "[QE] TNVIEW - Error in modifyObject.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			QueryResult *lidres = new QueryReferenceResult(newObject->getLogicalID());

			errcode = qres->push(lidres);
			if (errcode != 0) return errcode;

			return 0;
		} // case: TNVIEW

		case TreeNode::TNREGVIEW: {
			debug_print(*ec,  "[QE] Type: TNREGVIEW");
			QueryNode *query = ((RegisterViewNode *) tree)->getQuery();
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());

			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;
			
			int errcode2;
			
			if (execution_result->type() != QueryResult::QREFERENCE) {
				debug_print(*ec,  "[QE] TNREGVIEW error - execution result is not QueryReference");
				debug_print(*ec,  (ErrQExecutor | ERefExpected));
				return (ErrQExecutor | ERefExpected);
			}

			ObjectPointer *optr;
			LogicalID *execution_result_lid = ((QueryReferenceResult*)execution_result)->getValue();
			errcode = tr->getObjectPointer (execution_result_lid, Store::Read, optr, false);
			if (errcode != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				debug_print(*ec,  "[QE] register view operation - Error in getObjectPointer.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			string virtual_objects_name = "";
			vector<LogicalID*>* inner_vec = (optr->getValue())->getVector();
			for (unsigned int i=0; i < inner_vec->size(); i++) {
				ObjectPointer *inner_optr;
				errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
				if (errcode != 0) {
					errcode2 = persistDelete(execution_result_lid);
					if(errcode2 != 0) return errcode2;
					debug_print(*ec,  "[QE] register view operation - Error in getObjectPointer.");
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				if (inner_optr->getName() == QE_VIEWDEF_VIRTUALOBJECTS_NAME) {
					virtual_objects_name = (inner_optr->getValue())->getString();
					break;
				}
			}
			
			vector<LogicalID*>* vec_sysvirt;
			if ((errcode = tr->getSystemViewsLID(virtual_objects_name, vec_sysvirt)) != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				debug_print(*ec,  "[QE] register view operation - error in getSystemViewsLID");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			int vecSize_sysvirt = vec_sysvirt->size();
			debug_printf(*ec, "[QE] %d SystemViews LID by name taken\n", vecSize_sysvirt);
			
			if (vecSize_sysvirt > 0) {
				debug_print(*ec,  "[QE] register view operation error: new virtual objects and a system view have the same name");
				
				errcode = persistDelete(execution_result_lid);
				if(errcode != 0) return errcode;
				
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			
			vector<LogicalID*>* vec_virt;
			if ((errcode = tr->getViewsLID(virtual_objects_name, vec_virt)) != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				debug_print(*ec,  "[QE] register view operation - error in getViewsLID");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			int vecSize_virt = vec_virt->size();
			debug_printf(*ec, "[QE] %d Views LID by name taken\n", vecSize_virt);
			
			if (vecSize_virt > 0) {
				debug_print(*ec,  "[QE] register view operation error: virtual objects name already taken");
				
				errcode = persistDelete(execution_result_lid);
				if(errcode != 0) return errcode;
				
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			
			vector<LogicalID*>* vec_roots;
			if ((errcode = tr->getRootsLID(virtual_objects_name, vec_roots)) != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				debug_print(*ec,  "[QE] register view operation - error in getRootsLID");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			unsigned int vecSize_roots = vec_roots->size();
			
			if (vecSize_roots > 0) {
				debug_printf(*ec, "[QE] %d Roots LID by name taken\n", vecSize_roots);
				debug_print(*ec,  "[QE] WARNING, root objects with the same name as new virtual objects found. These objects will be transformed into virtuals.");
				
				string proc_code = "";
				string proc_param = "";
				errcode = exViews->getOn_procedure(execution_result_lid, "on_create", proc_code, proc_param, tr);
				if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
				if (proc_code == "") {
					debug_print(*ec,  "[QE] operator <create> - this VirtualObject doesn't have this operation defined");
					
					errcode = persistDelete(execution_result_lid);
					if(errcode != 0) return errcode;
					
					debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
					return (ErrQExecutor | EOperNotDefined);
				}
				
				for (unsigned int i=0; i<vecSize_roots; i++) {
					LogicalID *curr_root = vec_roots->at(i);
					QueryResult *curr_root_before_deref = new QueryReferenceResult(curr_root);
					QueryResult *curr_root_after_deref;
					errcode = this->derefQuery(curr_root_before_deref, curr_root_after_deref);
					if (errcode != 0) {
						errcode2 = persistDelete(execution_result_lid);
						if(errcode2 != 0) return errcode2;
						return errcode;
					}
					QueryResult *param_binder = new QueryBinderResult(proc_param, curr_root_after_deref);
					
					vector<QueryBagResult*> envs_sections;
					errcode = exViews->createNewSections(NULL, (QueryBinderResult*) param_binder, execution_result_lid, envs_sections, tr, this);
					if (errcode != 0) {
						antyStarveFunction(errcode);
						inTransaction = false;
						errcode2 = persistDelete(execution_result_lid);
						if(errcode2 != 0) return errcode2;
						return errcode;
					}
					
					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) {
						errcode2 = persistDelete(execution_result_lid);
						if(errcode2 != 0) return errcode2;
						return errcode;
					}
					
					QueryResult *tmp_res;
					errcode = qres->pop(tmp_res);
					if (errcode != 0) {
						errcode2 = persistDelete(execution_result_lid);
						if(errcode2 != 0) return errcode2;
						return errcode;
					}
					
					errcode = persistDelete(curr_root);
					if(errcode != 0) return errcode;
				}
			}
			
			errcode = tr->addView(virtual_objects_name.c_str(), optr);
			if (errcode != 0) { 
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				debug_print(*ec,  "[QE] Error in addView");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = tr->addRoot(optr);
			if (errcode != 0) { 
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				debug_print(*ec,  "[QE] Error in addRoot");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;

			return 0;
		}//case TNREGVIEW
		
		case TreeNode::TNVIRTUALIZEAS: {
			debug_print(*ec,  "[QE] Type: TNVIRTUALIZEAS");
			QueryResult *final_result = new QueryBagResult();

			QueryResult *l_tmp;
			errcode = executeRecQuery (((VirtualizeAsNode *) tree)->getQuery());
			if (errcode != 0) return errcode;
			errcode = qres->pop(l_tmp);
			if (errcode != 0) return errcode;
			QueryResult *lResult;
			if (((l_tmp->type()) != QueryResult::QSEQUENCE) && ((l_tmp->type()) != QueryResult::QBAG)) {
				lResult = new QueryBagResult();
				lResult->addResult(l_tmp);
			}
			else lResult = l_tmp;

			string virtualizeAsName = (((VirtualizeAsNode *) tree)->getName());
			LogicalID* referenced_view_lid = NULL;
			string referenced_view_code;
			string referenced_view_param;
			QueryResult *subqueryResult = NULL;
			LogicalID *subquery_view_parent = NULL;

			if (((VirtualizeAsNode *) tree)->getSubQuery() == NULL) {
				vector<LogicalID*>* vec_virt;
				if ((errcode = tr->getViewsLID(virtualizeAsName, vec_virt)) != 0) {
					debug_print(*ec,  "[QE] error in getViewsLID");
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				int vecSize_virt = vec_virt->size();
				debug_printf(*ec, "[QE] %d Views LID by name taken\n", vecSize_virt);
				if (vecSize_virt != 1) {
					debug_print(*ec,  "[QE] bindName error, while searching for a view");
					debug_print(*ec,  (ErrQExecutor | EBadBindName));
					return (ErrQExecutor | EBadBindName);
				}

				referenced_view_lid = vec_virt->at(0);
				errcode = exViews->getOn_procedure(referenced_view_lid, "on_virtualize", referenced_view_code, referenced_view_param, tr);
				if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
				if (referenced_view_code == "") {
					debug_print(*ec,  "[QE] operator <virtualize as> - this VirtualObject doesn't have this operation defined");
					debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
					return (ErrQExecutor | EOperNotDefined);
				}
			}
			else {
				errcode = executeRecQuery (((VirtualizeAsNode *) tree)->getSubQuery());
				if (errcode != 0) return errcode;
				QueryResult *r_tmp;
				errcode = qres->pop(r_tmp);
				if ((r_tmp->type() == QueryResult::QBAG) && (r_tmp->size() == 1)) {
					QueryResult *tmp_subquery_res;
					errcode = (((QueryBagResult *) r_tmp)->at(0, tmp_subquery_res));
					if (errcode != 0) return errcode;
					if (tmp_subquery_res->type() == QueryResult::QVIRTUAL) {
						LogicalID* sub_lid = ((QueryVirtualResult *) tmp_subquery_res)->view_defs.at(0);
						errcode = exViews->getSubview(sub_lid, virtualizeAsName, referenced_view_lid, tr);
						if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
						if (referenced_view_lid != NULL) {
							errcode = exViews->getOn_procedure(referenced_view_lid, "on_virtualize", referenced_view_code, referenced_view_param, tr);
							if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
							if (referenced_view_code == "") {
								debug_print(*ec,  "[QE] operator <virtualize as> - this VirtualObject doesn't have this operation defined");
								debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
								return (ErrQExecutor | EOperNotDefined);
							}
							subqueryResult = tmp_subquery_res;
						}
						else {
							debug_print(*ec,  "[QE] <virtualize as> - error evaluating subquery");
							debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
							return (ErrQExecutor | EQEUnexpectedErr);
						}
					}
					else if (tmp_subquery_res->type() == QueryResult::QREFERENCE) {
						LogicalID *maybe_parent = ((QueryReferenceResult *) tmp_subquery_res)->getValue();

						ObjectPointer *optrOut;
						errcode = tr->getObjectPointer (maybe_parent, Store::Read, optrOut, false);
						if (errcode != 0) {
							debug_print(*ec,  "[QE] <virtualize as> - Error in getObjectPointer.");
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}

						//TODO powinna byc mozliwosc zapisania w objectPointerze ze zawiera jako podobiekty perspektywy
						//ObjectPointer powinien miec funkcje nazwa->LID perspektywy definiujacej obiekty o takiej nazwie
						//cos na ksztalt getViews(name), wtedy ponizszy kawalek kodu moznaby uspawnic znacznie
						DataValue* db_value = optrOut->getValue();
						int vType = db_value->getType();
						if (vType != Store::Vector) {
							debug_print(*ec,  "[QE] <virtualize as> - ERROR! looking for subview in referenced object, object is not vector type");
							debug_print(*ec,  (ErrQExecutor | EOtherResExp));
							return (ErrQExecutor | EOtherResExp);
						}
						vector<LogicalID*> *insVector = db_value->getVector();
						if (insVector == NULL) {
							debug_print(*ec,  "[QE] <virtualize as> - insVector == NULL");
							debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
							return (ErrQExecutor | EQEUnexpectedErr);
						}

						for (unsigned int j = 0; j < insVector->size(); j++) {
							LogicalID *maybe_subview_logID = insVector->at(j);
							ObjectPointer *maybe_subview_optr;
							if ((errcode = tr->getObjectPointer(maybe_subview_logID, Store::Read, maybe_subview_optr, false)) != 0) {
								debug_print(*ec,  "[QE] <virtualize as> - Error in getObjectPointer");
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							DataValue* maybe_subview_value = maybe_subview_optr->getValue();
							int maybe_subview_vType = maybe_subview_value->getType();
							ExtendedType maybe_subview_extType = maybe_subview_value->getSubtype();
							if ((maybe_subview_vType == Store::Vector) && (maybe_subview_extType == Store::View)) {
								string maybe_subview_name = "";
								string maybe_subview_code = "";
								errcode = exViews->checkViewAndGetVirtuals(maybe_subview_logID, maybe_subview_name, maybe_subview_code, tr);
								if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
								if (maybe_subview_name == virtualizeAsName) {
									if (referenced_view_lid != NULL){
										debug_print(*ec,  "[QE] ERROR! More then one subview defining virtual objects with same name");
										debug_print(*ec,  (ErrQExecutor | EBadViewDef));
										return (ErrQExecutor | EBadViewDef);
									}
									else referenced_view_lid = maybe_subview_logID;
								}
							}
						}

						if (referenced_view_lid == NULL) {
							debug_print(*ec,  "[QE] operator <virtualize as> - this object doesn't have this operation defined, subview missing");
							debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
							return (ErrQExecutor | EOperNotDefined);
						}
						else {
							errcode = exViews->getOn_procedure(referenced_view_lid, "on_virtualize", referenced_view_code, referenced_view_param, tr);
							if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
							if (referenced_view_code == "") {
								debug_print(*ec,  "[QE] operator <virtualize as> - this VirtualObject doesn't have this operation defined");
								debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
								return (ErrQExecutor | EOperNotDefined);
							}
							subquery_view_parent = maybe_parent;
						}
					}
					else {
						debug_print(*ec,  "[QE] <virtualize as> - subquery should evaluate to single virtual result or single reference");
						debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
						return (ErrQExecutor | EQEUnexpectedErr);
					}
				}
				else {
					debug_print(*ec,  "[QE] operator <virtualize as> - subquery should evaluate to one element bag");
					debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
					return (ErrQExecutor | EQEUnexpectedErr);
				}
			}

			if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {
				debug_print(*ec,  "[QE] For each row of this score, the right argument will be computed");
				for (unsigned int i = 0; i < (lResult->size()); i++) {
					QueryResult *currentResult;
					if ((lResult->type()) == QueryResult::QSEQUENCE)
						errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
					else
						errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
					if (errcode != 0) return errcode;

					vector<QueryBagResult*> envs_sections;

					QueryResult *param_binder = new QueryBinderResult(referenced_view_param, currentResult);
					errcode = exViews->createNewSections((QueryVirtualResult*) subqueryResult, (QueryBinderResult*) param_binder, referenced_view_lid, envs_sections, tr, this);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}		

					errcode = callProcedure(referenced_view_code, envs_sections);
					if (errcode != 0) return errcode;
					QueryResult *callproc_res;
					errcode = qres->pop(callproc_res);
					if (errcode != 0) return errcode;

					QueryResult *bagged_callproc_res = new QueryBagResult();
					((QueryBagResult *) bagged_callproc_res)->addResult(callproc_res);
					for (unsigned int ii = 0; ii < bagged_callproc_res->size(); ii++) {
						QueryResult *seed;
						errcode = ((QueryBagResult *) bagged_callproc_res)->at(ii, seed);
						if (errcode != 0) return errcode;
						vector<QueryResult *> seeds;
						seeds.push_back(seed);
						vector<LogicalID *> view_defs;
						view_defs.push_back(referenced_view_lid);

						if (subqueryResult != NULL) {
							for (unsigned int k = 0; k < (((QueryVirtualResult *) subqueryResult)->view_defs.size()); k++ ) {
								view_defs.push_back(((QueryVirtualResult *) subqueryResult)->view_defs.at(k));
							}
							for (unsigned int k = 0; k < (((QueryVirtualResult *) subqueryResult)->seeds.size()); k++ ) {
								seeds.push_back(((QueryVirtualResult *) subqueryResult)->seeds.at(k));
							}
							subquery_view_parent = ((QueryVirtualResult *) subqueryResult)->view_parent;
						}

						QueryResult *virt_res = new QueryVirtualResult(virtualizeAsName, view_defs, seeds, subquery_view_parent);
						final_result->addResult(virt_res);
					}
				}
			}
			else {
				debug_print(*ec,  "[QE] ERROR! Virtualize As operation, bad left argument");
				debug_print(*ec,  (ErrQExecutor | EOtherResExp));
				return (ErrQExecutor | EOtherResExp);
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Virtualize As operation Done!");
			return 0;
		}//case TNVIRTUALIZEAS

		case TreeNode::TNINTERFACEBIND: {
		    debug_print(*ec,  "[QE] Type: TNINTERFACEBIND");
		    string leftName = ((InterfaceBind *)tree)->getInterfaceName();
		    string rightName = ((InterfaceBind *)tree)->getImplementationName();
		
			bool matches;
			errcode = im->bindInterface(leftName, rightName, tr, matches);
			if (errcode) return errorOccur("[QE] TNINTERFACEBIND error in bindInterface", errcode);
			string verbose = matches ? "bound" : "cannot bind: mismatch";
			
		    QueryResult *result = new QueryStringResult(verbose);
		    errcode = qres->push(result);
		    if (errcode != 0) return errcode;
		    debug_print(*ec,  "[QE] TNINTERFACEBIND done");
		    return 0;
		}

		case TreeNode::TNREGINTERFACE: {
		    debug_print(*ec,  "[QE] Type: TNREGINTERFACE");
			RegisterInterfaceNode* ri = (RegisterInterfaceNode *)tree;
			CreateType ct = ri->getCreateType();
			InterfaceNode query = ri->getInterfaceQuery();
			errcode = executeRecQuery(&query);
			if (errcode) return errcode;

		    debug_printf(*ec, "[QE] TNREGINTERFACE inner result pushed\n");
		    QueryResult *execution_result;
		    errcode = qres->pop(execution_result);
		    if (errcode) return errcode;
		    if (execution_result->type() != QueryResult::QREFERENCE) 
				return qeErrorOccur( "[QE] TNREGINTERFACE error - execution result is not QueryReference", ERefExpected );
		    debug_printf(*ec, "[QE] TNREGINTERFACE got reference\n");
			LogicalID* interfaceLid = ((QueryReferenceResult*)execution_result)->getValue();

			bool checkValidity = true;
			errcode = im->addInterface(interfaceLid, tr, ct, checkValidity);
			if (errcode) return errorOccur("[QE] TNREGINTERFACE error in addInterface", errcode);
			
		    errcode = qres->push(execution_result);
		    if (errcode) return errcode;
		    debug_print(*ec,  "[QE] TNREGINTERFACE returning");
		    return 0;
		}

		case TreeNode::TNINTERFACEMETHOD: {
		    debug_print(*ec,  "[QE] Type: TNINTERFACEMETHOD");
		    string name = ((InterfaceMethod *) tree)->getName();
		    vector<LogicalID *> methodLids;

		    debug_printf(*ec, "[QE] Type: TNINTERFACEMETHOD name=%s\n", name.c_str());

		    QueryResult *methodStruct = new QueryStructResult();
		    QueryResult *paramName, *paramNameBinder;
		    
		    TAttributes params = ((InterfaceMethod *) tree)->getParams();
		    debug_printf(*ec, "[QE] TNINTERFACEMETHOD parameter count = %d\n", params.size());
		    for(TAttributes::iterator it = params.begin(); it != params.end(); ++it) 
		    {
			paramName = new QueryStringResult(it->getName());
			paramNameBinder = new QueryBinderResult(QE_METHOD_PARAM_BIND_NAME, paramName);
			methodStruct->addResult(paramNameBinder);
			debug_printf(*ec, "[QE] TNINTERFACEMETHOD parameter (%s) added\n", it->getName().c_str());
		    }

		    QueryResult *methodBinder = new QueryBinderResult(name, methodStruct);
		    ObjectPointer *optr;

		    debug_printf(*ec, "[QE] TNINTERFACEMETHOD making object from binder\n");

		    if ((errcode = objectFromBinder(methodBinder, optr))!=0)
			return errcode;

		    debug_printf(*ec, "[QE] TNINTERFACEMETHOD object made\n");

		    QueryResult *lidres = new QueryReferenceResult(optr->getLogicalID());
		    if ((errcode = qres->push(lidres))!=0)
			return errcode;

		    debug_print(*ec,  "[QE] TNINTERFACEMETHOD proccessing complete");
		    return 0;
		}

		case TreeNode::TNINTERFACEATTRIBUTE: {
		    string name = ((InterfaceAttribute *) tree)->getName();
		    debug_printf(*ec, "[QE] Type: TNINTERFACEATTRIBUTE Name = %s\n", name.c_str());
		    		    
		    QueryResult *nameString = new QueryStringResult(name);
		    QueryResult *nameBinder = new QueryBinderResult(QE_NAME_BIND_NAME, nameString);

		    ObjectPointer *optr;
		    if ((errcode = objectFromBinder(nameBinder, optr))!=0)
			return errcode;

		    QueryResult *lidres = new QueryReferenceResult(optr->getLogicalID());
		    if ((errcode = qres->push(lidres))!=0)
			return errcode;

		    debug_print(*ec,  "[QE] TNINTERFACEATTRIBUTE proccessing complete");
		    return 0;
		}

		case TreeNode::TNINTERFACENODE: {
			debug_print(*ec,  "[QE] Type: TNINTERFACENODE");
			
			InterfaceNode *node = (InterfaceNode *)tree;			
			string interfaceName = node->getInterfaceName();
			string objectName = node->getObjectName();
			debug_print(*ec,  "[QE] Interface name: " + interfaceName);

			// vector that holds all the logical ids
			vector<LogicalID *> interfaceLids;
			pushStringToLIDs(objectName, QE_OBJECT_NAME_BIND_NAME, interfaceLids);

			// get and process supers
			QParser::TSupers supers = node->getSupers();
			debug_printf(*ec, "[QE] TNINTERFACENODE: supers count: %d\n", supers.size());
			QParser::TSupers::iterator it;
			for (it = supers.begin(); it != supers.end(); ++it)
			{
			    string super = *it;
			    pushStringToLIDs(super, QE_EXTEND_BIND_NAME, interfaceLids);
			}
			
			TAttributes attributes = node->getAttributes();
			debug_printf(*ec, "[QE] TNINTERFACENODE: attributes count: %d\n", attributes.size());
			for(TAttributes::iterator it = attributes.begin(); it != attributes.end(); it++) 
			{
			    InterfaceAttribute *a = new InterfaceAttribute(*it);
			    debug_printf(*ec, "[QE] TNINTERFACENODE: adding attribute\n");
			    if ((errcode = executeRecQuery(a)) != 0)
			        return errcode;
			    QueryResult *execution_result;
			    if ((errcode = qres->pop(execution_result))!=0)
			        return errcode;
			    if (execution_result->type() != QueryResult::QREFERENCE)
			        return qeErrorOccur("[QE] TNINTERFACENODE error - execution result is not QueryReference (attributes)", ERefExpected);
			    LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();
			    ObjectPointer *optr;
			    if ((errcode = tr->getObjectPointer (lid, Store::Read, optr, false))!=0)
			    	return trErrorOccur("[QE] TNINTERFACENODE - Error in getObjectPointer (attributes).", errcode);
			    string optr_name = optr->getName();
			    LogicalID* newLid;
			    if ((errcode = lidFromReference(QE_FIELD_BIND_NAME, lid, newLid))!=0)
			        return errcode;
			    debug_printf(*ec, "[QE] TNINTERFACENODE: attribute added, lid = %d\n", newLid);
			    interfaceLids.push_back(newLid);
			}

			QParser::TMethods methods = node->getMethods();
			
			for(QParser::TMethods::iterator it = methods.begin(); it != methods.end(); it++) 
			{
			    InterfaceMethod *m = new InterfaceMethod(*it);
			    if ((errcode = executeRecQuery(m)) != 0)
			        return errcode;
			    QueryResult *execution_result;
			    if ((errcode = qres->pop(execution_result))!=0)
			        return errcode;
			    if (execution_result->type() != QueryResult::QREFERENCE)
			        return qeErrorOccur("[QE] TNINTERFACENODE error - execution result is not QueryReference (methods)", ERefExpected);
			    LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();
			    ObjectPointer *optr;
			    if ((errcode = tr->getObjectPointer (lid, Store::Read, optr, false))!=0)
			    	return trErrorOccur("[QE] TNINTERFACENODE - Error in getObjectPointer (methods).", errcode);
			    string optr_name = optr->getName();
			    LogicalID* newLid;
			    if ((errcode = lidFromReference(QE_METHOD_BIND_NAME, lid, newLid))!=0)
			        return errcode;
			    interfaceLids.push_back(newLid);
			}

			// create result
			DataValue *dbValue = sm->createVectorValue(&interfaceLids);
			LogicalID* newLid;
			if ((errcode = createObjectAndPutOnQRes(dbValue, interfaceName, Store::Interface, newLid))!=0)
			    return errcode;

			debug_print(*ec,  "[QE] TNINTERFACENODE processing complete");
			return 0;
		}
				
		case TreeNode::TNSCHEMANODE:
		{
			debug_print(*ec,  "[QE] Type: TNSCHEMANODE");
			SchemaNode *sn = (SchemaNode *)tree;
			string schemaName = sn->getName();
			bool fullyDefined = sn->getIsFullyDefined();
			
			TNameToAccess accessPoints;
			if (!fullyDefined) 
				accessPoints = sn->getAccessPoints();
			else
			{
				TInterfacesWithCrudMap icmap = sn->getInterfaces();
				TInterfacesWithCrudMap::iterator it;
				for (it = icmap.begin(); it != icmap.end(); ++it)
				{
					string apname = it->first;
					TInterfaceWithCrud ic =  it->second;
					InterfaceNode n = ic.first;
					int crud = ic.second;
					
					RegisterInterfaceNode *rn = new RegisterInterfaceNode(&n, CT_CREATE_OR_UPDATE);
					errcode = executeRecQuery(rn);
					if (errcode)
						return errcode;
					QueryResult *execution_result;
					if ((errcode = qres->pop(execution_result))!=0)
			    		return errcode;
					
					debug_printf(*ec, "[QE] Schema: adding access point");
					accessPoints[apname] = crud; 
				}							
			}
			
			//process aps
			vector<LogicalID *> schemaLids;
			info_printf(*ec, "[QE] TNSchema: schema by name %s has access points count: %d\n", schemaName.c_str(), accessPoints.size());
			Schemas::TNameToAccess::iterator it;
			for (it = accessPoints.begin(); it != accessPoints.end(); ++it)
			{
			    string apName = (*it).first;
			    int crud = (*it).second;
				pushStringToLIDs(apName, QE_FIELD_BIND_NAME, schemaLids);
				LogicalID* tmpLid;
				lidFromInt(QE_CRUD_BIND_NAME, crud, tmpLid);
				schemaLids.push_back(tmpLid);
			}

			//create DBResult and then, an object from it		
			DataValue *dbValue = sm->createVectorValue(&schemaLids);
			LogicalID* newLid;
			if ((errcode = createObjectAndPutOnQRes(dbValue, schemaName, Store::StoreSchema, newLid))!=0)
			    return errcode;

			debug_print(*ec,  "[QE] TNSCHEMANODE processing complete");
			return 0;		
		}
				
		case TreeNode::TNREGSCHEMA:
		{
			debug_print(*ec,  "[QE] Type: TNREGSCHEMA");
			RegisterSchemaNode* rs = (RegisterSchemaNode *)tree;
			SchemaNode query = rs->getSchemaQuery();
			CreateType ct = rs->getCreateType();
			errcode = executeRecQuery(&query);
			if (errcode) return errcode;
			
		    debug_printf(*ec, "[QE] TNREGSCHEMA inner result pushed\n");
		    QueryResult *execution_result;
		    errcode = qres->pop(execution_result);
		    if (errcode) return errcode;
		    if (execution_result->type() != QueryResult::QREFERENCE) 
				return qeErrorOccur( "[QE] TNREGSCHEMA error - execution result is not QueryReference", ERefExpected );

		    debug_printf(*ec, "[QE] TNREGSCHEMA got reference\n");
			LogicalID* schemaLid = ((QueryReferenceResult*)execution_result)->getValue();
		    errcode = os->addSchema(schemaLid, tr, ct);
			if (errcode)
				return errorOccur("[QE] TNREGSCHEMA error in addSchema", errcode);

		    errcode = qres->push(execution_result);
		    if (errcode) return errcode;
		    debug_print(*ec,  "[QE] TNREGSCHEMA returning");
		    return 0;
		}
				
		case TreeNode::TNSCHEMAEXPIMP:
		{
			debug_print(*ec, "[QE] TNSCHEMAEXPIMP starts");
			const string ext = ".sch";
			SchemaExpImpNode* schemaExpImpNode = (SchemaExpImpNode *)tree;
			string schemaName = schemaExpImpNode->getSchemaName();
			bool isExport = schemaExpImpNode->getIsExport();
			string strres; 
			if (isExport)
			{
				errcode = os->exportSchema(schemaName, tr);
				if (errcode) return errorOccur("[QE] TNSCHEMAEXPIMP error in exportSchema", errcode);
				strres = "exported";
			}
			else
			{
				string schemaString;
				errcode = os->importSchema(schemaName, schemaString);
				if (errcode) 
					return errorOccur("[QE] TNSCHEMAEXPIMP - schema import error", errcode);
				else
				{
					int id = session->get_id();
					QueryParser parser(id);
					TreeNode *tree = NULL;  
					int errcode = parser.parseIt(id, schemaString, tree);
					debug_printf(*ec, "[QE] TNSCHEMAEXPIMP parser on %s returned %d\n", schemaString.c_str(), errcode); 
					errcode = executeRecQuery(tree);
					if (errcode) return errcode;
					QueryResult *result;
					errcode = qres->pop(result);
					if (errcode) return errcode;
					debug_printf(*ec, "[QE] TNSCHEMAEXPIMP executor returned %d\n", errcode); 
					if (errcode)
					{
						debug_printf(*ec, "[QE] TNSCHEMAEXPIMP executor returned error: %d\n", errcode); 
						return errcode;
					}
				}
				strres = "imported";
			}
			
			QueryResult *result = new QueryStringResult(strres);
		    errcode = qres->push(result);
		    if (errcode != 0) return errcode;
		
			debug_print(*ec, "[QE] TNSCHEMAEXPIMP returns");
			return 0;	
		}

		case TreeNode::TNEXCLUDES:
		case TreeNode::TNINCLUDES: {
			if(nodeType == TreeNode::TNINCLUDES) {
				debug_print(*ec,  "[QE] Type: TNINCLUDES");
			} else {
				debug_print(*ec,  "[QE] Type: TNEXCLUDES");
			}
			QueryNode *objectsQuery = ((ClassesObjectsNode *) tree)->getObjectsQuery();
			if(objectsQuery != NULL) {
				errcode = executeRecQuery(objectsQuery);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());

			QueryResult *objectsResult;
			errcode = qres->pop(objectsResult);
			if (errcode != 0) return errcode;

			if (objectsResult->type() != QueryResult::QBAG) {
				return qeErrorOccur("[QE] Bag of Reference expected.", EBagOfRefExpected);
			}
			QueryBagResult *bagOfObjects = (QueryBagResult *)objectsResult;

			QueryNode *classQuery = ((ClassesObjectsNode *) tree)->getClassesQuery();
			SetOfLids classMarks;
			if(classQuery != NULL) {
				errcode = executeRecQuery(classQuery);
				if(errcode != 0) return errcode;
				QueryResult *classResult;
				errcode = qres->pop(classResult);
				if (errcode != 0) return errcode;
				if(classResult->type() != QueryResult::QBAG) {
					return qeErrorOccur("[QE] Bag of Reference expected.", EBagOfRefExpected);
				}

				for(unsigned int i = 0; i < ((QueryBagResult*)classResult)->size(); ++i) {
					QueryResult *oneClassResult;
					errcode = ((QueryBagResult*)classResult)->at(i, oneClassResult);
					if(errcode != 0) return errcode;
					if(oneClassResult->type() != QueryResult::QREFERENCE) {
						return qeErrorOccur("[QE] Reference expected.", ERefExpected);
					}
					if(!cg->vertexExist(((QueryReferenceResult*)oneClassResult)->getValue())) {
						return 1;//TODO (sk) error NoClassDefFound
					}
					classMarks.insert( ((QueryReferenceResult*)oneClassResult)->getValue() );
				}
			}

			for(unsigned int i = 0; i < bagOfObjects->size(); i++) {
				QueryResult *oneObjectResult;
				errcode = bagOfObjects->at(i, oneObjectResult);
				if(errcode != 0) return errcode;
				if(oneObjectResult->type() != QueryResult::QREFERENCE) {
					return qeErrorOccur("[QE] Reference expected.", ERefExpected);
				}
				ObjectPointer* optr;
				errcode = tr->getObjectPointer( ((QueryReferenceResult*)oneObjectResult)->getValue(), Store::Read, optr, false);
				if(errcode != 0) {
					return trErrorOccur("[QE] Can't get object to set classMark.", errcode);
				}
				if(optr->getValue()->getSubtype() == Store::Class) {
					//TODO (sk) Inproper class usage.
					return 1;
				}
				DataValue* newDV = optr->getValue()->clone();
				if(nodeType == TreeNode::TNINCLUDES) {
					newDV->addClassMarks(&classMarks);
				} else {
					newDV->removeClassMarks(&classMarks);
				}
				errcode = tr->modifyObject(optr, newDV);
				if(errcode != 0) {
					return trErrorOccur("[QE] Can't modify object and set classMark.", errcode);
				}
			}
			qres->push(objectsResult);//assums that lids are same after tr->modifyObject()
			return 0;
		}


		case TreeNode::TNINSTANCEOF: {
			debug_print(*ec,  "[QE] Type: TNINSTANCEOF");
			bool isInstanceOf = true;
			if(((ObjectsClassesNode*)tree)->getObjectsQuery() != NULL && ((ObjectsClassesNode*)tree)->getClassesQuery() != NULL) {
				//getting object
				errcode = executeRecQuery(((ObjectsClassesNode*)tree)->getObjectsQuery());
				if(errcode != 0) return errcode;
				QueryResult *exec_result_object;//czy to nalezy usunac?
				errcode = qres->pop(exec_result_object);
				if (errcode != 0) return errcode;
				//getting classes
				errcode = executeRecQuery(((ObjectsClassesNode*)tree)->getClassesQuery());
				if(errcode != 0) return errcode;
				QueryResult *exec_result_classes;//czy to nalezy usunac?
				errcode = qres->pop(exec_result_classes);
				if (errcode != 0) return errcode;

				if(exec_result_classes->type() != QueryResult::QBAG) {
					return 1;//TODO (sk) BagExpected
				}
				if(exec_result_object->type() != QueryResult::QBAG) {
					return 1;//TODO (sk) BagExpected
				}
				QueryBagResult* classesBag = dynamic_cast<QueryBagResult*>(exec_result_classes);
				QueryBagResult* objectsBag = dynamic_cast<QueryBagResult*>(exec_result_object);

				for(unsigned int i = 0; i < classesBag->size(); ++i) {
					QueryResult* classResult;
					errcode = classesBag->at(i, classResult);
					if(errcode != 0){
						return errcode;
					}
					if(classResult->type() != QueryResult::QREFERENCE) {
						return 1;//TODO (sk) ReferenceExpected
					}
					LogicalID* classLid = ((QueryReferenceResult*)classResult)->getValue();
					if(!cg->vertexExist(classLid)) {
						return 1;//TODO (sk) NoClassDefFound
					}
					for(unsigned int j = 0; j < objectsBag->size(); ++j) {
						QueryResult* objectResult;
						errcode = objectsBag->at(j, objectResult);
						if(errcode != 0){
							return errcode;
						}
						if(objectResult->type() != QueryResult::QREFERENCE) {
							return 1;//TODO (sk) ReferenceExpected
						}
						ObjectPointer *optr;
						errcode = tr->getObjectPointer (((QueryReferenceResult*)objectResult)->getValue(), Store::Read, optr, false);
						if (errcode != 0) {
							return trErrorOccur("[QE] TNINSTANCEOF- Error in getObjectPointer.", errcode);
						}
						errcode = cg->isCastAllowed(classLid, optr, isInstanceOf);
						if(errcode != 0) {
							return errcode;
						}
						if(!isInstanceOf){
							break;
						}
					}
					if(!isInstanceOf){
						break;
					}
				}
			}
			errcode = qres->push(new QueryBoolResult(isInstanceOf));
			if(errcode != 0) return errcode;
			debug_print(*ec,  "[QE] TNINSTANCEOF processing complete");
			return 0;
		}

		case TreeNode::TNCAST: {
			debug_print(*ec,  "[QE] Type: TNCAST");
			QueryResult *execResultObject;
			errcode = execRecursivQueryAndPop(((ObjectsClassesNode*)tree)->getObjectsQuery(), execResultObject);
			if(errcode != 0) return errcode;
			QueryResult *execResultClasses;
			errcode = execRecursivQueryAndPop(((ObjectsClassesNode*)tree)->getClassesQuery(), execResultClasses);

			if(execResultClasses->type() != QueryResult::QBAG) {
				return 1;//TODO (sk) BagExpected
			}
			if(execResultObject->type() != QueryResult::QBAG) {
				return 1;//TODO (sk) BagExpected
			}
			QueryBagResult* classesBag = dynamic_cast<QueryBagResult*>(execResultClasses);
			QueryBagResult* objectsBag = dynamic_cast<QueryBagResult*>(execResultObject);
			QueryResult* finalResult;

			if(classesBag->isEmpty()) {
				finalResult = execResultObject;
			} else {
				finalResult = new QueryBagResult();
				for(unsigned int i = 0; i < objectsBag->size(); ++i) {
					QueryResult* objectResult;
					errcode = objectsBag->at(i, objectResult);
					if(errcode != 0){
						//delete finalResult;
						return errcode;
					}
					if(objectResult->type() == QueryResult::QREFERENCE) {
						ObjectPointer *optr;
						errcode = tr->getObjectPointer (((QueryReferenceResult*)objectResult)->getValue(), Store::Read, optr, false);
						if (errcode != 0) {
							//delete finalResult;
							return trErrorOccur("[QE] TNINSTANCEOF- Error in getObjectPointer.", errcode);
						}
						bool isInstanceOf = true;
						for(unsigned int j = 0; j < classesBag->size(); ++j) {
							QueryResult* classResult;
							errcode = classesBag->at(j, classResult);
							if(errcode != 0){
								//delete finalResult;
								return errcode;
							}
							if(classResult->type() != QueryResult::QREFERENCE) {
								//delete finalResult;
								return 1;//TODO (sk) ReferenceExpected
							}
							LogicalID* classLid = ((QueryReferenceResult*)classResult)->getValue();
							if(!cg->vertexExist(classLid)) {
								//delete finalResult;
								return 1;//TODO (sk) NoClassDefFound
							}
							errcode = cg->isCastAllowed(classLid, optr, isInstanceOf);
							if(errcode != 0) {
								//delete finalResult;
								return errcode;
							}
							if(!isInstanceOf){
								break;
							}
						}
						if(isInstanceOf){
							finalResult->addResult(objectResult);
						}
					}
				}
			}
			errcode = qres->push(finalResult);
			if(errcode != 0) return errcode;
			debug_print(*ec,  "[QE] TNCAST processing complete");
			return 0;
		}

		case TreeNode::TNCLASS: {
			debug_print(*ec,  "[QE] Type: TNCLASS");
			vector<LogicalID *> classVector;//main class object
			string className = ((ClassNode *) tree)->getName();
			string invariantName = ((ClassNode *) tree)->getInvariant();
			NameListNode* fields = ((ClassNode *) tree)->getFields();
			NameListNode* staticFields = ((ClassNode *) tree)->getStaticFields();
			NameListNode* extendedClasses = ((ClassNode *) tree)->getExtends();
			CreateType ct = ((ClassNode *) tree)->getCreateType();

			int errcode;

			//class name
			bool classExist;
			errcode = cg->classExists(className, tr, classExist);
			if(errcode != 0) {
				return trErrorOccur("[QE] Error in classExists", errcode);
			}
			if(classExist && ct == CT_CREATE) {
				return qeErrorOccur("[QE] Class: \"" + className + "\" already exist.", ENotUniqueClassName);
			} else if(!classExist && ct == CT_UPDATE) {
				//TODO (sk) Class to update not found.
				return qeErrorOccur("[QE] Class to update: \"" + className + "\" not found.", ENotUniqueClassName);
			}

			if(classExist) {
				//Static members must be removed to allow static methods update.
				errcode = persistStaticMembersDelete(className);
				if(errcode != 0) return errcode;
			}

			//invariant name
			if(!invariantName.empty()) {
				pushStringToLIDs(invariantName, QE_INVARIANT_BIND_NAME, classVector);
			}

			//extended classes
			vector<LogicalID *> extendedclassesLIDs;
			if(extendedClasses != NULL) {
				set<string> classesNamesRef;
				set<string>* classesNames = &classesNamesRef;
				errcode = extendedClasses->namesFromUniqueList(classesNames);
				if (errcode != 0) {
					return otherErrorOccur("[QE] Not unique extended class name.", errcode);
				}
				errcode = cg->classesLIDsFromNames(classesNames, tr, extendedclassesLIDs);
				if(errcode != 0) {
					return trErrorOccur("[QE] Error in getClassesLID", errcode);
				}
				pushStringsToLIDs(classesNames, QE_EXTEND_BIND_NAME, classVector);
			}

			//adding fields
			if(fields != NULL) {
				set<string> fieldsNamesRef;
				set<string>* fieldsNames = &fieldsNamesRef;
				errcode = fields->namesFromUniqueList(fieldsNames);
				if (errcode != 0) {
					return otherErrorOccur("[QE] Not unique field name.", errcode);
				}
				pushStringsToLIDs(fieldsNames, QE_FIELD_BIND_NAME, classVector);
			}

			//adding static fields
			if(staticFields != NULL) {
				set<string> fieldsNamesRef;
				set<string>* fieldsNames = &fieldsNamesRef;
				errcode = staticFields->namesFromUniqueList(fieldsNames);
				if (errcode != 0) {
					return otherErrorOccur("[QE] Not unique field name.", errcode);
				}
				pushStringsToLIDs(fieldsNames, QE_STATIC_FIELD_BIND_NAME, classVector);
			}

			//adding methods
			vector<QueryNode*> procs = ((ClassNode *) tree)->getProcedures();
			for(vector<QueryNode*>::iterator i = procs.begin(); i != procs.end(); i++) {
				errcode = executeRecQuery(*i);
				if(errcode != 0) return errcode;
				QueryResult *execution_result;
				errcode = qres->pop(execution_result);
				if (errcode != 0) return errcode;
				if (execution_result->type() != QueryResult::QREFERENCE) {
					return qeErrorOccur("[QE] TNCLASS error - execution result is not QueryReference", ERefExpected);
				}
				LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();
				ObjectPointer *optr;
				errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
				if (errcode != 0) {
					return trErrorOccur("[QE] class creating operation - Error in getObjectPointer.", errcode);
				}
				string optr_name = optr->getName();

				LogicalID* newLid;
				errcode = lidFromReference(QE_METHOD_BIND_NAME, lid, newLid);
				if(errcode != 0) return errcode;
				classVector.push_back(newLid);
			}

			//adding static methods
			vector<QueryNode*> staticProcs = ((ClassNode *) tree)->getStaticProcedures();
			for(vector<QueryNode*>::iterator i = staticProcs.begin(); i != staticProcs.end(); i++) {
				errcode = executeRecQuery(*i);
				if(errcode != 0) return errcode;
				QueryResult *execution_result;
				errcode = qres->pop(execution_result);
				if (errcode != 0) return errcode;
				if (execution_result->type() != QueryResult::QREFERENCE) {
					return qeErrorOccur("[QE] TNCLASS error - execution result is not QueryReference", ERefExpected);
				}
				LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();
				ObjectPointer *optr;
				errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
				if (errcode != 0) {
					return trErrorOccur("[QE] class creating operation - Error in getObjectPointer.", errcode);
				}

				errcode = tr->addRoot(optr);
				if (errcode != 0) {
					return trErrorOccur("[QE] Error in addRoot", errcode);
				}

				string optr_name = optr->getName();

				LogicalID* newLid;
				errcode = lidFromReference(QE_STATIC_METHOD_BIND_NAME, lid, newLid);
				if(errcode != 0) return errcode;
				classVector.push_back(newLid);
			}

			// Seting extended classes (property classMarks)
			DataValue *dbValue = sm->createVectorValue(&classVector);
			for(vector<LogicalID *>::iterator i = extendedclassesLIDs.begin(); i != extendedclassesLIDs.end(); ++i) {
				dbValue->addClassMark(*i);
			}

			LogicalID* newLid;
			if(classExist) {//update part
				//Check if updated class try to extends its subclass, what is forbiden.
				bool isUpdatePossible = true;
				LogicalID* upLid;
				errcode = cg->checkExtendsForUpdate(className, dbValue->getClassMarks(), upLid, isUpdatePossible);
				if(errcode != 0) return errcode;
				if(!isUpdatePossible) {
					return 1;//TODO (sk) Proba dziedziczenia po podklasie.
				}
				ObjectPointer *upOptr;
				errcode = tr->getObjectPointer (upLid, Store::Read, upOptr, false);
				if (errcode != 0) {
					return trErrorOccur("[QE] update class operation - Error in getObjectPointer.", errcode);
				}
				//usuniecie informacji z nadklas
				errcode = cg->removePersistFromSuperclasses(upOptr, tr);
				if(errcode != 0) return trErrorOccur("[QE] error in removePersistFromSuperclasses", errcode);

				//Przekazanie podklas.
				//KLUCZOWA OPERACJA. Update istnieje po to, zeby podmienic klase, tak by,
				//klasy i obiekty z niej dziedziczace nadal sie do niej odnosily.
				dbValue->addSubclasses(upOptr->getValue()->getSubclasses());

				dbValue->setSubtype(Store::Class);

				errcode = tr->modifyObject(upOptr, dbValue);
				if (errcode != 0) {
					return trErrorOccur("[QE] TNCLASS - Error in modifyObject.", errcode);
				}
				newLid = upOptr->getLogicalID();
				errcode = qres->push(new QueryReferenceResult(newLid));
				if (errcode != 0) return errcode;
			} else {//create part
				errcode = createObjectAndPutOnQRes(dbValue, className, Store::Class, newLid);
				if(errcode != 0) return errcode;
			}

			// Seting newLid as subclass (property subclasses) in extended classes
			if(dbValue->getClassMarks() == NULL) {
				return 0;
			}
			for(SetOfLids::iterator i = (dbValue->getClassMarks())->begin(); i != (dbValue->getClassMarks())->end(); ++i) {
				LogicalID* classMark = *i;
				ObjectPointer *optr;
				errcode = tr->getObjectPointer (classMark, Store::Read, optr, false);
				if (errcode != 0) {
					return trErrorOccur("[QE] create class operation - Error in getObjectPointer of extended class.", errcode);
				}
				DataValue* newDV = optr->getValue()->clone();
				newDV->addSubclass(newLid);
				errcode = tr->modifyObject(optr, newDV);
				if(errcode != 0) {
					return trErrorOccur("[QE] Can't modify object and set classMark.", errcode);
				}
			}
			return 0;
		}

		case TreeNode::TNREGCLASS: {
			debug_print(*ec,  "[QE] Type: TNREGCLASS");
			QueryNode *query = ((RegisterClassNode *) tree)->getQuery();
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());

			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;

			if (execution_result->type() != QueryResult::QREFERENCE) {
				return qeErrorOccur( "[QE] TNREGCLASS error - execution result is not QueryReference", ERefExpected );
			}

			ObjectPointer *optr;
			errcode = tr->getObjectPointer (((QueryReferenceResult*)execution_result)->getValue(), Store::Read, optr, false);
			if (errcode != 0) {
				return trErrorOccur("[QE] register class operation - Error in getObjectPointer.", errcode);
			}

			bool classIsUpdated = cg->vertexExist(optr->getLogicalID());

			if(!classIsUpdated) {
				errcode = tr->addRoot(optr);
				if (errcode != 0) {
					return trErrorOccur("[QE] Error in addRoot", errcode);
				}
			}

			string invariant_name = "";
			vector<LogicalID*>* inner_vec = (optr->getValue())->getVector();
			for (unsigned int i=0; i < inner_vec->size(); i++) {
				ObjectPointer *inner_optr;
				errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
				if (errcode != 0) {
					return trErrorOccur("[QE] register class operation - Error in getObjectPointer.", errcode);
				}
				if (inner_optr->getName() == QE_INVARIANT_BIND_NAME) {
					invariant_name = (inner_optr->getValue())->getString();
					break;
				}
			}

			if(classIsUpdated) {
				errcode = tr->removeClass(optr);
				if (errcode != 0) {
					return trErrorOccur("[QE] Error in class removing.", errcode);
				}
			}
			errcode = tr->addClass(optr->getName().c_str(), invariant_name.c_str(), optr);
			if (errcode != 0) {
				return trErrorOccur("[QE] Error in addClass", errcode);
			}
			if(classIsUpdated) {
				errcode = cg->updateClass(optr, tr, this);
				if(errcode != 0) return errcode;
				im->implementationUpdated(optr->getName(), invariant_name, BIND_CLASS, tr);	
			} else {
				errcode = cg->addClass(optr, tr, this);
				if(errcode != 0) return errcode;
			}

			debug_print(*ec,  cg->toString());

			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;
			return 0;
		}

		case TreeNode::TNCREATE: {
			debug_print(*ec,  "[QE] Type: TNCREATE");
			QueryResult *result = new QueryBagResult();
			errcode = executeRecQuery (tree->getArg());
			if (errcode != 0) return errcode;
			QueryResult *tmp_result;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			QueryResult *bagRes = new QueryBagResult();
			bagRes->addResult(tmp_result);
			bool isStaticMember = ((CreateNode*)tree)->isClassMember();
			bool needsDeepCardCheck = tree->needsCoerce(TypeCheck::DTable::CD_CAN_CRT);
			for (unsigned int i = 0; i < bagRes->size(); i++) {
				QueryResult *binder;
				errcode = ((QueryBagResult *) bagRes)->at(i, binder);
				if (errcode != 0) return errcode;

				if ((binder->type()) != QueryResult::QBINDER) {
					return qeErrorOccur("[QE] objectFromBinder() expected a binder, got something else", EOtherResExp);
				}
				string newobject_name = ((QueryBinderResult*)binder)->getName();
				if (!am->hasAccess(CREATE, newobject_name)) 
				{
					debug_printf(*ec, "[QE] TNCREATE: user has no rights to create %s objects", newobject_name.c_str());
					continue;
				}
				
				
				if (needsDeepCardCheck) {
					int cum;
					errcode = sigs.checkSubCardsRec(tree->getCoerceSig(), binder, true, cum);
					if (errcode != 0)
					{
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
				}

				if(isStaticMember) {
					//statyczne skladowe klas moga byc tworzone tylko
					//gdy zostaly zadeklarowane w klasach

					bool fieldExist = false;
					errcode = cg->staticFieldExist(newobject_name, fieldExist);
					if(errcode != 0) return errcode;
					if(!fieldExist) {
						return 1;//TODO: No class def found
					}
				}

				bool foundView;
				errcode = createOnView(newobject_name, foundView, binder, result);
				if (errcode != 0) return errcode;
				if (!foundView)
				{	
					QueryBinderResult* binderR = (QueryBinderResult*)binder;

					debug_printf(*ec, "[QE] TNCREATE - checking bind for %s\n", newobject_name.c_str());
					bool interfaced = false;
					int implType = 0;
					string iName;
					bool found;
					SetOfLids classMarks;
					InterfaceKey k;
					if (im->isObjectNameBound(newobject_name))
					{   //Creating an interface object
						iName = im->getInterfaceNameForObject(newobject_name, found);						
						ImplementationInfo iI = im->getImplementationForInterface(iName, found, true);
						string impName = iI.getName();
						string impObjName = iI.getObjectName();
						int implType = iI.getType();
						if (found)
						{
							if (implType == BIND_CLASS)
							{
								LogicalID* classGraphLid = NULL;
								cg->getClassLidByName(impName, classGraphLid);
								if (classGraphLid)
								{
									binderR->setName(impObjName);
									SetOfLids classMarks;
									classMarks.insert(classGraphLid);
									interfaced = true;
								}
							}
							else if (implType == BIND_VIEW)
							{
								bool foundBoundView;
								binderR->setName(impObjName);
								errcode = createOnView(impObjName, foundBoundView, binderR, result);
								if (errcode) return errcode;
								interfaced = true;
							}
							else 
								return -1; //Should not happen - we requested FINAL bind in getInterfaceBindForObjName call
						}
					}
										
					ObjectPointer *optr;
					errcode = this->objectFromBinder(binderR, optr);
					if (errcode != 0) return errcode;
					
					if (interfaced)
					{
						if (implType == BIND_CLASS)
						{
							//TODO - what about "missing" fields? (in implementation but not visible via interface?)
							DataValue* newDV = optr->getValue()->clone();
							newDV->addClassMarks(&classMarks);
							errcode = tr->modifyObject(optr, newDV);
							if(errcode != 0) return trErrorOccur("[QE] Can't modify object and set classMarks", errcode);
						}
						//key to indicate "Special" reference, telling us that perhaps not all fields and methods are visible
						k.setKey(iName);
					}
					
					debug_printf(*ec, "[QE] TNCREATE - adding root for %s\n", optr->getName().c_str());

					errcode = tr->addRoot(optr);
					if (errcode != 0) {
						debug_print(*ec,  "[QE] Error in addRoot");
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}

					QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID(), k);
					
					((QueryBagResult *) result)->addResult (lidres);
				}
				//delete binder;
			}
			//delete tmp_result;
			//delete bagRes;
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] CREATE Done!");
			return 0;
		} //case TNCREATE

		case TreeNode::TNINT: {
			int intValue = ((IntNode *) tree)->getValue();
			debug_printf(*ec, "[QE] TNINT: %d\n", intValue);
			QueryResult *result = new QueryIntResult(intValue);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			debug_printf(*ec, "[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
		} //case TNINT

		case TreeNode::TNREFID: {
			int intValue = ((RefidNode *) tree)->getValue();
			debug_printf(*ec, "[QE] TNREFID: %d\n", intValue);
			LogicalID *lid = StoreManager::theStore->createLID(intValue);
			QueryResult *result = new QueryReferenceResult(lid);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			debug_printf(*ec, "[QE] QueryReferenceResult (%d) created\n",intValue);
			return 0;
		} //case TNREFID

		case TreeNode::TNSTRING: {
			string stringValue = ((StringNode *) tree)->getValue();
			debug_print(*ec,  "[QE] TNSTRING: " + stringValue);
			QueryResult *result = new QueryStringResult(stringValue);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] QueryStringResult created");
			return 0;
		} //case TNSTRING

		case TreeNode::TNDOUBLE: {
			double doubleValue = ((DoubleNode *) tree)->getValue();
			debug_printf(*ec, "[QE] TNDOUBLE: %f\n", doubleValue);
			QueryResult *result = new QueryDoubleResult(doubleValue);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			debug_printf(*ec, "[QE] QueryDoubleResult (%f) created\n", doubleValue);
			return 0;
		} //case TNDOUBLE

		case TreeNode::TNAS: {
			debug_print(*ec,  "[QE] AS/GROUP_AS operation recognized");
			string name = tree->getName();
			errcode = executeRecQuery(tree->getArg());
			if (errcode != 0) return errcode;
			QueryResult *tmp_result;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			bool grouped = ((NameAsNode *) tree)->isGrouped();
			QueryResult *final_result = new QueryBagResult();
			if (grouped) {
				debug_print(*ec,  "[QE] GROUP AS operation");
				QueryResult *tmp_binder = new QueryBinderResult(name, tmp_result);
				debug_print(*ec,  "[QE] GROUP AS: new binder created and added to final result");
				final_result->addResult(tmp_binder);
			}
			else {
				debug_print(*ec,  "[QE] AS operation");
				QueryResult *partial_result;
				if (((tmp_result->type()) != QueryResult::QSEQUENCE) && ((tmp_result->type()) != QueryResult::QBAG)) {
					partial_result = new QueryBagResult();
					partial_result->addResult(tmp_result);
				}
				else {
					partial_result = tmp_result;
				}
				for (unsigned int i = 0; i < partial_result->size(); i++) {
					QueryResult *nextResult;
					errcode = ((QueryBagResult *) partial_result)->at(i, nextResult);
					if (errcode != 0) return errcode;
					QueryResult *tmp_binder = new QueryBinderResult(name, nextResult);
					debug_print(*ec,  "[QE] AS: new binder created and added to final result");
					final_result->addResult(tmp_binder);
				}
				//delete partial_result;
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] AS operation Done!");
			return 0;
		} //case TNAS

		case TreeNode::TNUNOP: {
			UnOpNode::unOp op = ((UnOpNode *) tree)->getOp();
			debug_print(*ec,  "[QE] Unary operator - type recognized");
			errcode = executeRecQuery(tree->getArg());
			if (errcode != 0) return errcode;
			QueryResult *tmp_result;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			QueryResult *op_result;
			errcode = this->unOperate(op, tmp_result, op_result);
			if (errcode != 0) return errcode;
			//delete tmp_result;
			errcode = qres->push(op_result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Unary operation Done!");
			return 0;
		} //case TNUNOP

		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			debug_print(*ec,  "[QE] Algebraic operator - type recognized");
			QueryResult *lResult, *rResult;
			errcode = executeRecQuery(((AlgOpNode *) tree)->getLArg());
			if (errcode != 0) return errcode;
			errcode = executeRecQuery (((AlgOpNode *) tree)->getRArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(rResult);
			if (errcode != 0) return errcode;
			errcode = qres->pop(lResult);
			if (errcode != 0) return errcode;
			QueryResult *op_result;
			errcode = this->algOperate(op, lResult, rResult, op_result, (AlgOpNode *) tree);
			if (errcode != 0) return errcode;
			//delete lResult;
			//delete rResult;
			errcode = qres->push(op_result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Algebraic operation Done!");
			return 0;
		} // TNALGOP

		case TreeNode::TNFIX: {
			debug_print(*ec,  "[QE] FIXPOINT operation recognized");
			QueryResult *final = new QueryStructResult();
			int howManyOps = ((FixPointNode *) tree)->howManyParts();
			for (int i = 0; i<howManyOps; i++) {
				string currName = ((FixPointNode *) tree)->getName(i);
				QueryResult *currRes = new QueryBagResult();
				QueryResult *currBinder = new QueryBinderResult(currName, currRes);
				final->addResult(currBinder);
			}
			bool looping = true;
			while (looping) {
				if (this->evalStopped()) {
					debug_print(*ec,  "[QE] query evaluating stopped by Server, aborting transaction ");
					errcode = tr->abort();
					inTransaction = false;
					if (errcode != 0) {
						debug_print(*ec,  "[QE] error in transaction->abort()");
						return errcode;
					}
					debug_print(*ec,  "[QE] Transaction aborted succesfully");
					debug_print(*ec,  (ErrQExecutor | EEvalStopped));
					return (ErrQExecutor | EEvalStopped);
				}

				QueryResult *next_final = new QueryStructResult();

				errcode = (final)->nested_and_push_on_envs(this, tr);
				if (errcode != 0) return errcode;

				for (int i = 0; i<howManyOps; i++) {
					string currName = ((FixPointNode *) tree)->getName(i);
					errcode = executeRecQuery (((FixPointNode *) tree)->getQuery(i));
					if (errcode != 0) return errcode;
					QueryResult *currRes;
					errcode = qres->pop(currRes);
					if (errcode != 0) return errcode;
					QueryResult *currBinder = new QueryBinderResult(currName, currRes);
					next_final->addResult(currBinder);
				}
				errcode = envs->pop(this);
				if (errcode != 0) return errcode;
				if (not (final->equal(next_final))) {
					//delete final;
					final = next_final;
				}
				else looping = false;
			}
			errcode = qres->push(final);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Fixpoint operation Done!");
			return 0;
		} // TNFIX

		case TreeNode::TNPARAM: {
			debug_print(*ec,  "[QE] Parametr opeartion");
			QueryResult *res;
			string paramName = ((ParamNode *) tree)->getName();
			if (prms == NULL) {
				debug_print(*ec,  "[QE] error! Parametr operation - params == NULL");
				debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			int howMany = prms->count(paramName);
			if (howMany != 1) {
				debug_print(*ec,  "[QE] error! Parametr operation - wrong parametr");
				debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			map<string, QueryResult*>::iterator pos;
			pos = prms->find(paramName);
			if (pos != prms->end()) res = pos->second;
			else {
				debug_print(*ec,  "[QE] error! Parametr operation - index out of range");
				debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			QueryResult *final_res;
			errcode = exViews->reVirtualize(res, final_res);
			if (errcode != 0) return errcode;
			errcode = qres->push(final_res);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] Parametr operation Done!");
			return 0;
		} // TNPARAM

		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			debug_print(*ec,  "[QE] NonAlgebraic operator - type recognized");
			QueryResult *l_tmp_Result;
			am->setKeepSection(true);
			errcode = executeRecQuery (((NonAlgOpNode *) tree)->getLArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(l_tmp_Result);
			if (errcode != 0) return errcode;
			QueryResult *lResult;
			if (((l_tmp_Result->type()) != QueryResult::QSEQUENCE) && ((l_tmp_Result->type()) != QueryResult::QBAG)) {
				lResult = new QueryBagResult();
				lResult->addResult(l_tmp_Result);
			}
			else
				lResult = l_tmp_Result;
			debug_print(*ec,  "[QE] Left argument of NonAlgebraic query has been computed");
			QueryResult *partial_result = new QueryBagResult();
			QueryResult *visited_result = new QueryBagResult();
			QueryResult *final_result = new QueryBagResult();
			if ((op == NonAlgOpNode::closeBy) || (op == NonAlgOpNode::closeUniqueBy) || (op == NonAlgOpNode::leavesBy) || (op == NonAlgOpNode::leavesUniqueBy)) {
				switch (op) {
					case NonAlgOpNode::closeBy: {
						debug_print(*ec,  "[QE] NonAlgebraic recursive operator <closeBy>");
						partial_result->addResult(lResult);
						final_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::closeUniqueBy: {
						debug_print(*ec,  "[QE] NonAlgebraic recursive operator <closeUniqueBy>");
						partial_result->addResult(lResult);
						final_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::leavesBy: {
						debug_print(*ec,  "[QE] NonAlgebraic recursive operator <leavesBy>");
						partial_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::leavesUniqueBy: {
						debug_print(*ec,  "[QE] NonAlgebraic recursive operator <leavesUniqueBy>");
						partial_result->addResult(lResult);
						visited_result->addResult(lResult);
						break;
					}
					default: {
						break;
					}
				}//switch
				while (not (partial_result->isEmpty())) {

					if (this->evalStopped()) {
						debug_print(*ec,  "[QE] query evaluating stopped by Server, aborting transaction ");
						errcode = tr->abort();
						inTransaction = false;
						if (errcode != 0) {
							debug_print(*ec,  "[QE] error in transaction->abort()");
							return errcode;
						}
						debug_print(*ec,  "[QE] Transaction aborted succesfully");
						debug_print(*ec,  (ErrQExecutor | EEvalStopped));
						return (ErrQExecutor | EEvalStopped);
					}

					QueryResult *currentResult;
					errcode = partial_result->getResult(currentResult);
					if (errcode != 0) return errcode;

					errcode = (currentResult)->nested_and_push_on_envs(this, tr);
					if (errcode != 0) return errcode;

					QueryResult *newResult;
					errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
					if (errcode != 0) return errcode;
					errcode = qres->pop(newResult);
					if (errcode != 0) return errcode;
					debug_print(*ec,  "[QE] Computing right Argument with a new scope of ES");
					errcode = envs->pop(this);
					if (errcode != 0) return errcode;
					switch (op) {
						case NonAlgOpNode::closeBy: {
							QueryResult *nResult;
							if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
								nResult = new QueryBagResult();
								nResult->addResult(newResult);
							}
							else
								nResult = newResult;
							for (unsigned int i = 0; i < (nResult->size()); i++) {
								QueryResult *current_new_result;
								if ((nResult->type()) == QueryResult::QSEQUENCE)
									errcode = (((QuerySequenceResult *) nResult)->at(i, current_new_result));
								else
									errcode = (((QueryBagResult *) nResult)->at(i, current_new_result));
								if (errcode != 0) return errcode;
								partial_result->addResult(current_new_result); // this can cause an infinite loop !
								final_result->addResult(current_new_result);

							}
							break;
						}
						case NonAlgOpNode::closeUniqueBy: {
							QueryResult *nResult;
							if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
								nResult = new QueryBagResult();
								nResult->addResult(newResult);
							}
							else
								nResult = newResult;
							for (unsigned int i = 0; i < (nResult->size()); i++) {
								QueryResult *current_new_result;
								if ((nResult->type()) == QueryResult::QSEQUENCE)
									errcode = (((QuerySequenceResult *) nResult)->at(i, current_new_result));
								else
									errcode = (((QueryBagResult *) nResult)->at(i, current_new_result));
								if (errcode != 0) return errcode;
								bool isIncl;
								errcode = isIncluded(current_new_result, final_result, isIncl);
								if (errcode != 0) return errcode;
								if (not (isIncl)) { // occur check
									partial_result->addResult(current_new_result);
									final_result->addResult(current_new_result);
								}
							}
							break;
						}
						case NonAlgOpNode::leavesBy: {
							if (newResult->isNothing())
								final_result->addResult(currentResult);
							else {
								QueryResult *nResult;
								if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
									nResult = new QueryBagResult();
									nResult->addResult(newResult);
								}
								else
									nResult = newResult;
								for (unsigned int i = 0; i < (nResult->size()); i++) {
									QueryResult *c_result;
									if ((nResult->type()) == QueryResult::QSEQUENCE)
										errcode = (((QuerySequenceResult *) nResult)->at(i, c_result));
									else
										errcode = (((QueryBagResult *) nResult)->at(i, c_result));
									if (errcode != 0) return errcode;
									partial_result->addResult(c_result); // this can cause an infinite loop !
								}
							}
							break;
						}
						case NonAlgOpNode::leavesUniqueBy: {
							if (newResult->isNothing())
								final_result->addResult(currentResult);
							else {
								QueryResult *nResult;
								if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
									nResult = new QueryBagResult();
									nResult->addResult(newResult);
								}
								else
									nResult = newResult;
								for (unsigned int i = 0; i < (nResult->size()); i++) {
									QueryResult *c_result;
									if ((nResult->type()) == QueryResult::QSEQUENCE)
										errcode = (((QuerySequenceResult *) nResult)->at(i, c_result));
									else
										errcode = (((QueryBagResult *) nResult)->at(i, c_result));
									if (errcode != 0) return errcode;
									bool isIncl;
									errcode = isIncluded(c_result, visited_result, isIncl);
									if (errcode != 0) return errcode;
									if (not (isIncl)) { // occur check
										partial_result->addResult(c_result);
										visited_result->addResult(c_result);
									}
								}
							}
							break;
						}
						default: {
							break;
						}
					}//switch
				}//while
			}//if
			else { // this is normal (not recursive) non algebraic operation
				if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {

					debug_print(*ec,  "[QE] For each row of this score, the right argument will be computed");
					for (unsigned int i = 0; i < (lResult->size()); i++) {
						QueryResult *currentResult;

						if ((lResult->type()) == QueryResult::QSEQUENCE)
							errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
						else
							errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
						if (errcode != 0) return errcode;

						QueryResult *rResult;

						debug_printf(*ec, "[QE] nested operation started()\n");
						errcode = (currentResult)->nested_and_push_on_envs(this, tr);
						if (errcode != 0) return errcode;

						QueryBagResult *pseudoVars = new QueryBagResult();
						pseudoVars->addResult(new QueryBinderResult("_position", new QueryIntResult(i)));
						envs->push(pseudoVars, tr, this);

						debug_print(*ec,  "[QE] Computing right Argument with a new scope of ES");
						errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(rResult);
						if (errcode != 0) return errcode;

						errcode = envs->pop(this);
						if (errcode != 0) return errcode;
						// TODO: delete pseudoVars;
						errcode = envs->pop(this);
						if (errcode != 0) return errcode;

						errcode = this->combine(op,currentResult,rResult,partial_result);
						if (errcode != 0) return errcode;
						debug_print(*ec,  "[QE] Combined partial results");
					}
				}
				else {
					debug_print(*ec,  "[QE] ERROR! NonAlgebraic operation, bad left argument");
					debug_print(*ec,  (ErrQExecutor | EOtherResExp));
					return (ErrQExecutor | EOtherResExp);
				}
				errcode = merge(op,partial_result, ec, final_result);
				if (errcode != 0) return errcode;
				debug_print(*ec,  "[QE] Merged partial results into final result");
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] NonAlgebraic operation Done!");
			return 0;
		} //case TNNONALGOP

		case TreeNode::TNCOND: {
			CondNode::condOp op = ((CondNode *) tree)->getOp();
			debug_print(*ec,  "[QE] Conditional operator - type recognized");
			switch (op) {
				case CondNode::iff: {
					debug_print(*ec,  "[QE] IF <q1> THEN <q2> FI");
					QueryResult *tmp_result;
					errcode = executeRecQuery(((CondNode *) tree)->getCondition());
					if (errcode != 0) return errcode;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					bool bool_val;
					if (tmp_result->isBool()) {
						errcode = tmp_result->getBoolValue(bool_val);
						if (errcode != 0) return errcode;
					}
					else {
						debug_print(*ec,  "[QE] ERROR! <q1> must be BOOLEAN ");
						debug_print(*ec,  (ErrQExecutor | EBoolExpected));
						return (ErrQExecutor | EBoolExpected);
					}
					if (bool_val) {
						errcode = executeRecQuery(((CondNode *) tree)->getLArg());
						if (errcode != 0) return errcode;
					}
					else {
						QueryResult *result = new QueryNothingResult();
						errcode = qres->push(result);
						if (errcode != 0) return errcode;
					}
					break;
				}
				case CondNode::ifElsee: {
					debug_print(*ec,  "[QE] IF <q1> THEN <q2> ELSE <q3> FI");
					QueryResult *tmp_result;
					errcode = executeRecQuery(((CondNode *) tree)->getCondition());
					if (errcode != 0) return errcode;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					bool bool_val;
					if (tmp_result->isBool()) {
						errcode = tmp_result->getBoolValue(bool_val);
						if (errcode != 0) return errcode;
					}
					else {
						debug_print(*ec,  "[QE] ERROR! <q1> must be BOOLEAN ");
						debug_print(*ec,  (ErrQExecutor | EBoolExpected));
						return (ErrQExecutor | EBoolExpected);
					}
					if (bool_val) {
						errcode = executeRecQuery(((CondNode *) tree)->getLArg());
						if (errcode != 0) return errcode;
					}
					else {
						errcode = executeRecQuery(((CondNode *) tree)->getRArg());
						if (errcode != 0) return errcode;
					}
					break;
				}
				case CondNode::whilee: {
					debug_print(*ec,  "[QE] WHILE <q1> DO <q2> OD");
					QueryResult *tmp_result;
					errcode = executeRecQuery(((CondNode *) tree)->getCondition());
					if (errcode != 0) return errcode;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					bool bool_val;
					if (tmp_result->isBool()) {
						errcode = tmp_result->getBoolValue(bool_val);
						if (errcode != 0) return errcode;
					}
					else {
						debug_print(*ec,  "[QE] ERROR! <q1> must be BOOLEAN ");
						debug_print(*ec,  (ErrQExecutor | EBoolExpected));
						return (ErrQExecutor | EBoolExpected);
					}
					while (bool_val) {
						errcode = executeRecQuery(((CondNode *) tree)->getLArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
						errcode = executeRecQuery(((CondNode *) tree)->getCondition());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
						if (tmp_result->isBool()) {
							errcode = tmp_result->getBoolValue(bool_val);
							if (errcode != 0) return errcode;
						}
						else {
							debug_print(*ec,  "[QE] ERROR! <q1> must be BOOLEAN ");
							debug_print(*ec,  (ErrQExecutor | EBoolExpected));
							return (ErrQExecutor | EBoolExpected);
						}
					}
					QueryResult *result = new QueryNothingResult();
					errcode = qres->push(result);
					if (errcode != 0) return errcode;
					break;
				}
				default: {
					debug_print(*ec,  "[QE] ERROR! Condition operation type not known");
					debug_print(*ec,  (ErrQExecutor | EUnknownNode));
					return (ErrQExecutor | EUnknownNode);
				}
			}
			debug_print(*ec,  "[QE] Condition operation Done!");
			return 0;
		} //case TNCOND
			case TreeNode::TNINDEXDML: {
			IndexSelectNode* isn = dynamic_cast< IndexSelectNode*>(tree);
			vector<QueryNode*> subqueries = isn->getConstraints()->getSubqueries(); //maxymalnie 2 -> O(1)
			QueryResult *tmp_result, *derefResult;
			vector<QueryResult*> subresults;
			for (unsigned int i = 0; i < subqueries.size(); i++) {
				errcode = executeRecQuery(subqueries.at(i));
				if (errcode != 0) return errcode;
				errcode = qres->pop(tmp_result);
				if (errcode != 0) return errcode;
				errcode = this->derefQuery(tmp_result, derefResult);
				if (errcode != 0) return errcode;
				subresults.push_back(derefResult);
			}
			isn->getConstraints()->setSubresults(subresults);
			QueryResult *result;
			errcode = IndexManager::getHandle()->search((dynamic_cast< IndexSelectNode*>(tree)), tr , &result);
			if (errcode) return errcode;
			errcode = qres->push(result);
			return errcode;
		}
		case TreeNode::TNLINK: {
		debug_print(*ec,  "[QE] Link opeartion");

		LinkNode* link = (LinkNode*) tree;
		QueryResult* structRes = new QueryStructResult();
		QueryResult* ipRes = new QueryBinderResult("ip", new QueryStringResult(link->getIp()));
		QueryResult* portRes = new QueryBinderResult("port", new QueryIntResult(link->getPort()));
		structRes->addResult(ipRes);
		structRes->addResult(portRes);
		QueryResult* bind = new QueryBinderResult(link->getNodeName(), structRes);

		ObjectPointer *optr;
		errcode = this->objectFromBinder(bind, optr);
		DataValue* value = optr->getValue();
		value->setSubtype(Store::Link);
		if (errcode != 0) return errcode;

		string object_name = optr->getName();
		if (!am->hasAccess(CREATE, object_name)) 
		{
			debug_printf(*ec,  "[QE] user has no rights to create %s objects", object_name.c_str());	
		    QueryBagResult * result = new QueryBagResult();
		    errcode = qres->push(result);
		    if (errcode != 0) return errcode;
		    return 0;
		}
		if ((errcode = tr->addRoot(optr)) != 0) {
		    debug_print(*ec,  "[QE] Error in addRoot");
		    antyStarveFunction(errcode);
		    inTransaction = false;
		    return errcode;
		}

		QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
		QueryBagResult * result = new QueryBagResult();
		result->addResult (lidres);

		errcode = qres->push(result);
		if (errcode != 0) return errcode;

		debug_print(*ec,  "[QE] Link operation Done!");
		return 0;
		}
		case TreeNode::TNREMOTE: {

			RemoteNode* rn = (RemoteNode *) tree;
			LogicalID* lid = rn->getLogicalID();
			QueryReferenceResult* qrr = new QueryReferenceResult(lid);

			if (rn->isDeref()) {
			  //jesli deref to to!
				debug_printf(*ec, "[QE] zaczynam prosbe o zdalny deref");
				QueryResult *op_result;
				errcode = this->derefQuery(qrr, op_result);
				if (errcode != 0) return errcode;
				//delete tmp_result;
				errcode = qres->push(op_result);
				if (errcode != 0) return errcode;
				debug_print(*ec,  "[QE] Unary operation Done!");
				return 0;
			}

			debug_print(*ec,  "zaczynam wezel TNREMOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			vector<ObjectPointer*>* vec;

			debug_print(*ec,  "remote 1");
			if (lid != NULL) {
				debug_printf(*ec, "prosba o nested na obiekcie zdalnym\n");

				QueryResult *newStackSection;
				vector<DataValue*> _tmp_dataVal_vec;
				errcode = (qrr)->nested(const_cast<QueryExecutor*>(this), tr, newStackSection, _tmp_dataVal_vec);
				if (errcode != 0) return errcode;
				errcode = qres->push(newStackSection);
				if (errcode != 0) return errcode;
				debug_printf(*ec, "nested na obiekcie zdalnym udalo sie. nowa sekcja stosu gotowa");
				return 0;
			}
			debug_print(*ec,  "remote 1");
			if ((errcode = tr->getRoots(vec)) != 0) {
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			QueryBagResult *r = new QueryBagResult();
			int vecSize = vec->size();
			debug_printf(*ec, "[QE] %d Roots LID by name taken\n", vecSize);
			for (int i = 0; i < vecSize; i++ ) {
				 ObjectPointer *optr = vec->at(i);
				 LogicalID *lid = optr->getLogicalID();
				 string name = optr->getName();

				debug_printf(*ec, "[QE] LogicalID received: %d\n",  lid->toInteger());
				QueryReferenceResult *lidres = new QueryReferenceResult(lid);
				QueryBinderResult *qbr = new QueryBinderResult(name, lidres);
				r->addResult(qbr);
			}
			delete vec;
			qres->push(r);
			return 0;
		}
		case TreeNode::TNCOERCE : { // May appear only through TC coerce augments
			CoerceNode *cn = (CoerceNode *) tree;
			int cType = cn->getCType();
			debug_print(*ec,  "[QE] COERCE Node to be processed... ");
			errcode = executeRecQuery(cn->getArg());
			if (errcode != 0) return errcode;
			QueryResult *tmp_result;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;

			QueryResult *op_result;
			errcode = sigs.coerceOperate(cType, tmp_result, op_result, tree, tr);
			if (errcode != 0) return errcode;

			errcode = qres->push(op_result);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] COERCE Node Done!");
			return 0;
		}
		case TreeNode::TNCASTTO : {// May appear when variant result appears (eg. through union)...
			//QE ignores this node - it assumes typechecker checked it correct.
			debug_print(*ec,  "[QE] CAST TO Node ...");
			errcode = executeRecQuery(((SimpleCastNode *)tree)->getArg());
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] CAST TO Node Done!");
			return 0;
			//do not touch the qres stack - let superior node read what arg left there.
			//if the loopCHECK was introduced - it would just throw 'CastNode' error on error...
		}
		default:
			{
			debug_print(*ec,  "Unknown node type");
			debug_print(*ec,  (ErrQExecutor | EUnknownNode));
			return (ErrQExecutor | EUnknownNode);
			}

		} // end of switch(nodeType)

	} // if tree!=Null
	else { // tree == NULL; return empty result
		debug_print(*ec,  "[QE] empty tree, nothing to do");
		QueryResult *result = new QueryNothingResult();
		errcode = qres->push(result);
		if (errcode != 0) return errcode;
		debug_print(*ec,  "[QE] QueryNothingResult created");
	}
	return 0;
}//executeQuerry


int QueryExecutor::derefQuery(QueryResult *arg, QueryResult *&res) {
	debug_print(*ec,  "[QE] derefQuery()");
	int errcode;
	int argType = arg->type();
        ObjectPointer *optr = NULL;
        
	switch (argType) {
		case QueryResult::QSEQUENCE: {
			res = new QuerySequenceResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBAG: {
			res = new QueryBagResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryBagResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QSTRUCT: {
			res = new QueryStructResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryStructResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->derefQuery(((QueryBinderResult *) arg)->getItem(), tmp_item);
			if (errcode != 0) return errcode;
			res = new QueryBinderResult(tmp_name, tmp_item);
			break;
		}
		case QueryResult::QBOOL: {
			res = arg;
			break;
		}
		case QueryResult::QINT: {
			res = arg;
			break;
		}
		case QueryResult::QDOUBLE: {
			res = arg;
			break;
		}
		case QueryResult::QSTRING: {
			res = arg;
			break;
		}
		case QueryResult::QREFERENCE: {
			QueryReferenceResult *qRef = (QueryReferenceResult *) arg;
			LogicalID * lid = qRef->getValue();
			if (lid != NULL && lid->getServer() != "")
				return EUnknownNode;

			if (((QueryReferenceResult *) arg)->wasRefed()) {
				res = arg;
				break;
			}
			InterfaceKey interfaceKey = qRef->getInterfaceKey();
				
			debug_print(*ec,  "[QE] derefQuery() - dereferencing Object");

			errcode = tr->getObjectPointer(lid, Store::Read, optr, true);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] Error in getObjectPointer");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}

			if (optr == NULL) {
				res = new QueryBagResult();
				break;
			}

			if (optr->isRoot()) {
			    string object_name = optr->getName();
			}

			DataValue* value = optr->getValue();
			int vType = value->getType();
			switch (vType) {
				case Store::Integer: {
					debug_print(*ec,  "[QE] derefQuery() - ObjectValue = Int");
					int tmp_value = value->getInt();
					res = new QueryIntResult(tmp_value);
					break;
				}
				case Store::Double: {
					debug_print(*ec,  "[QE] derefQuery() - ObjectValue = Double");
					double tmp_value = value->getDouble();
					res = new QueryDoubleResult(tmp_value);
					break;
				}
				case Store::String: {
					debug_print(*ec,  "[QE] derefQuery() - ObjectValue = String");
					string tmp_value = value->getString();
					res = new QueryStringResult(tmp_value);
					break;
				}
				case Store::Pointer: {
					debug_print(*ec,  "[QE] derefQuery() - ObjectValue = Pointer");
					LogicalID *tmp_value = value->getPointer()->clone();
					//LogicalID *tmp_value = value->getPointer();
                                        res = new QueryReferenceResult(tmp_value);
					break;
				}
				case Store::Vector: {
					debug_print(*ec,  "[QE] derefQuery() - ObjectValue = Vector");
					
					bool filterOut;
					string k = interfaceKey.getKey();
					TStringSet namesVisible = im->getAllFieldNamesAndCheckBind(k, filterOut);
					if (filterOut)
						debug_printf(*ec, "[QE] derefQuery - field names will be filtered out, %d names visible\n", namesVisible.size());
					
					vector<LogicalID*>* tmp_vec = value->getVector();
					res = new QueryStructResult();
					int tmp_vec_size = tmp_vec->size();
					for (int i = 0; i < tmp_vec_size; i++) {
						LogicalID *currID = tmp_vec->at(i)->clone();
						ObjectPointer *currOptr;
						errcode = tr->getObjectPointer(currID, Store::Read, currOptr, false);
						if (errcode != 0) {
							debug_print(*ec,  "[QE] Error in getObjectPointer");
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
					
						string currName = currOptr->getName();
						if (filterOut)
						{
							if (namesVisible.find(currName) == namesVisible.end())
							{   //Field filtered out
								continue;
							}
						}
						
						//MH set direct parent id...
						currID->setDirectParent(lid);
						QueryResult *currIDRes = new QueryReferenceResult(currID);
						QueryResult *currInside;
						this->derefQuery(currIDRes, currInside);
						QueryResult *currBinder = new QueryBinderResult(currName, currInside);
						res->addResult(currBinder);
/*                                                //gtimoszuk
                                                if (currOptr != NULL) {
                                                    delete currOptr;
                                                } */
					}
					break;
				}
				default: {
					debug_print(*ec,  "[QE] derefQuery() - wrong argument type");
					debug_print(*ec,  (ErrQExecutor | EUnknownValue));
					return (ErrQExecutor | EUnknownValue);
					break;
				}
			}
			break;
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		case QueryResult::QVIRTUAL: {
			QueryVirtualResult *vres = (QueryVirtualResult *) arg;
			if (vres->refed) {
				res = arg;
				break;
			}
			LogicalID *view_def = vres->view_defs.at(0);
			vector<QueryResult *> seeds = vres->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = exViews->getOn_procedure(view_def, "on_retrieve", proc_code, proc_param, tr);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
			if (proc_code == "") {
				debug_print(*ec,  "[QE] derefQuery() - this VirtualObject doesn't have this operation defined");
				debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
				return (ErrQExecutor | EOperNotDefined);
			}
			
			string k = vres->getInterfaceKey().getKey();

			vector<QueryBagResult*> envs_sections;

			errcode = exViews->createNewSections(vres, NULL, NULL, envs_sections, tr, this);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}	

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;

			errcode = qres->pop(res);
			if (errcode != 0) return errcode;			
					
			bool filterOut;
			TStringSet namesVisible = im->getAllFieldNamesAndCheckBind(k, filterOut);	
			if ((res->type() == QueryResult::QSTRUCT) && (filterOut))
			{
				debug_printf(*ec, "[QE] derefQuery - dereferenced names will be filtered out, %d names visible\n", namesVisible.size());			
				QueryStructResult *structres = (QueryStructResult *)res;
				QueryStructResult *accepted = new QueryStructResult();
				for (size_t it = 0; it != structres->size(); ++it)
				{
					QueryResult *currBinder;
					errcode = structres->at(it, currBinder);
					if (errcode) return errcode;
					if (currBinder->type() == QueryResult::QBINDER)
					{
						QueryBinderResult* bindres = (QueryBinderResult *) currBinder;
						string name = bindres->getName();
						if (namesVisible.find(name) == namesVisible.end())
						{   //name filtered out
							continue;
						}
						accepted->addResult(currBinder);
					}
					else
						accepted->addResult(currBinder);
				}
				res = accepted;
			}
			
			
			break;
		}
		default: {
			debug_print(*ec,  "[QE] ERROR in derefQuery() - unknown result type");
			debug_print(*ec,  (ErrQExecutor | EOtherResExp));
			return (ErrQExecutor | EOtherResExp);
		}
	}
	QueryResult *tmp_res = res;
	int tmp_resType = tmp_res->type();
	if (tmp_resType == QueryResult::QBAG) {
		if (tmp_res->size() == 1) {
			errcode = ((QueryBagResult *) tmp_res)->at(0, res);
			if (errcode != 0) return errcode;
		}
	}
	else if (tmp_resType == QueryResult::QSEQUENCE) {
		if (tmp_res->size() == 1) {
			errcode = ((QuerySequenceResult *) tmp_res)->at(0, res);
			if (errcode != 0) return errcode;
		}
	}
/*        //gtimoszuk
        
        if (optr != NULL) {
            delete optr;
        } */
         
        return 0;
}

int QueryExecutor::refQuery(QueryResult *arg, QueryResult *&res) {
	debug_print(*ec,  "[QE] refQuery()");
	int errcode;
	int argType = arg->type();
	switch (argType) {
		case QueryResult::QSEQUENCE: {
			res = new QuerySequenceResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->refQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBAG: {
			res = new QueryBagResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryBagResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->refQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QSTRUCT: {
			res = new QueryStructResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryStructResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->refQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->refQuery(((QueryBinderResult *) arg)->getItem(), tmp_item);
			if (errcode != 0) return errcode;
			res = new QueryBinderResult(tmp_name, tmp_item);
			break;
		}
		case QueryResult::QBOOL: {
			res = arg;
			break;
		}
		case QueryResult::QINT: {
			res = arg;
			break;
		}
		case QueryResult::QDOUBLE: {
			res = arg;
			break;
		}
		case QueryResult::QSTRING: {
			res = arg;
			break;
		}
		case QueryResult::QREFERENCE: {
			res = arg;
			((QueryReferenceResult *) res)->setRef();
			break;
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		case QueryResult::QVIRTUAL: {
			res = arg;
			((QueryVirtualResult *) res)->refed = true;
			break;
		}
		default: {
			debug_print(*ec,  "[QE] ERROR in refQuery() - unknown result type");
			debug_print(*ec,  (ErrQExecutor | EOtherResExp));
			return (ErrQExecutor | EOtherResExp);
		}
	}
	return 0;
}

int QueryExecutor::nameofQuery(QueryResult *arg, QueryResult *&res) {


    debug_print(*ec,  "[QE] nameofQuery()");
    int errcode;
    int argType = arg->type();
    switch (argType) {
        case QueryResult::QSEQUENCE: {
            res = new QuerySequenceResult();
            for (unsigned int i = 0; i < (arg->size()); i++) {
                QueryResult *tmp_item;
                errcode = ((QuerySequenceResult *) arg)->at(i, tmp_item);
                if (errcode != 0) return errcode;
                QueryResult *tmp_res;
                errcode = this->nameofQuery(tmp_item, tmp_res);
                if (errcode != 0) return errcode;
                res->addResult(tmp_res);
            }
            break;
        }
        case QueryResult::QBAG: {
            res = new QueryBagResult();
            for (unsigned int i = 0; i < (arg->size()); i++) {
                QueryResult *tmp_item;
                errcode = ((QueryBagResult *) arg)->at(i, tmp_item);
                if (errcode != 0) return errcode;
                QueryResult *tmp_res;
                errcode = this->nameofQuery(tmp_item, tmp_res);
                if (errcode != 0) return errcode;
                res->addResult(tmp_res);
            }
            break;
        }
        case QueryResult::QSTRUCT: {
            res = new QueryStructResult();
            for (unsigned int i = 0; i < (arg->size()); i++) {
                QueryResult *tmp_item;
                errcode = ((QueryStructResult *) arg)->at(i, tmp_item);
                if (errcode != 0) return errcode;
                QueryResult *tmp_res;
                errcode = this->nameofQuery(tmp_item, tmp_res);
                if (errcode != 0) return errcode;
                res->addResult(tmp_res);
            }
            break;
        }
        case QueryResult::QBINDER: {
            string tmp_name = ((QueryBinderResult *) arg)->getName();
            QueryResult *tmp_item;
            errcode = this->nameofQuery(((QueryBinderResult *) arg)->getItem(), tmp_item);
            if (errcode != 0) return errcode;
            res = new QueryBinderResult(tmp_name, tmp_item);
            break;
        }
        case QueryResult::QBOOL: {
            res = new QueryBagResult();
            break;
        }
        case QueryResult::QINT: {
            res = new QueryBagResult();
            break;
        }
        case QueryResult::QDOUBLE: {
            res = new QueryBagResult();
            break;
        }
        case QueryResult::QSTRING: {
            res = new QueryBagResult();
            break;
        }
        case QueryResult::QREFERENCE: {

            LogicalID * lid = ((QueryReferenceResult *) arg)->getValue();

            if (lid != NULL && lid->getServer() != "") {
		    return EUnknownNode;
            }

            if (((QueryReferenceResult *) arg)->wasRefed()) {
                res = arg;
                break;
            }
            LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
            debug_print(*ec,  "[QE] nameof() - dereferencing Object");
            ObjectPointer *optr;
            errcode = tr->getObjectPointer(ref_value, Store::Read, optr, true);
            if (errcode != 0) {
                debug_print(*ec,  "[QE] Error in getObjectPointer");
                antyStarveFunction(errcode);
                inTransaction = false;
                return errcode;
            }

            if (optr == NULL) {
        		res = new QueryBagResult();
        		break;
            }

            res = new QueryStringResult(optr->getName());

            break;
        }
        case QueryResult::QNOTHING: {
            res = arg;
            break;
        }
        case QueryResult::QVIRTUAL: {
		string vo_name = ((QueryVirtualResult*) arg)->vo_name;
		res = new QueryStringResult(vo_name);
		break;
	}
        default: {
            debug_print(*ec,  "[QE] ERROR in derefQuery() - unknown result type");
            debug_print(*ec,  (ErrQExecutor | EOtherResExp));
            return (ErrQExecutor | EOtherResExp);
        }
    }
    QueryResult *tmp_res = res;
    int tmp_resType = tmp_res->type();
    if (tmp_resType == QueryResult::QBAG) {
        if (tmp_res->size() == 1) {
            errcode = ((QueryBagResult *) tmp_res)->at(0, res);
            if (errcode != 0) return errcode;
        }
    }
    else if (tmp_resType == QueryResult::QSEQUENCE) {
        if (tmp_res->size() == 1) {
            errcode = ((QuerySequenceResult *) tmp_res)->at(0, res);
            if (errcode != 0) return errcode;
        }
    }
    return 0;

    res = new QueryStringResult("NAME");
    return 0;
}

int QueryExecutor::unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final) {
	int errcode;
	switch (op) {
		case UnOpNode::deref: {
			debug_print(*ec,  "[QE] DEREF operation");
			errcode = this->derefQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::ref: {
			debug_print(*ec,  "[QE] REF operation");
			errcode = this->refQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::nameof: {
			debug_print(*ec,  "[QE] NAMEOF operation");
			errcode = this->nameofQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::unMinus: {
			debug_print(*ec,  "[QE] UN_MINUS operation");
			QueryResult *derefArg;
			errcode = this->derefQuery(arg,derefArg);
			if ((derefArg->type() == QueryResult::QINT) || (derefArg->type() == QueryResult::QDOUBLE)) {
				if (derefArg->type() == QueryResult::QINT)
					errcode = ((QueryIntResult *)derefArg)->minus(final);
				else
					errcode = ((QueryDoubleResult *)derefArg)->minus(final);
				if (errcode != 0) return errcode;
			}
			else {
				debug_print(*ec,  "[QE] ERROR! UN_MINUS argument must be INT or DOUBLE");
				debug_print(*ec,  (ErrQExecutor | ENumberExpected));
				return (ErrQExecutor | ENumberExpected);
			}
			break;
		}
		case UnOpNode::boolNot: {
			debug_print(*ec,  "[QE] NOT operation");
			if (arg->isBool()) {
				bool bool_tmp;
				errcode = arg->getBoolValue(bool_tmp);
				if (errcode != 0) return errcode;
				final = new QueryBoolResult(not bool_tmp);
			}
			else {
				debug_print(*ec,  "[QE] ERROR! NOT argument must be BOOLEAN");
				debug_print(*ec,  (ErrQExecutor | EBoolExpected));
				return (ErrQExecutor | EBoolExpected);
			}
			break;
		}
		case UnOpNode::count: {
			debug_print(*ec,  "[QE] COUNT operation");
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			int count_res = bagArg->size();
			final = new QueryIntResult(count_res);
			break;
		}
		case UnOpNode::sum: {
			debug_print(*ec,  "[QE] SUM operation");
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_sum_res = 0;
			int i_count = 0;
			double d_sum_res = 0.0;
			int d_count = 0;
			if (count_res == 0)
				final = new QueryIntResult(0);
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						i_sum_res = i_sum_res + (((QueryIntResult *) curr_res)->getValue());
						i_count++;
						break;
					}
					case QueryResult::QDOUBLE: {
						d_sum_res = d_sum_res + (((QueryDoubleResult *) curr_res)->getValue());
						d_count++;
						break;
					}
					default : { break; }
				}
			}
			if (d_count > 0) {
				d_sum_res = d_sum_res + ((double) i_sum_res);
				final = new QueryDoubleResult(d_sum_res);
			}
			else {
				final = new QueryIntResult(i_sum_res);
			}
			break;
		}
		case UnOpNode::avg: {
			debug_print(*ec,  "[QE] AVG operation");
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_sum_res = 0;
			int i_count = 0;
			double d_sum_res = 0.0;
			int d_count = 0;
			if (count_res == 0)
				final = new QueryIntResult(0);
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						i_sum_res = i_sum_res + (((QueryIntResult *) curr_res)->getValue());
						i_count++;
						break;
					}
					case QueryResult::QDOUBLE: {
						d_sum_res = d_sum_res + (((QueryDoubleResult *) curr_res)->getValue());
						d_count++;
						break;
					}
					default : { break; }
				}
			}
			d_sum_res = d_sum_res + ((double) i_sum_res);
			double total = d_sum_res / (i_count + d_count);
			final = new QueryDoubleResult(total);
			break;
		}
		case UnOpNode::min: {
			debug_print(*ec,  "[QE] MIN operation");
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_min_res = 0;
			bool i_counted = false;
			double d_min_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				debug_print(*ec,  "[QE] MIN operation - can't evaluate min value on this set, QueryNothingResult created");
				final = new QueryNothingResult();
			}
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						if (not i_counted) {
							i_min_res = (((QueryIntResult *) curr_res)->getValue());
							i_counted = true;
						}
						else {
							if ((((QueryIntResult *) curr_res)->getValue()) < i_min_res)
								i_min_res = (((QueryIntResult *) curr_res)->getValue());
						}
						break;
					}
					case QueryResult::QDOUBLE: {
						if (not d_counted) {
							d_min_res = (((QueryDoubleResult *) curr_res)->getValue());
							d_counted = true;
						}
						else {
							if ((((QueryDoubleResult *) curr_res)->getValue()) < d_min_res)
								d_min_res = (((QueryDoubleResult *) curr_res)->getValue());
						}
						break;
					}
					default : { break; }
				}
			}
			if (i_counted) {
				if (d_counted) {
					if (i_min_res <= d_min_res)
						final = new QueryIntResult(i_min_res);
					else
						final = new QueryDoubleResult(d_min_res);
				}
				else {
					final = new QueryIntResult(i_min_res);
				}
			}
			else {
				if (d_counted) {
					final = new QueryDoubleResult(d_min_res);
				}
				else { //no int or double value
					debug_print(*ec,  "[QE] MIN - can't evaluate min value on this set, QueryNothingResult created");
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::max: {
			debug_print(*ec,  "[QE] MAX operation");
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_max_res = 0;
			bool i_counted = false;
			double d_max_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				debug_print(*ec,  "[QE] MAX operation - can't evaluate max value on this set, QueryNothingResult created");
				final = new QueryNothingResult();
			}
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						if (not i_counted) {
							i_max_res = (((QueryIntResult *) curr_res)->getValue());
							i_counted = true;
						}
						else {
							if ((((QueryIntResult *) curr_res)->getValue()) > i_max_res)
								i_max_res = (((QueryIntResult *) curr_res)->getValue());
						}
						break;
					}
					case QueryResult::QDOUBLE: {
						if (not d_counted) {
							d_max_res = (((QueryDoubleResult *) curr_res)->getValue());
							d_counted = true;
						}
						else {
							if ((((QueryDoubleResult *) curr_res)->getValue()) > d_max_res)
								d_max_res = (((QueryDoubleResult *) curr_res)->getValue());
						}
						break;
					}
					default : { break; }
				}
			}
			if (i_counted) {
				if (d_counted) {
					if (i_max_res >= d_max_res)
						final = new QueryIntResult(i_max_res);
					else
						final = new QueryDoubleResult(d_max_res);
				}
				else {
					final = new QueryIntResult(i_max_res);
				}
			}
			else {
				if (d_counted) {
					final = new QueryDoubleResult(d_max_res);
				}
				else { //no int or double value
					debug_print(*ec,  "[QE] MAX - can't evaluate max value on this set, QueryNothingResult created");
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::distinct: {
			debug_print(*ec,  "[QE] DISTINCT operation");
			final = new QueryBagResult();
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			if ((bagArg->size()) == 0) break;
			((QueryBagResult *) bagArg)->sortBag();
			QueryResult *current;
			errcode = ((QueryBagResult *) bagArg)->getResult(current);
			if (errcode != 0) return errcode;
			final->addResult(current);
			while ((bagArg->size()) > 0) {
				QueryResult *next;
				errcode = ((QueryBagResult *) bagArg)->getResult(next);
				if (errcode != 0) return errcode;
				if (next->not_equal(current)) {
					current = next;
					final->addResult(current);
				}
			}
			break;
		}
		case UnOpNode::deleteOp: {
			debug_print(*ec,  "[QE] DELETE operation");
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			errcode = persistDelete(bagArg);
			if(errcode != 0) return errcode;
			debug_print(*ec,  "[QE] delete operation complete - QueryNothingResult created");
			final = new QueryNothingResult();
			break;
		}
		default: { // unOperation type not known
			debug_print(*ec,  "[QE] ERROR! Unary operation type not known");
			debug_print(*ec,  (ErrQExecutor | EUnknownNode));
			return (ErrQExecutor | EUnknownNode);
		}
	}
	return 0;
}

int QueryExecutor::persistStaticMembersDelete(const string& object_name) {
	vector<LogicalID*>* lids;
	int errcode = tr->getRootsLIDWithBegin(ClassGraph::classNameToExtPrefix(object_name), lids);
	if (errcode != 0) {
		return trErrorOccur("[QE] Error in geting static members to remove.", errcode);
	}
	errcode = persistDelete(lids);
	delete lids;
	if (errcode != 0) return errcode;
	return 0;
}

int QueryExecutor::persistDelete(LogicalID *lid) {
	int errcode = 0;
	ObjectPointer *optr;
	if ((errcode = tr->getObjectPointer (lid, Store::Write, optr, true)) !=0) {
		debug_print(*ec,  "[QE] Error in getObjectPointer.");
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}

	if (optr == NULL) {
		return 0;
	}
	
	string object_name = optr->getName();
	
	if (!am->hasAccess(DELETE, object_name))
	{
		debug_printf(*ec, "[QE] user has no delete right to %s objects", object_name.c_str());
		return 0;
	}
	
	//Class does not have to be root.
	Store::ExtendedType et = (optr->getValue())->getSubtype(); 
	if (et == Store::Class) {
		//Class removing means removing of all its static members too.
		errcode = persistStaticMembersDelete(object_name);
		if (errcode != 0) return errcode;
		//Removing class from superclasses and subclasses and updating class graph.
		errcode = cg->removePersistFromSuperclasses(optr, tr);
		if (errcode != 0) return trErrorOccur("[QE] error in removePersistFromSuperclasses.", errcode);
		errcode = cg->removePersistFromSubclasses(optr, tr);
		if (errcode != 0) return trErrorOccur("[QE] error in removePersistFromSubclasses", errcode);
		cg->removeClass(optr->getLogicalID());
		//Removing class from class file.
		errcode = tr->removeClass(optr);
		if (errcode != 0) {
			return trErrorOccur("[QE] Error in class removing.", errcode);
		}
	}
	else if (et == Store::Interface) 
	{
		string interfaceName = object_name;
		errcode = im->removeInterface(interfaceName, tr, optr);
		if (errcode != 0) 
			return trErrorOccur("[QE] Error while removing interface", errcode);
		im->implementationRemoved(interfaceName, tr);
		return 0;
	}
	else if (et == Store::StoreSchema)
	{
		string schemaName = object_name;
		errcode = os->removeSchema(schemaName, tr, optr);
		if (errcode)
			return trErrorOccur("[QE] Error while removing outer schema", errcode);
		return 0;
	}
	
	

	if (optr->getIsRoot()) {
		if (et == Store::View) {
			if ((errcode = tr->removeView(optr)) != 0) {
				debug_print(*ec,  "[QE] Error in removeView.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
		}
		if ((errcode = tr->removeRoot(optr)) != 0) {
			debug_print(*ec,  "[QE] Error in removeRoot.");
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
		debug_print(*ec,  "[QE] Root removed");
	}

	if ((errcode = tr->deleteObject(optr)) != 0) {
		debug_print(*ec,  "[QE] Error in deleteObject.");
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	
	if (et == Store::Class || et == Store::View)
		im->implementationRemoved(object_name, tr);
	
	return 0;
}

int QueryExecutor::persistDelete(vector<LogicalID*>* lids) {
	if(lids == NULL) return 0;
	for(vector<LogicalID*>::iterator i = lids->begin(); i != lids->end(); ++i) {
		int errcode = persistDelete(*i);
		if(errcode != 0) return errcode;
	}
	return 0;
}

int QueryExecutor::persistDelete(QueryResult* bagArg) {
	int errcode = 0;
	for (unsigned int i = 0; i < bagArg->size(); i++) {
		QueryResult* toDelete;  //the object to be deleted
		errcode = ((QueryBagResult *) bagArg)->at(i, toDelete);
		if (errcode != 0) return errcode;
		int toDeleteType = toDelete->type();
		
		if (toDeleteType == QueryResult::QREFERENCE) {
			LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
			errcode = persistDelete(lid);
			if(errcode != 0) return errcode;
		}
		else if (toDeleteType == QueryResult::QVIRTUAL) {
			LogicalID *view_def = ((QueryVirtualResult*) toDelete)->view_defs.at(0);
			vector<QueryResult *> seeds = ((QueryVirtualResult*) toDelete)->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = exViews->getOn_procedure(view_def, "on_delete", proc_code, proc_param, tr);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

			if (proc_code == "") {
				debug_print(*ec,  "[QE] delete() - this VirtualObject doesn't have this operation defined");
				debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			errcode = exViews->createNewSections((QueryVirtualResult*) toDelete, NULL, NULL, envs_sections, tr, this);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;

			QueryResult *res;
			errcode = qres->pop(res);
			if (errcode != 0) return errcode;
		}
		else {
			debug_print(*ec,  "[QE] ERROR! DELETE argument must consist of QREFERENCE");
			debug_print(*ec,  (ErrQExecutor | ERefExpected));
			return (ErrQExecutor | ERefExpected);
		}
		debug_print(*ec,  "[QE] Object deleted");
	}
	return 0;
}


int QueryExecutor::algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final, AlgOpNode *tn) {
	int errcode;
	if ((op == AlgOpNode::plus) || (op == AlgOpNode::minus) || (op == AlgOpNode::times) || (op == AlgOpNode::divide)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		if ((derefL->type() == QueryResult::QSTRING) || (derefR->type() == QueryResult::QSTRING)) {
			stringstream ss;
			string leftString;
			switch(derefL->type()) {
				case QueryResult::QINT: {
					ss << (((QueryIntResult *)derefL)->getValue());
					ss >> leftString;
					break;
				}
				case QueryResult::QDOUBLE: {
					ss << (((QueryDoubleResult *)derefL)->getValue());
					ss >> leftString;
					break;
				}
				case QueryResult::QSTRING: {
					leftString = ((QueryStringResult *)derefL)->getValue();
					break;
				}
				default: break;
					//ignore, but not sure if it's the right thing to do
			}
			string rightString;
			switch(derefR->type()) {
				case QueryResult::QINT: {
					ss << (((QueryIntResult *)derefR)->getValue());
					ss >> rightString;
					break;
				}
				case QueryResult::QDOUBLE: {
					ss << (((QueryDoubleResult *)derefR)->getValue());
					ss >> rightString;
					break;
				}
				case QueryResult::QSTRING: {
					rightString = ((QueryStringResult *)derefR)->getValue();
					break;
				}
				default:
					break;
					//ignore, but not sure if it's the right thing to do
			}
			string final_value = leftString + rightString;
			final = new QueryStringResult(final_value);
		}
		else if ((derefL->type() == QueryResult::QINT) || (derefL->type() == QueryResult::QDOUBLE)) {
			switch (op) {
				case AlgOpNode::plus: {
					debug_print(*ec,  "[QE] + operation");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->plus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->plus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::minus: {
					debug_print(*ec,  "[QE] - operation");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->minus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->minus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::times: {
					debug_print(*ec,  "[QE] * operation");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->times(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->times(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::divide: {
					debug_print(*ec,  "[QE] / operation");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->divide_by(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->divide_by(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				default: { break; }
			}
		}
		else {
			switch (op) {
				case AlgOpNode::plus: {
					debug_print(*ec,  "[QE] ERROR! + arguments must be INT or DOUBLE");
					debug_print(*ec,  (ErrQExecutor | ENumberExpected));
					return (ErrQExecutor | ENumberExpected);
				}
				case AlgOpNode::minus: {
					debug_print(*ec,  "[QE] ERROR! - arguments must be INT or DOUBLE");
					debug_print(*ec,  (ErrQExecutor | ENumberExpected));
					return (ErrQExecutor | ENumberExpected);
				}
				case AlgOpNode::times: {
					debug_print(*ec,  "[QE] ERROR! * arguments must be INT or DOUBLE");
					debug_print(*ec,  (ErrQExecutor | ENumberExpected));
					return (ErrQExecutor | ENumberExpected);
				}
				case AlgOpNode::divide: {
					debug_print(*ec,  "[QE] ERROR! / arguments must be INT or DOUBLE");
					debug_print(*ec,  (ErrQExecutor | ENumberExpected));
					return (ErrQExecutor | ENumberExpected);
				}
				default: { break; }
			}
		}
		return 0;
	}
	else if ((op == AlgOpNode::eq) || (op == AlgOpNode::neq) || (op == AlgOpNode::lt) ||
	(op == AlgOpNode::gt) || (op == AlgOpNode::le) || (op == AlgOpNode::ge)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		switch (op) {
			case AlgOpNode::eq: {
				debug_print(*ec,  "[QE] = operation");
				bool bool_tmp = derefL->equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::neq: {
				debug_print(*ec,  "[QE] != operation");
				bool bool_tmp = derefL->not_equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::lt: {
				debug_print(*ec,  "[QE] < operation");
				bool bool_tmp = derefL->less_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::gt: {
				debug_print(*ec,  "[QE] > operation");
				bool bool_tmp = derefL->greater_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::le: {
				debug_print(*ec,  "[QE] <= operation");
				bool bool_tmp = derefL->less_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::ge: {
				debug_print(*ec,  "[QE] >= operation");
				bool bool_tmp = derefL->greater_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			default : { break; }
		}
		return 0;
	}
	else if ((op == AlgOpNode::boolOr) || (op == AlgOpNode::boolAnd)) {
		if ((lArg->isBool()) && (rArg->isBool())) {
			bool bool_l, bool_r;
			errcode = lArg->getBoolValue(bool_l);
			if (errcode != 0) return errcode;
			errcode = rArg->getBoolValue(bool_r);
			if (errcode != 0) return errcode;
			switch (op) {
				case AlgOpNode::boolOr: {
					debug_print(*ec,  "[QE] OR operation");
					bool bool_tmp = (bool_l || bool_r);
					final = new QueryBoolResult(bool_tmp);
					break;
				}
				case AlgOpNode::boolAnd: {
					debug_print(*ec,  "[QE] AND operation");
					bool bool_tmp = (bool_l && bool_r);
					final = new QueryBoolResult(bool_tmp);
					break;
				}
				default : { break; }
			}
		}
		else {
			switch (op) {
				case AlgOpNode::boolOr: {
					debug_print(*ec,  "[QE] ERROR! OR arguments must be BOOLEAN");
					debug_print(*ec,  (ErrQExecutor | EBoolExpected));
					return (ErrQExecutor | EBoolExpected);
				}
				case AlgOpNode::boolAnd: {
					debug_print(*ec,  "[QE] ERROR! AND arguments must be BOOLEAN");
					debug_print(*ec,  (ErrQExecutor | EBoolExpected));
					return (ErrQExecutor | EBoolExpected);
				}
				default : { break; }
			}
		}
		return 0;
	}
	else if (op == AlgOpNode::bagUnion) {
		debug_print(*ec,  "[QE] BAG_UNION operation");
		final = new QueryBagResult();
		((QueryBagResult *) final)->addResult(lArg);
		((QueryBagResult *) final)->addResult(rArg);
		return 0;
	}
	else if ((op == AlgOpNode::bagIntersect) || (op == AlgOpNode::bagMinus)) {
		if (op == (AlgOpNode::bagIntersect)) debug_print(*ec,  "[QE] BAG_INTERSECT operation");
		if (op == (AlgOpNode::bagMinus)) debug_print(*ec,  "[QE] BAG_MINUS operation");

		final = new QueryBagResult();
		QueryResult *leftBag = new QueryBagResult();
		leftBag->addResult(lArg);
		QueryResult *rightBag = new QueryBagResult();
		rightBag->addResult(rArg);
		
		if (((leftBag->size()) == 0) || ((rightBag->size()) == 0)) {
			if (op == (AlgOpNode::bagMinus))
				((QueryBagResult *) final)->addResult(lArg);
		}
		else {
			((QueryBagResult *) leftBag)->sortBag();
			((QueryBagResult *) rightBag)->sortBag();

			QueryResult *left_elem;
			QueryResult *right_elem;
			
			while ((leftBag->size()) != 0) {
				errcode = ((QueryBagResult *) leftBag)->at(0, left_elem);
				if (errcode != 0) return errcode;
				
				if ((rightBag->size()) != 0) {
					errcode = ((QueryBagResult *) rightBag)->at(0, right_elem);
					if (errcode != 0) return errcode;
					
					if (left_elem->equal(right_elem)) {
						if (op == (AlgOpNode::bagIntersect))
							((QueryBagResult *) final)->addResult(left_elem);
						errcode = ((QueryBagResult *) leftBag)->getResult(left_elem);
						if (errcode != 0) return errcode;
						
						errcode = ((QueryBagResult *) rightBag)->getResult(right_elem);
						if (errcode != 0) return errcode;
					}
					else if (left_elem->less_than(right_elem)) {
						if (op == (AlgOpNode::bagMinus))
							((QueryBagResult *) final)->addResult(left_elem);
						
						errcode = ((QueryBagResult *) leftBag)->getResult(left_elem);	
						if (errcode != 0) return errcode;
					}
					else {
						errcode = ((QueryBagResult *) rightBag)->getResult(right_elem);	
						if (errcode != 0) return errcode;
					}
				}
				else {
					if (op == (AlgOpNode::bagMinus))
						((QueryBagResult *) final)->addResult(left_elem);
					errcode = ((QueryBagResult *) leftBag)->getResult(left_elem);
					if (errcode != 0) return errcode;
				}
			}
		}
		return 0;
	}
	else if (op == AlgOpNode::comma) {
		debug_print(*ec,  "[QE] COMMA operation");
		final = new QueryBagResult();
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		QueryResult *rBag = new QueryBagResult();
		rBag->addResult(rArg);
		QueryResult *lelem;
		QueryResult *relem;
		for (unsigned int i=0; i < lBag->size(); i++) {
			errcode = ((QueryBagResult *)lBag)->at(i, lelem);
			if (errcode != 0) return errcode;
			for (unsigned int j=0; j < rBag->size(); j++) {
				errcode = ((QueryBagResult *)rBag)->at(j, relem);
				if (errcode != 0) return errcode;
				QueryResult *act_struct = new QueryStructResult();
				act_struct->addResult(lelem);
				act_struct->addResult(relem);
				final->addResult(act_struct);
			}
		}
		return 0;
	}
	else if (op == AlgOpNode::semicolon) {
		debug_print(*ec,  "[QE] SEMICOLON operation");
		final = rArg;
		return 0;
	}
	else if ((op == AlgOpNode::insert) || (op == AlgOpNode::insert_viewproc)) {
		debug_print(*ec,  "[QE] INSERT operation");
		if (tn->isCoerced()) {
			debug_print(*ec,  "[QE][TC] INSERT operation is COERCED !");
			for (int i = 0; i < tn->nrOfCoerceActions(); i++) {
				int actId = tn->getCoerceAction(i);
				cout << actId << "- ";
				switch (actId) {
					case TypeCheck::DTable::CD_CAN_ASGN : cout << "insert: ASSIGN CHECK\n";
					case TypeCheck::DTable::CD_CAN_INS : cout << "insert: INSERT CHECK\n";
					case TypeCheck::DTable::CD_EXT_INS : cout << "insert: INSERT SHALLOW CD CHECK\n";
					default: break; //ignore, but i'm not sure it's ok
				}
			}
		}
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			debug_print(*ec,  "[QE] ERROR! Left argument of insert operation must have size 1");
			debug_print(*ec,  (ErrQExecutor | ERefExpected));
			return (ErrQExecutor | ERefExpected);
		}
		QueryResult* outer;
		errcode = ((QueryBagResult *) lBag)->at(0, outer);
		if (errcode != 0) return errcode;
		int leftResultType = outer->type();
		if (leftResultType == QueryResult::QVIRTUAL) {

			LogicalID *main_view_def = ((QueryVirtualResult*) outer)->view_defs.at(0);
			string main_vo_name = ((QueryVirtualResult*) outer)->vo_name;
			vector<QueryResult *> main_seeds = ((QueryVirtualResult*) outer)->seeds;


			QueryResult *rBag = new QueryBagResult();
			rBag->addResult(rArg);
			for (unsigned int i = 0; i < rBag->size(); i++) {
				debug_printf(*ec, "[QE] trying to INSERT %d element of rightArg into leftArg\n", i);
				QueryResult *toInsert;
				errcode = ((QueryBagResult *) rBag)->at(i, toInsert);
				if (errcode != 0) return errcode;

				while (toInsert->type() == QueryResult::QVIRTUAL) {
					LogicalID *view_def = ((QueryVirtualResult*) toInsert)->view_defs.at(0);
					vector<QueryResult *> seeds = ((QueryVirtualResult*) toInsert)->seeds;
					string proc_code = "";
					string proc_param = "";
					errcode = exViews->getOn_procedure(view_def, "on_store", proc_code, proc_param, tr);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
					if (proc_code == "") {
						debug_print(*ec,  "[QE] INSERT on_store - this VirtualObject doesn't have this operation defined");
						debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					errcode = exViews->createNewSections((QueryVirtualResult*) toInsert, NULL, NULL, envs_sections, tr, this);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}	

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *on_store_res;
					errcode = qres->pop(on_store_res);
					if (errcode != 0) return errcode;

					while (on_store_res->type() == QueryResult::QBAG) {
						if (on_store_res->size() != 1) {
							debug_print(*ec,  "[QE] insert operation: on_store procedure result is not single");
								debug_print(*ec,  (ErrQExecutor | EOtherResExp));
								return (ErrQExecutor | EOtherResExp);
							}
						QueryResult *curr;
						errcode = ((QueryBagResult *) on_store_res)->at(0, curr);
						on_store_res = curr;
					}
					toInsert = on_store_res;
				}

				if (toInsert->type() == QueryResult::QREFERENCE) {
					string proc_code = "";
					string proc_param = "";
					errcode = exViews->getOn_procedure(main_view_def, "on_insert", proc_code, proc_param, tr);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
					if (proc_code == "") {
						debug_print(*ec,  "[QE] operator <insert> - this VirtualObject doesn't have this operation defined");
						debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					QueryResult *param_binder = new QueryBinderResult(proc_param, toInsert);
					errcode = exViews->createNewSections((QueryVirtualResult*) outer, (QueryBinderResult*) param_binder, NULL, envs_sections, tr, this);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}	

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *res;
					errcode = qres->pop(res);
					if (errcode != 0) return errcode;
				}
				else if (toInsert->type() == QueryResult::QBINDER) {
					vector<LogicalID *> subviews;
					errcode = exViews->getSubviews(main_view_def, main_vo_name, subviews, tr);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

					string toInsert_name = ((QueryBinderResult *)toInsert)->getName();
					QueryResult *toInsert_value = ((QueryBinderResult *)toInsert)->getItem();

					LogicalID *sub_view_def = NULL;
					for (unsigned int j = 0; j < subviews.size(); j++ ) {
						LogicalID *subview_lid = subviews.at(j);
						string subview_name = "";
						string subview_code = "";
						errcode = exViews->checkViewAndGetVirtuals(subview_lid, subview_name, subview_code, tr);
						if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
						if (subview_name == toInsert_name) {
							if (sub_view_def != NULL){
								debug_print(*ec,  "[QE] ERROR! More then one subview defining virtual objects with same name");
								debug_print(*ec,  (ErrQExecutor | EBadViewDef));
								return (ErrQExecutor | EBadViewDef);
							}
							else {
								sub_view_def = subview_lid;
								continue;
							}
							delete subview_lid;
						}
					}

					if (sub_view_def == NULL) {
						debug_print(*ec,  "[QE] operator <insert into> - this virtualObject doesn't have this operation defined, subview missing");
						debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
						return (ErrQExecutor | EOperNotDefined);
					}
					string proc_code = "";
					string proc_param = "";
					errcode = exViews->getOn_procedure(sub_view_def, "on_create", proc_code, proc_param, tr);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
					if (proc_code == "") {
						debug_print(*ec,  "[QE] operator <insert into> - this VirtualObject doesn't have this operation defined");
						debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					QueryResult *param_binder = new QueryBinderResult(proc_param, toInsert_value);
					errcode = exViews->createNewSections((QueryVirtualResult*) outer, (QueryBinderResult*) param_binder, sub_view_def, envs_sections, tr, this);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}	

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *res;
					errcode = qres->pop(res);
					if (errcode != 0) return errcode;
				}
				else {
					debug_print(*ec,  "[QE] Right argument of insert operation must consist of QREFERENCE or QBINDER");
					debug_print(*ec,  (ErrQExecutor | ERefExpected));
					return (ErrQExecutor | ERefExpected);
				}
			}
		}
		else if (leftResultType == QueryResult::QREFERENCE) {
			LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
			ObjectPointer *optrOut;
			errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] insert operation - Error in getObjectPointer.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string outer_name = optrOut->getName();
			if (!am->hasAccess(UPDATE, outer_name))
			{
				debug_printf(*ec, "[QE] user has no update access for %s objects", outer_name.c_str());
				final = new QueryNothingResult();
				return 0;
			}
			debug_print(*ec,  "[QE] insert operation - getObjectPointer on leftArg done");
			DataValue* db_value = optrOut->getValue();
			ExtendedType optrOut_extType = db_value->getSubtype();
			debug_print(*ec,  "[QE] insert operation - Value taken");
			int vType = db_value->getType();
			if (vType != Store::Vector) {
				debug_print(*ec,  "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector");
				debug_print(*ec,  (ErrQExecutor | EOtherResExp));
				return (ErrQExecutor | EOtherResExp);
			}
			vector<LogicalID*> *insVector = db_value->getVector();
			if (insVector == NULL) {
				debug_print(*ec,  "[QE] insert operation - insVector == NULL");
				debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			debug_print(*ec,  "[QE] Vector taken");
			debug_printf(*ec, "[QE] Vec.size = %d\n", insVector->size());

			//----------	TC dynamic coerce input 	---------- //
			bool needsExtCoerce = tn->needsCoerce(TypeCheck::DTable::CD_EXT_INS);
			bool needsDeepCardCheck = tn->needsCoerce(TypeCheck::DTable::CD_CAN_INS);
			map<string, int> subCardMap;
			if (needsExtCoerce) {
				debug_print(*ec,  "[QE][TC] Fill maps that help check card constraints are met on inserted objects.");
				for (unsigned int j = 0;  j < insVector->size(); j++) {
					ObjectPointer *subPtr;
					errcode = tr->getObjectPointer (insVector->at(j), Store::Read, subPtr, true);
					if (errcode != 0) {
						debug_print(*ec,  "[QE] insert operation - ext coerce - Error in getObjectPointer.");
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					if (subPtr == NULL) break;
					string sName = subPtr->getName();
					if (subCardMap.find(sName) == subCardMap.end())
						subCardMap[sName] = 0;
					subCardMap[sName]++;
					cout << "---" << sName << ": " << subCardMap[sName] << "\n";
				}
			}
			//-------------------------------------------------------//

			QueryResult *rBag = new QueryBagResult();
			rBag->addResult(rArg);
			vector<LogicalID*> toInsVector;
			for (unsigned int i = 0; i < rBag->size(); i++) {
				debug_printf(*ec, "[QE] trying to INSERT %d element of rightArg into leftArg\n", i);
				QueryResult *toInsert;
				errcode = ((QueryBagResult *) rBag)->at(i, toInsert);
				if (errcode != 0) return errcode;
				ObjectPointer *optrIn;
				ObjectPointer *checkIn;
				LogicalID *lidIn;

				while (toInsert->type() == QueryResult::QVIRTUAL) {
					LogicalID *view_def = ((QueryVirtualResult*) toInsert)->view_defs.at(0);
					vector<QueryResult *> seeds = ((QueryVirtualResult*) toInsert)->seeds;
					string proc_code = "";
					string proc_param = "";
					errcode = exViews->getOn_procedure(view_def, "on_store", proc_code, proc_param, tr);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
					if (proc_code == "") {
						debug_print(*ec,  "[QE] INSERT on_store - this VirtualObject doesn't have this operation defined");
						debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					errcode = exViews->createNewSections((QueryVirtualResult*) toInsert, NULL, NULL, envs_sections, tr, this);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}	

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *on_store_res;
					errcode = qres->pop(on_store_res);
					if (errcode != 0) return errcode;

					while (on_store_res->type() == QueryResult::QBAG) {
						if (on_store_res->size() != 1) {
							debug_print(*ec,  "[QE] insert operation: on_store procedure result is not single");
								debug_print(*ec,  (ErrQExecutor | EOtherResExp));
								return (ErrQExecutor | EOtherResExp);
							}
						QueryResult *curr;
						errcode = ((QueryBagResult *) on_store_res)->at(0, curr);
						on_store_res = curr;
					}
					toInsert = on_store_res;
				}

				int rightResultType = toInsert->type();
				string object_name = "";
				switch (rightResultType) {
					case QueryResult::QREFERENCE: {
						lidIn = ((QueryReferenceResult *) toInsert)->getValue();
						errcode = tr->getObjectPointer (lidIn, Store::Read, checkIn, true);
						if (errcode != 0) {
							debug_print(*ec,  "[QE] insert operation - Error in getObjectPointer.");
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						object_name = checkIn->getName();
						//Check external card constraints on inserted object
						if (needsExtCoerce) {
							debug_print(*ec,  "[QE][TC] insert:ref:: checking that no '"+object_name+"' here yet.");
							if (subCardMap.find(object_name) == subCardMap.end())
								subCardMap[object_name] = 0;
							int subNum = (++subCardMap[object_name]);
							if (subNum > 1) {
								debug_print(*ec,  "[QE][TC] insert:ref:: coerce ERROR - card of inserted object exceeded.");
								debug_print(*ec,  (ErrQExecutor | ECrcInsExtTooMany));
								return (ErrQExecutor | ECrcInsExtTooMany);
							}
						}
						//Check internal cards constraints (for subobjects)
						if (needsDeepCardCheck) {
							string txt = (optrIn == NULL ? " NULL!" : "NOT null...");
							debug_print(*ec,  "optr is " + txt);
							txt = (tn->getCoerceSig() == NULL ? " NULL!" : "NOT null...");
							debug_print(*ec,  "coerceSig is " + txt);
							errcode = sigs.checkSubCardsRec(tn->getCoerceSig(), checkIn, tr);
							if (errcode != 0) 
							{
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
						}

						errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn, true);
						if (errcode != 0) {
							debug_print(*ec,  "[QE] insert operation - Error in getObjectPointer.");
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						if (optrIn == NULL) {
							debug_print(*ec,  "[QE] ERROR in insert operation - objectPointer is NULL");
							debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
							return (ErrQExecutor | EQEUnexpectedErr);
						}
						object_name = optrIn->getName();
						if(ClassGraph::isExtName( object_name )) {
							/*
							* Dobrze by bylo zaznaczac w obiekcie, ze jest statyczna skladowa,
							* a nie przekonywac sie o tym na podstawie postaci nazwy.
							*/
							return 1;//TODO (sk) Cant move static class member from root.
						}

						debug_printf(*ec, "[QE] getObjectPointer on %d element of rightArg done\n", i);
						
						if ((((optrIn->getValue())->getType()) == Store::Vector) && (((optrIn->getValue())->getSubtype()) == Store::View)) {
							string virtual_objects_name = "";
							vector<LogicalID*>* view_inner_vec = (optrIn->getValue())->getVector();
							
							for (unsigned int k=0; k < view_inner_vec->size(); k++) {
								ObjectPointer *view_inner_optr;
								errcode = tr->getObjectPointer (view_inner_vec->at(k), Store::Read, view_inner_optr, false);
								if (errcode != 0) {
									debug_print(*ec,  "[QE] insert view operation - Error in getObjectPointer.");
									antyStarveFunction(errcode);
									inTransaction = false;
									return errcode;
								}
								if (view_inner_optr->getName() == QE_VIEWDEF_VIRTUALOBJECTS_NAME) {
									virtual_objects_name = (view_inner_optr->getValue())->getString();
									break;
								}
							}
							
							errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut, false); 
							if (errcode != 0) {
								debug_print(*ec,  "[QE] insert operation - Error in getObjectPointer.");
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							db_value = optrOut->getValue();
							optrOut_extType = db_value->getSubtype();
							vType = db_value->getType();
							if (vType != Store::Vector) {
								debug_print(*ec,  "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector");
								debug_print(*ec,  (ErrQExecutor | EOtherResExp));
								return (ErrQExecutor | EOtherResExp);
							}
							insVector = db_value->getVector();
							if (insVector == NULL) {
								debug_print(*ec,  "[QE] insert operation - insVector == NULL");
								debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
								return (ErrQExecutor | EQEUnexpectedErr);
							}
							
							vector<LogicalID*> subobjects_to_virtualize;
							for (unsigned int k=0; k < insVector->size(); k++) {
								ObjectPointer *to_virtualize_optr;
								errcode = tr->getObjectPointer (insVector->at(k), Store::Read, to_virtualize_optr, false);
								if (errcode != 0) {
									debug_print(*ec,  "[QE] insert view operation - Error in getObjectPointer.");
									antyStarveFunction(errcode);
									inTransaction = false;
									return errcode;
								}
								if (to_virtualize_optr->getName() == virtual_objects_name) {
									subobjects_to_virtualize.push_back(insVector->at(k));;
								}
							}
							
							if (subobjects_to_virtualize.size() > 0) {
								debug_print(*ec,  "[QE] WARNING, subobjects with the same name as new virtual objects found. These objects will be transformed into virtuals.");
								string proc_code = "";
								string proc_param = "";
								errcode = exViews->getOn_procedure(lidIn, "on_create", proc_code, proc_param, tr);
								if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
								if (proc_code == "") {
									debug_print(*ec,  "[QE] insert view <create operation> - this View doesn't have this operation defined");
									
									errcode = persistDelete(lidIn);
									if(errcode != 0) return errcode;
									
									debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
									return (ErrQExecutor | EOperNotDefined);
								}
								
								for (unsigned int k=0; k < subobjects_to_virtualize.size(); k++) {
									LogicalID *curr_subobject = subobjects_to_virtualize.at(k);
									QueryResult *curr_subobject_before_deref = new QueryReferenceResult(curr_subobject);
									QueryResult *curr_subobject_after_deref;
									errcode = this->derefQuery(curr_subobject_before_deref, curr_subobject_after_deref);
									if (errcode != 0) {
										return errcode;
									}
									QueryResult *param_binder = new QueryBinderResult(proc_param, curr_subobject_after_deref);
									
									vector<QueryBagResult*> envs_sections;
									QueryResult *lidOut_tmp_qr = new QueryReferenceResult(lidOut);
									QueryBinderResult *viewParentBinder = new QueryBinderResult(QE_NOTROOT_VIEW_PARENT_NAME, lidOut_tmp_qr);
									QueryBagResult *viewParentBag = new QueryBagResult();
									viewParentBag->addResult(viewParentBinder);
									envs_sections.push_back(viewParentBag);
									
									errcode = exViews->createNewSections(NULL, (QueryBinderResult*) param_binder, lidIn, envs_sections, tr, this);
									if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
									
									errcode = callProcedure(proc_code, envs_sections);
									if(errcode != 0) return errcode;
									
									QueryResult *tmp_res;
									errcode = qres->pop(tmp_res);
									if (errcode != 0) return errcode;
									
									errcode = persistDelete(curr_subobject);
									if(errcode != 0) return errcode;
								}
							}
						}
						
						if (optrIn->getIsRoot()) {
							if ((((optrIn->getValue())->getType()) == Store::Vector) && (((optrIn->getValue())->getSubtype()) == Store::View)) {
								errcode = tr->removeView(optrIn);
								if (errcode != 0) {
									debug_print(*ec,  "[QE] insert operation - Error in removeView.");
									antyStarveFunction(errcode);
									inTransaction = false;
									return errcode;
								}
							}

							errcode = tr->removeRoot(optrIn);
							if (errcode != 0) {
								debug_print(*ec,  "[QE] insert operation - Error in removeRoot.");
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							debug_print(*ec,  "[QE] insert operation - Root removed");
						}
						else if (op != AlgOpNode::insert_viewproc) {
							debug_print(*ec,  "[QE] ERROR in insert operation - right argument must be Root or a new object (or procedure, view)");
							debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
							return (ErrQExecutor | EQEUnexpectedErr);
						}
						toInsVector.push_back(lidIn);
						break;
					}
					case QueryResult::QBINDER: {
						object_name = ((QueryBinderResult *)toInsert)->getName();
						LogicalID *maybe_sub_view_def = NULL;

						if (needsExtCoerce) {
							debug_print(*ec,  "[QE][TC] insert:binder:: checking that no '"+object_name+"' here yet.");
							if (subCardMap.find(object_name) == subCardMap.end())
								subCardMap[object_name] = 0;
							int subNum = (++subCardMap[object_name]);
							if (subNum > 1) {
								debug_print(*ec,  "[QE][TC] insert:binder:: coerce ERROR- card of inserted object exceeded.");
								debug_print(*ec,  (ErrQExecutor | ECrcInsExtTooMany));
								return (ErrQExecutor | ECrcInsExtTooMany);
							}
						}
						if (needsDeepCardCheck) {
							int cum;
							errcode = sigs.checkSubCardsRec(tn->getCoerceSig(), toInsert, true, cum);
							if (errcode != 0) {
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
						}

						//TODO powinna byc mozliwosc zapisania w objectPointerze ze zawiera jako podobiekty perspektywy
						//ObjectPointer powinien miec funkcje nazwa->LID perspektywy definiujacej obiekty o takiej nazwie
						//cos na ksztalt getViews(name), wtedy ponizszy kawalek kodu moznaby uspawnic znacznie
						int insVector_size = insVector->size();
						for (int j = 0; j < insVector_size; j++ ) {
							LogicalID *maybe_subview_logID = insVector->at(j);
							ObjectPointer *maybe_subview_optr;
							if ((errcode = tr->getObjectPointer(maybe_subview_logID, Store::Read, maybe_subview_optr, false)) != 0) {
								debug_print(*ec,  "[QE] INSERT operator - Error in getObjectPointer");
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							DataValue* maybe_subview_value = maybe_subview_optr->getValue();
							int maybe_subview_vType = maybe_subview_value->getType();
							ExtendedType maybe_subview_extType = maybe_subview_value->getSubtype();
							if ((maybe_subview_vType == Store::Vector) && (maybe_subview_extType == Store::View)) {
								string maybe_subview_name = "";
								string maybe_subview_code = "";
								errcode = exViews->checkViewAndGetVirtuals(maybe_subview_logID, maybe_subview_name, maybe_subview_code, tr);
								if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
								if (maybe_subview_name == object_name) {
									if (maybe_sub_view_def != NULL){
										debug_print(*ec,  "[QE] ERROR! More then one subview defining virtual objects with same name");
										debug_print(*ec,  (ErrQExecutor | EBadViewDef));
										return (ErrQExecutor | EBadViewDef);
									}
									else maybe_sub_view_def = maybe_subview_logID;
								}
							}
						}
						if (maybe_sub_view_def == NULL) {
							errcode = this->objectFromBinder(toInsert, optrIn);
							if (errcode != 0) return errcode;
							lidIn = (optrIn->getLogicalID());
							toInsVector.push_back(lidIn);
						}
						else {
							string proc_code = "";
							string proc_param = "";
							errcode = exViews->getOn_procedure(maybe_sub_view_def, "on_create", proc_code, proc_param, tr);
							if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
							if (proc_code == "") {
								debug_print(*ec,  "[QE] operator <insert into> - this VirtualObject doesn't have this operation defined");
								debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
								return (ErrQExecutor | EOperNotDefined);
							}

							vector<QueryBagResult*> envs_sections;

							QueryResult *tmp_result_1 = new QueryReferenceResult(lidOut);
							QueryResult *nested_tmp_result_1;
							vector<DataValue*> _tmp_dataVal_vec;
							errcode = tmp_result_1->nested(this, tr, nested_tmp_result_1, _tmp_dataVal_vec);
							if(errcode != 0) return errcode;
							envs_sections.push_back((QueryBagResult *) nested_tmp_result_1);

							QueryResult *toInsert_value = ((QueryBinderResult *)toInsert)->getItem();
							QueryResult *param_binder = new QueryBinderResult(proc_param, toInsert_value);
							errcode = exViews->createNewSections(NULL, (QueryBinderResult*) param_binder, maybe_sub_view_def, envs_sections, tr, this);
							if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

							errcode = callProcedure(proc_code, envs_sections);
							if(errcode != 0) return errcode;

							QueryResult *maybe_sub_view_res;
							errcode = qres->pop(maybe_sub_view_res);
							if (errcode != 0) return errcode;
						}
						break;
					}
					default: {
						debug_print(*ec,  "[QE] Right argument of insert operation must consist of QREFERENCE or QBINDER or QVIRTUAL");
						debug_print(*ec,  (ErrQExecutor | ERefExpected));
						return (ErrQExecutor | ERefExpected);
					}
				}
			}

			errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] insert operation - Error in getObjectPointer.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			db_value = optrOut->getValue();
			optrOut_extType = db_value->getSubtype();
			vType = db_value->getType();
			if (vType != Store::Vector) {
				debug_print(*ec,  "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector");
				debug_print(*ec,  (ErrQExecutor | EOtherResExp));
				return (ErrQExecutor | EOtherResExp);
			}
			insVector = db_value->getVector();
			if (insVector == NULL) {
				debug_print(*ec,  "[QE] insert operation - insVector == NULL");
				debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
				return (ErrQExecutor | EQEUnexpectedErr);
			}

			for (unsigned int i = 0; i < toInsVector.size(); i++) {
				debug_print(*ec,  "[QE] Inserting the object");
				insVector->push_back(toInsVector.at(i));
				debug_printf(*ec, "[QE] New Vec.size = %d\n", insVector->size());
			}
			StoreManager *sm = StoreManager::theStore;
			DataValue *dbDataVal = sm->createVectorValue(insVector);
			dbDataVal->setClassMarks(db_value->getClassMarks());
			dbDataVal->setSubclasses(db_value->getSubclasses());
			debug_print(*ec,  "[QE] dataValue: setVector done");
			db_value = dbDataVal;
			errcode = tr->modifyObject(optrOut, db_value);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] insert operation - Error in modifyObject.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			(optrOut->getValue())->setSubtype(optrOut_extType);
			errcode = tr->modifyObject(optrOut, optrOut->getValue());
			if (errcode != 0) {
				debug_print(*ec,  "[QE] insert operation - Error in modifyObject.");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			debug_print(*ec,  "[QE] insert operation: value of the object was changed");
		}
		else {
			debug_print(*ec,  "[QE] ERROR! Left argument of insert operation must consist of QREFERENCE");
			debug_print(*ec,  (ErrQExecutor | ERefExpected));
			return (ErrQExecutor | ERefExpected);
		}
		debug_print(*ec,  "[QE] INSERT operation Done");
		final = new QueryNothingResult();
		return 0;
	}
	else if ((op == AlgOpNode::assign) || (op == AlgOpNode::assign_viewproc)) {
		debug_print(*ec,  "[QE] ASSIGN operation");
		if (tn->isCoerced()) {
			debug_print(*ec,  "[QE][TC] ASSIGN operation is COERCED !");
			for (int i = 0; i < tn->nrOfCoerceActions(); i++) {
				int actId = tn->getCoerceAction(i);
				cout << actId << "- ";
				switch (actId) {
					case TypeCheck::DTable::CD_CAN_ASGN : cout << "ASSIGN CHECK\n";
					case TypeCheck::DTable::CD_CAN_INS : cout << "INSERT CHECK\n";
					case TypeCheck::DTable::CD_EXT_INS : cout << "INSERT SHALLOW CD CHECK\n";
					default: break; //ignore, but i'm not sure it's ok
				}
			}
		}
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			debug_print(*ec,  "[QE] ERROR! operator <assign> expects that left argument is single REFERENCE");
			debug_print(*ec,  (ErrQExecutor | ERefExpected));
			return (ErrQExecutor | ERefExpected);
		}
		errcode = ((QueryBagResult *) lBag)->at(0, lArg);
		if (errcode != 0) return errcode;
		int leftResultType = lArg->type();
		if (leftResultType == QueryResult::QVIRTUAL) {
			LogicalID *view_def = ((QueryVirtualResult*) lArg)->view_defs.at(0);
			vector<QueryResult *> seeds = ((QueryVirtualResult*) lArg)->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = exViews->getOn_procedure(view_def, "on_update", proc_code, proc_param, tr);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
			if (proc_code == "") {
				debug_print(*ec,  "[QE] operator <assign> - this VirtualObject doesn't have this operation defined");
				debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			QueryResult *param_binder = new QueryBinderResult(proc_param, rArg);
			errcode = exViews->createNewSections((QueryVirtualResult*) lArg, (QueryBinderResult*) param_binder, NULL, envs_sections, tr, this);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;
			QueryResult *res;
			errcode = qres->pop(res);
			if (errcode != 0) return errcode;
		}
		else if (leftResultType == QueryResult::QREFERENCE) {
			LogicalID *old_lid = ((QueryReferenceResult *) lArg)->getValue();
			ObjectPointer *old_optr;
			errcode = tr->getObjectPointer (old_lid, Store::Write, old_optr, false);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] operator <assign>: error in getObjectPointer()");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string object_name = old_optr->getName();
			if (!am->hasAccess(UPDATE, object_name))
			{
				debug_printf(*ec,  "[QE] user has no update right on %s objects to assign", object_name.c_str());
				final = new QueryNothingResult();
				return 0;
			}
			debug_print(*ec,  "[QE] operator <assign>: getObjectPointer on left argument done");
			if ((old_optr->getIsRoot()) && ((old_optr->getValue())->getSubtype() == Store::View)) {
				errcode = tr->removeView(old_optr);
				if (errcode != 0) {
					debug_print(*ec,  "[QE] Error in removeView.");
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
			}
			StoreManager *sm = StoreManager::theStore;
			DataValue *dbValue = sm->createIntValue(0);
			if (op == AlgOpNode::assign) {
				QueryResult *derefed;
				errcode = this->derefQuery(rArg, derefed);
				if (errcode != 0) return errcode;
				while (derefed->type() == QueryResult::QVIRTUAL) {
					LogicalID *view_def = ((QueryVirtualResult*) derefed)->view_defs.at(0);
					vector<QueryResult *> seeds = ((QueryVirtualResult*) derefed)->seeds;
					string proc_code = "";
					string proc_param = "";
					errcode = exViews->getOn_procedure(view_def, "on_store", proc_code, proc_param, tr);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
					if (proc_code == "") {
						debug_print(*ec,  "[QE] UPDATE on_store - this VirtualObject doesn't have this operation defined");
						debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					errcode = exViews->createNewSections((QueryVirtualResult*) derefed, NULL, NULL, envs_sections, tr, this);
					if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *on_store_res;
					errcode = qres->pop(on_store_res);
					if (errcode != 0) return errcode;

					while (on_store_res->type() == QueryResult::QBAG) {
						if (on_store_res->size() != 1) {
							debug_print(*ec,  "[QE] update operation: on_store procedure result is not single");
								debug_print(*ec,  (ErrQExecutor | EOtherResExp));
								return (ErrQExecutor | EOtherResExp);
							}
						QueryResult *curr;
						errcode = ((QueryBagResult *) on_store_res)->at(0, curr);
						on_store_res = curr;
					}
					derefed = on_store_res;
					debug_print(*ec,  "[QE] < query := ('virtual')); > operation");
				}
				int derefed_type = derefed->type();
				//TC dynamic coerce... might need only if derefed_type is struct or binder...
				bool needsDeepCardCheck = tn->needsCoerce(TypeCheck::DTable::CD_CAN_ASGN);
				switch (derefed_type) {
					case QueryResult::QINT: {
						int intValue = ((QueryIntResult *) derefed)->getValue();
						dbValue->setInt(intValue);
						debug_print(*ec,  "[QE] < query := ('integer')); > operation");
						break;
					}
					case QueryResult::QDOUBLE: {
						double doubleValue = ((QueryDoubleResult *) derefed)->getValue();
						dbValue->setDouble(doubleValue);
						debug_print(*ec,  "[QE] < query := ('double')); > operation");
						break;
					}
					case QueryResult::QSTRING: {
						string stringValue = ((QueryStringResult *) derefed)->getValue();
						dbValue->setString(stringValue);
						debug_print(*ec,  "[QE] < query := ('string')); > operation");
						break;
					}
					case QueryResult::QREFERENCE: {
						LogicalID* refValue = ((QueryReferenceResult *) derefed)->getValue();
						dbValue->setPointer(refValue);
						debug_print(*ec,  "[QE] < query := ('reference')); > operation");
						break;
					}
					case QueryResult::QSTRUCT: {
						if (needsDeepCardCheck) {
							int cum;
							errcode = sigs.checkSubCardsRec(tn->getCoerceSig(), derefed, false, cum);
							if (errcode != 0) {
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
						}
						vector<LogicalID*> vectorValue;
						for (unsigned int i=0; i < (derefed->size()); i++) {
							QueryResult *tmp_res;
							errcode = ((QueryStructResult *) derefed)->at(i, tmp_res);
							if (errcode != 0) return errcode;
							ObjectPointer *optr_tmp;
							errcode = this->objectFromBinder(tmp_res, optr_tmp);
							if (errcode != 0) return errcode;
							LogicalID *lid_tmp = optr_tmp->getLogicalID();
							vectorValue.push_back(lid_tmp);
						}
						dbValue->setVector(&vectorValue);
						debug_print(*ec,  "[QE] < query := ('struct')); > operation");
						break;
					}
					case QueryResult::QBINDER: {
						if (needsDeepCardCheck) {
							int cum;
							errcode = sigs.checkSubCardsRec(tn->getCoerceSig(), derefed, true, cum);
							if (errcode != 0) {
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
						}
						vector<LogicalID*> vectorValue;
						ObjectPointer *optr_tmp;
						errcode = this->objectFromBinder(derefed, optr_tmp);
						if (errcode != 0) return errcode;
						LogicalID *lid_tmp = optr_tmp->getLogicalID();
						vectorValue.push_back(lid_tmp);
						dbValue->setVector(&vectorValue);
						debug_print(*ec,  "[QE] < query := ('binder')); > operation");
						break;
					}
					default: {
						debug_print(*ec,  "[QE] operator <assign>: error, bad type right argument evaluated by executeRecQuery");
						debug_print(*ec,  (ErrQExecutor | EOtherResExp));
						return (ErrQExecutor | EOtherResExp);
					}
				}
			}

			ExtendedType rArg_extType = Store::None;
			if (rArg->isReferenceValue()) {
				QueryResult *rArg_ref;
				errcode = rArg->getReferenceValue(rArg_ref);
				if (errcode != 0) return errcode;
				LogicalID *rArg_lid = ((QueryReferenceResult *)rArg_ref)->getValue();
				ObjectPointer *rArg_optr;
				if (op == AlgOpNode::assign_viewproc)
					errcode = tr->getObjectPointer (rArg_lid, Store::Write, rArg_optr, false);
				else errcode = tr->getObjectPointer (rArg_lid, Store::Read, rArg_optr, false);
				if (errcode != 0) {
					debug_print(*ec,  "[QE] operator <assign>: error in getObjectPointer()");
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				rArg_extType = (rArg_optr->getValue())->getSubtype();

				if (op == AlgOpNode::assign_viewproc) {
					string rArg_name = rArg_optr->getName();
					if (object_name != rArg_name) {
						debug_print(*ec,  "[QE] operator <assign>: ERROR left and right arguments have different names");
						debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
						return (ErrQExecutor | EQEUnexpectedErr);
					}

					vector<LogicalID*>* inner_vec = (rArg_optr->getValue())->getVector();
					if ((old_optr->getIsRoot()) && (rArg_extType == Store::View)) {
						string virtual_objects_name = "";
						for (unsigned int i=0; i < inner_vec->size(); i++) {
							ObjectPointer *inner_optr;
							errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
							if (errcode != 0) {
								debug_print(*ec,  "[QE] register view operation - Error in getObjectPointer.");
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							if (inner_optr->getName() == QE_VIEWDEF_VIRTUALOBJECTS_NAME) {
								virtual_objects_name = (inner_optr->getValue())->getString();
								break;
							}
						}
						errcode = tr->addView(virtual_objects_name.c_str(), old_optr);
						if (errcode != 0) {
							debug_print(*ec,  "[QE] Error in addView");
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
					}

					dbValue->setVector(inner_vec);
					errcode = tr->deleteObject(rArg_optr);
					if (errcode != 0) {
						debug_print(*ec,  "[QE] Error in deleteObject.");
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
				}
				else if ((rArg_extType == Store::Procedure) || (rArg_extType == Store::View)) {
					rArg_extType = Store::None;
					string rArg_name = rArg_optr->getName();
					if (object_name != rArg_name) debug_print(*ec,  "[QE] operator <assign>: warning left and right arguments have different names");
				}
			}
			DataValue* value = dbValue;
			errcode = tr->modifyObject(old_optr, value);
			if (errcode != 0) {
				debug_print(*ec,  "[QE] operator <assign>: error in modifyObject()");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			(old_optr->getValue())->setSubtype(rArg_extType);
			errcode = tr->modifyObject(old_optr, old_optr->getValue());
			if (errcode != 0) {
				debug_print(*ec,  "[QE] operator <assign>: error in modifyObject()");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			debug_print(*ec,  "[QE] operator <assign>: value of the object was changed");
		}
		else {
			debug_print(*ec,  "[QE] ERROR! operator <assign> expects that left argument is REFERENCE");
			debug_print(*ec,  (ErrQExecutor | ERefExpected));
			return (ErrQExecutor | ERefExpected);
		}
		debug_print(*ec,  "[QE] ASSIGN operation Done");
		final = new QueryNothingResult();
		return 0;
	}
	else { // algOperation type not known
		debug_print(*ec,  "[QE] ERROR! Algebraic operation type not known");
		debug_print(*ec,  (ErrQExecutor | EUnknownNode));
		return (ErrQExecutor | EUnknownNode);
	}
	return 0;
}

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *rRes, QueryResult *&partial) {
	debug_print(*ec,  "[QE] combine() function applied to the partial results");
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <dot>");
			partial->addResult(rRes);
			break;
		}
		case NonAlgOpNode::join: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <join>");
			QueryResult *lBag = new QueryBagResult();
			lBag->addResult(curr);
			QueryResult *rBag = new QueryBagResult();
			rBag->addResult(rRes);
			for (unsigned int i=0; i < lBag->size(); i++) {
				QueryResult *lelem;
				errcode = ((QueryBagResult *)lBag)->at(i, lelem);
				if (errcode != 0) return errcode;
				for (unsigned int j=0; j < rBag->size(); j++) {
					QueryResult *relem;
					errcode = ((QueryBagResult *)rBag)->at(j, relem);
					if (errcode != 0) return errcode;
					QueryResult *act_struct = new QueryStructResult();
					act_struct->addResult(lelem);
					act_struct->addResult(relem);
					partial->addResult(act_struct);
				}
			}
			break;
		}
		case NonAlgOpNode::where: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <where>");
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				if (tmp_bool_value)
					partial->addResult(curr);
			}
			else {
				debug_print(*ec,  "[QE] combine(): ERROR! Right argument of operator <where> is not boolean!");
				debug_print(*ec,  (ErrQExecutor | EBoolExpected));
				return (ErrQExecutor | EBoolExpected);
			}
			break;
		}
		case NonAlgOpNode::orderBy: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <orderBy>");
			QueryResult *derefR;
			errcode = this->derefQuery(rRes,derefR);
			if (errcode != 0) return errcode;
			QueryResult *currRow = new QueryStructResult(curr, derefR);
			partial->addResult(currRow);
			break;
		}
		case NonAlgOpNode::exists: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <exists>");
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value);
				partial->addResult(tmp_result);
			}
			else {
				debug_print(*ec,  "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!");
				debug_print(*ec,  (ErrQExecutor | EBoolExpected));
				return (ErrQExecutor | EBoolExpected);
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <forAll>");
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value);
				partial->addResult(tmp_result);
			}
			else {
				debug_print(*ec,  "[QE] combine(): ERROR! Right argument of operator <forAll> is not boolean!");
				debug_print(*ec,  (ErrQExecutor | EBoolExpected));
				return (ErrQExecutor | EBoolExpected);
			}
			break;
		}
		case NonAlgOpNode::forEach: {
			debug_print(*ec,  "[QE] combine(): NonAlgebraic operator <forEach>");
			break;
		}
		default: {
			debug_print(*ec,  "[QE] combine(): unknown NonAlgebraic operator!");
			debug_print(*ec,  (ErrQExecutor | EUnknownNode));
			return (ErrQExecutor | EUnknownNode);
			break;
		}
	}
	return 0;
}



int QueryExecutor::abort() {
	debug_print(*ec,  "[QE] abort()");
	if (inTransaction) {
		int errcode = tr->abort();
		inTransaction = false;
		if (errcode != 0) {
			debug_print(*ec,  "[QE] Error in abort()");
			return errcode;
		}
		debug_print(*ec,  "[QE] Transaction aborted succesfully. Done!");
	}
	else
		debug_print(*ec,  "[QE] Transaction not opened. Closing nevertheless. Done!\n");
	return 0;
}

void QueryExecutor::antyStarveFunction(int errcode) {
	if (errcode == (ErrTManager | EDeadlock)) {
		antyStarve = true;
		debug_print(*ec,  "[QE] EDeadlock error found => AntyStarve mode is on");
	}
}

int QueryExecutor::objectFromBinder(QueryResult *res, ObjectPointer *&newObject) {
	int errcode;
	debug_print(*ec,  "[QE] objectFromBinder()");
	debug_printf(*ec, "[QE] QueryResult type: %d\n", res->type());
	if ((res->type()) != QueryResult::QBINDER) {
		debug_printf(*ec, "[QE] objectFromBinder() expected a binder, got something else: %d\n", res->type());
		debug_print(*ec,  (ErrQExecutor | EOtherResExp));
		return (ErrQExecutor | EOtherResExp);
	}
	string binderName = ((QueryBinderResult *) res)->getName();
	QueryResult *binderItem = ((QueryBinderResult *) res)->getItem();
	StoreManager *sm = StoreManager::theStore;
	DataValue *dbValue = sm->createIntValue(0);
	int binderItemType = binderItem->type();
	switch (binderItemType) {
		case QueryResult::QINT: {
			int intValue = ((QueryIntResult *) binderItem)->getValue();
			dbValue->setInt(intValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is INT type");
			break;
		}
		case QueryResult::QDOUBLE: {
			double doubleValue = ((QueryDoubleResult *) binderItem)->getValue();
			dbValue->setDouble(doubleValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is DOUBLE type");
			break;
		}
		case QueryResult::QSTRING: {
			string stringValue = ((QueryStringResult *) binderItem)->getValue();
			dbValue->setString(stringValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is STRING type");
			break;
		}
		case QueryResult::QREFERENCE: {
			LogicalID* refValue = ((QueryReferenceResult *) binderItem)->getValue();
			dbValue->setPointer(refValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is REFERENCE type");
			break;
		}
		case QueryResult::QBINDER: {
			vector<LogicalID*> vectorValue;
			ObjectPointer *optr;
			errcode = this->objectFromBinder(binderItem, optr);
			if (errcode != 0) return errcode;
			LogicalID *lid = optr->getLogicalID();
			debug_print(*ec,  "[QE] objectFromBinder(): logical ID received");
			vectorValue.push_back(lid);
			debug_print(*ec,  "[QE] objectFromBinder(): added to a vector");
			dbValue->setVector(&vectorValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is BINDER type");
			break;
		}
		case QueryResult::QBAG: {
			vector<LogicalID*> vectorValue;
			for (unsigned int i=0; i < (binderItem->size()); i++) {
				QueryResult *curr;
				errcode = ((QueryBagResult *) binderItem)->at(i, curr);
				if (errcode != 0) return errcode;
				if (curr->type() != QueryResult::QREFERENCE) {
					debug_print(*ec,  "[QE] objectFromBinder() QueryReference expected");
					debug_print(*ec,  (ErrQExecutor | ERefExpected));
					return (ErrQExecutor | ERefExpected);
				}
				LogicalID *lid = ((QueryReferenceResult *) curr)->getValue();
				debug_print(*ec,  "[QE] objectFromBinder(): logical ID received");
				vectorValue.push_back(lid);
				debug_print(*ec,  "[QE] objectFromBinder(): added to a vector");
			}
			dbValue->setVector(&vectorValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is BAG type");
			break;
		}
		case QueryResult::QSTRUCT: {
			vector<LogicalID*> vectorValue;
			for (unsigned int i=0; i < (binderItem->size()); i++) {
				QueryResult *curr;
				errcode = ((QueryStructResult *) binderItem)->at(i, curr);
				if (errcode != 0) return errcode;
				ObjectPointer *optr;
				errcode = this->objectFromBinder(curr, optr);
				if (errcode != 0) return errcode;
				LogicalID *lid = optr->getLogicalID();
				debug_print(*ec,  "[QE] objectFromBinder(): logical ID received");
				vectorValue.push_back(lid);
				debug_print(*ec,  "[QE] objectFromBinder(): added to a vector");
			}
			dbValue->setVector(&vectorValue);
			debug_print(*ec,  "[QE] objectFromBinder(): value is STRUCT type");
			break;
		}
		case QueryResult::QVIRTUAL: {

			LogicalID *view_def = ((QueryVirtualResult*) binderItem)->view_defs.at(0);
			vector<QueryResult *> seeds = ((QueryVirtualResult*) binderItem)->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = exViews->getOn_procedure(view_def, "on_store", proc_code, proc_param, tr);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
			if (proc_code == "") {
				debug_print(*ec,  "[QE] CREATE on_store - this VirtualObject doesn't have this operation defined");
				debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			errcode = exViews->createNewSections((QueryVirtualResult*) binderItem, NULL, NULL, envs_sections, tr, this);
			if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;

			QueryResult *on_store_res;
			errcode = qres->pop(on_store_res);
			if (errcode != 0) return errcode;

			while (on_store_res->type() == QueryResult::QBAG) {
				if (on_store_res->size() != 1) {
					debug_print(*ec,  "[QE] objectFromBinder(): on_store procedure result is not single");
					debug_print(*ec,  (ErrQExecutor | EOtherResExp));
					return (ErrQExecutor | EOtherResExp);
				}
				QueryResult *curr;
				errcode = ((QueryBagResult *) on_store_res)->at(0, curr);
				on_store_res = curr;
			}

			QueryResult *next_binder = new QueryBinderResult(binderName, on_store_res);
			errcode = this->objectFromBinder(next_binder, newObject);
			if (errcode != 0) return errcode;
			debug_print(*ec,  "[QE] objectFromBinder(): value is VIRTUAL type");
			return 0;
		}
		default: {
			debug_print(*ec,  "[QE] objectFromBinder(): bad type item in a binder");
			debug_print(*ec,  (ErrQExecutor | EOtherResExp));
			return (ErrQExecutor | EOtherResExp);
		}
	}
	DataValue* value;
	value = dbValue;
	if ((errcode = tr->createObject(binderName, value, newObject)) != 0) {
		debug_print(*ec,  "[QE] Error in createObject");
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	return 0;
}

int QueryExecutor::procCheck(unsigned int qSize, LogicalID *lid, string &code, vector<string> &prms, int &founded) {
	debug_print(*ec,  "[QE] checking if this LogicalID point procedure we look for");
	int errcode;
	ObjectPointer *optr;

	errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
	if (errcode != 0) {
		debug_print(*ec,  "[QE] procCheck - Error in getObjectPointer.");
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}

	DataValue* data_value = optr->getValue();

	int vType = data_value->getType();
	ExtendedType extType = data_value->getSubtype();

	int procBody_count = 0;
	string tmp_code;
	unsigned int params_count = 0;
	int others_count = 0;
	vector<string> tmp_prms;
	if ((vType == Store::Vector) && (extType == Store::Procedure)) {
		vector<LogicalID*>* tmp_vec = (data_value->getVector());
		int vec_size = tmp_vec->size();
		for (int i = 0; i < vec_size; i++ ) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
				debug_print(*ec,  "[QE] procCheck - Error in getObjectPointer");
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			if (tmp_vType == Store::String) {
				if (tmp_name == QE_METHOD_PROCBODY_BIND_NAME) {
					tmp_code = tmp_data_value->getString();
					procBody_count++;
				}
				else if (tmp_name == QE_METHOD_PARAM_BIND_NAME) {
					tmp_prms.push_back(tmp_data_value->getString());
					params_count++;
				}
				else others_count++;
			}
		}
		if ((procBody_count == 1) && (params_count == qSize) && (others_count == 0)) {
			debug_print(*ec,  "[QE] procCheck - success, this is procedure we've looked for");
			if (founded != 0) {
				debug_print(*ec,  "[QE] error there should be only one procedure in a scope with the same name and number of parameters");
				debug_print(*ec,  (ErrQExecutor | EProcNotSingle));
				return (ErrQExecutor | EProcNotSingle);
			}
			founded++;
			code = tmp_code;
			prms = tmp_prms;
		}
	}
	return 0;
}

int QueryExecutor::callProcedure(string code, vector<QueryBagResult*> sections) {

	debug_print(*ec,  "[QE] TNCALLPROC");
	debug_print(*ec,  code);

	int errcode;
	QueryParser* tmpQP = new QueryParser(session->get_id());
	TreeNode* tmpTN;
	errcode = tmpQP->parseIt(session->get_id(), code, tmpTN);
	if (errcode != 0) {
		debug_print(*ec,  "[QE] TNCALLPROC error while parsing procedure code");
		return errcode;
	}

	if(tmpTN != NULL) {

		envs->actual_prior++;
		unsigned int globalSectionNumber = envs->getSectionDBnumber() - 1;
		int oldGlobalSectionPrior = envs->es_priors[globalSectionNumber];
		envs->es_priors[globalSectionNumber] = envs->actual_prior;
		for (unsigned int i = 0; i < sections.size(); i++) {
			errcode = envs->push((QueryBagResult *) sections.at(i), tr, this);
			if (errcode != 0) return errcode;
		}

		am->disableAccessControl(envs->size() - 1);
		errcode = executeRecQuery(tmpTN);
		am->enableAccessControl(envs->size() - 1);	
		
		if (errcode == 0) {
			//skoro errcode == 0 tzn ze wywolana byla procedura a nie funkcja
			//nie bylo return value, wiec nie zwracam dalej otrzymanego wyniku
			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;
			qres->push(new QueryNothingResult());
		}
		else if (errcode == EEvalStopped) errcode = 0;
		if(errcode != 0) return errcode;

		for (unsigned int i = 0; i < sections.size(); i++) {
			errcode = envs->pop(this);
			if (errcode != 0) return errcode;
		}
		envs->es_priors[globalSectionNumber] = oldGlobalSectionPrior;
		envs->actual_prior--;

		delete tmpTN;
	}

	if (tmpQP != NULL) delete tmpQP;
	return 0;
}

int QueryExecutor::errorOccur(string msg, int errcode)
{
	int module = (errcode & ErrAllModules);
	if (module == ErrQExecutor)
		return qeErrorOccur(msg, errcode);
	if (module == ErrTManager)
		return trErrorOccur(msg, errcode);
	return otherErrorOccur(msg, errcode);
}

int QueryExecutor::otherErrorOccur( string msg, int errcode ) {
	debug_print(*ec,  msg);
	return errcode;
}

int QueryExecutor::qeErrorOccur( string msg, int errcode ) {
	debug_print(*ec,  msg);
	debug_print(*ec,  (ErrQExecutor | errcode));
	return (ErrQExecutor | errcode);
}

int QueryExecutor::trErrorOccur( string msg, int errcode ) {
	debug_print(*ec,  msg);
	antyStarveFunction(errcode);
	inTransaction = false;
	return errcode;
}

int QueryExecutor::execRecursivQueryAndPop(TreeNode *query, QueryResult*& execResult) {
	int errcode;
	if(query != NULL) {
		errcode = executeRecQuery(query);
		if(errcode != 0) return errcode;
	}
	else qres->push(new QueryNothingResult());
	errcode = qres->pop(execResult);
	if (errcode != 0) return errcode;
	return 0;
}

void QueryExecutor::pushStringToLIDs(string name, string bindName, vector<LogicalID*>& lids) {
	LogicalID* tmpLid;
	lidFromString(bindName, name, tmpLid);
	lids.push_back(tmpLid);
}

void QueryExecutor::pushStringsToLIDs(set<string>* names, string bindName, vector<LogicalID*>& lids) {
	for(set<string>::iterator i = names->begin(); i != names->end(); ++i) {
		pushStringToLIDs(*i, bindName, lids);
	}
}

int QueryExecutor::lidFromVector( string bindName, vector<QueryResult*> value, LogicalID*& lid) {
	return lidFromBinder(bindName, new QueryBagResult(value), lid);
}

int QueryExecutor::lidFromReference( string bindName, LogicalID* value, LogicalID*& lid) {
	return lidFromBinder(bindName, new QueryReferenceResult(value), lid);
}

int QueryExecutor::lidFromString( string bindName, string value, LogicalID*& lid) {
	return lidFromBinder(bindName, new QueryStringResult(value), lid);
}

int QueryExecutor::lidFromInt(string bindName, int value, LogicalID*& lid) {
	return lidFromBinder(bindName, new QueryIntResult(value), lid);
}

int QueryExecutor::lidFromBinder( string bindName, QueryResult* result, LogicalID*& lid) {
	QueryResult *tmp_bndr =new QueryBinderResult(bindName, result);
	ObjectPointer *tmp_optr;
	int errcode = objectFromBinder(tmp_bndr, tmp_optr);
	if (errcode != 0) return errcode;
	lid= tmp_optr->getLogicalID();
	return 0;
}

int QueryExecutor::createObjectAndPutOnQRes(DataValue* dbValue, string objectName, Store::ExtendedType type, LogicalID*& newLid) {
	int errcode = 0;
	ObjectPointer *newObject;
	DataValue* value = dbValue;
	if ((errcode = tr->createObject(objectName, value, newObject)) != 0) {
		debug_print(*ec,  "[QE] Error in createObject");
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}

	if(Store::None != type) {
		newObject->getValue()->setSubtype(type);
		errcode = tr->modifyObject(newObject, newObject->getValue());
		if (errcode != 0) {
			debug_print(*ec,  "[QE] TNCLASS - Error in modifyObject.");
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
	}
	newLid = newObject->getLogicalID();
	QueryResult *lidres = new QueryReferenceResult(newLid);

	errcode = qres->push(lidres);
	if (errcode != 0) return errcode;
	return 0;
}

int QueryExecutor::executeObjectDeclarationOrTypeDefinition(DeclareDefineNode *obdNode, bool isTypedef) {
	int errcode = 0;
	vector<string> *vec = new vector<string>();
	//we COULD,instead, replace the node if it was same as isTypedef; and throw ERROR only if it was the opposite.
	if ((tr->getDmlStct()->findRoot(obdNode->getName())) != DMLControl::absent) {
		return (ErrQExecutor | ESuchMdnExists);
	}
	errcode = sigs.objDeclRec(obdNode, obdNode->getName(), isTypedef, obdNode->getName(), vec, true, tr);
	if (errcode != 0)
	{
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}

	for (unsigned int i = 0; i < vec->size(); i++) {debug_print(*ec,  "qry: " + vec->at(i));}
	QueryParser parser(false);
	QueryResult *local_res = NULL;
	int locResult = 0;
	for (unsigned int i = 0; i < vec->size(); i++) {
		locResult += execute_locally(vec->at(i), &local_res, parser);
	}
	if (locResult != 0)	return (ErrQExecutor | EMdnCreateError);
	tr->getDmlStct()->addRoot(obdNode->getName(), (isTypedef ? DMLControl::type : DMLControl::object));
	debug_print(*ec,  tr->getDmlStct()->depsToString());
	debug_print(*ec,  tr->getDmlStct()->rootMdnsToString());
	DataScheme::dScheme(session->get_id(), tr)->setUpToDate(false);
	return 0;
}

int QueryExecutor::createOnView(string objectName, bool &found, QueryResult *binder, QueryResult *&result)
{
	found = false;
	vector<LogicalID*>* vec_virt;
	int errcode = 0;
	if ((errcode = tr->getViewsLID(objectName, vec_virt)) != 0) {
		debug_print(*ec,  "[QE] createOnView - error in getViewsLID");
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	int vecSize_virt = vec_virt->size();
	debug_printf(*ec, "[QE] %d Views LID by name taken\n", vecSize_virt);
	if (vecSize_virt > 1) {
		debug_print(*ec,  "[QE] Multiple views defining virtual objects with the same name");
		debug_print(*ec,  (ErrQExecutor | EBadBindName));
		return (ErrQExecutor | EBadBindName);
	}
	else if (vecSize_virt == 1) {
		LogicalID *view_def = vec_virt->at(0);
		string proc_code = "";
		string proc_param = "";
		errcode = exViews->getOn_procedure(view_def, "on_create", proc_code, proc_param, tr);
		if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}
		if (proc_code == "") {
			debug_print(*ec,  "[QE] operator <create> - this VirtualObject doesn't have this operation defined");
			debug_print(*ec,  (ErrQExecutor | EOperNotDefined));
			return (ErrQExecutor | EOperNotDefined);
		}
		vector<QueryBagResult*> envs_sections;

		QueryResult *create_arg = ((QueryBinderResult*)binder)->getItem();
		QueryResult *param_binder = new QueryBinderResult(proc_param, create_arg);

		errcode = exViews->createNewSections(NULL, (QueryBinderResult*) param_binder, view_def, envs_sections, tr, this);
		if (errcode != 0) {antyStarveFunction(errcode);	inTransaction = false; return errcode;}

		errcode = callProcedure(proc_code, envs_sections);
		if(errcode != 0) return errcode;

		QueryResult *res;
		errcode = qres->pop(res);
		if (errcode != 0) return errcode;
		QueryResult *bagged_res = new QueryBagResult();
		((QueryBagResult *) bagged_res)->addResult(res);
		for (unsigned int i = 0; i < bagged_res->size(); i++) {
			QueryResult *seed;
			errcode = ((QueryBagResult *) bagged_res)->at(i, seed);
			if (errcode != 0) return errcode;

			vector<QueryResult *> seeds;
			seeds.push_back(seed);
			vector<LogicalID *> view_defs;
			view_defs.push_back(view_def);

			QueryResult *virt_res = new QueryVirtualResult(objectName, view_defs, seeds, NULL);
			((QueryBagResult *) result)->addResult(virt_res);
			found = true;
		}
	}	
	return 0;
}

int QueryExecutor::initCg() { return ClassGraph::getHandle(cg); }

QueryExecutor::~QueryExecutor() {
	if (inTransaction) tr->abort();
	inTransaction = false;
        envs->deleteAll();
	delete envs;
	delete qres;
	delete exViews;
	delete am;
	debug_print(*ec,  "[QE] QueryExecutor shutting down\n");
}

}
