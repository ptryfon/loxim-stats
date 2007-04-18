#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ios>
#include <list>

#include "QueryResult.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "QueryParser/IndexNode.h"
#include "QueryParser/Privilige.h"
#include "QueryExecutor.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"
#include "RemoteExecutor.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace Indexes;
using namespace std;

namespace QExecutor {

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
	errcode = executeQuery(tree, result);
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
	*ec << "[QE] executeQuery()";

	if (tree != NULL) {
		int nodeType = tree->type();
		//tworzenie i usuwanie indeksow nie podlega transakcjom
		if (nodeType == (TreeNode::TNINDEXDDL)) {
			IndexNode *id = (dynamic_cast< IndexNode*>(tree));
			errcode = id->execute(result);
			//*result = new QueryNothingResult();
			return errcode;	
		} else	
		if (! inTransaction) {
			if (nodeType == TreeNode::TNTRANS) {
				if (((TransactNode *) tree)->getOp() == TransactNode::begin) {
					if (antyStarve) {
						ec->printf("[QE] Asking TransactionManager to REOPEN transaction number : %d\n", transactionNumber);
						errcode = TransactionManager::getHandle()->createTransaction(tr, transactionNumber);
						antyStarve = false;
					}
					else {
						*ec << "[QE] Asking TransactionManager for a NEW transaction";
						errcode = TransactionManager::getHandle()->createTransaction(tr);
					}
					if (errcode != 0) {
						*ec << "[QE] Error in createTransaction";
						*ec << "[QE] Transction not opened";
						*result = new QueryNothingResult();
						*ec << "[QE] nothing to do: QueryNothingResult created";
						inTransaction = false;
						return errcode;
					}
					else {
						transactionNumber = tr->getId()->getId();
						ec->printf("[QE] Transaction number : %d opened succesfully\n", transactionNumber);
						*result = new QueryNothingResult();
						*ec << "[QE] nothing else to do: QueryNothingResult created";
						inTransaction = true;
						return 0;
					}
				}
				else {
					*ec << "[QE] Transction not opened, type < begin; > first";
					*result = new QueryNothingResult();
					*ec << "[QE] nothing to do: QueryNothingResult created";
					return 0;
				}
			}
			else {
				*ec << "[QE] Transction not opened, type < begin; > first";
				*result = new QueryNothingResult();
				*ec << "[QE] nothing to do: QueryNothingResult created";
				return 0;
			}
		}
		if (nodeType == (TreeNode::TNTRANS)) {
			TransactNode::transactionOp op = ((TransactNode *) tree)->getOp();
			switch (op) {
				case TransactNode::begin: {
					*ec << "[QE] ERROR! Transaction already opened. It can't be opened once more!";
					*result = new QueryNothingResult();
					*ec << (ErrQExecutor | EQEUnexpectedErr);
					return ErrQExecutor | EQEUnexpectedErr;
				}
				case TransactNode::end: {
					errcode = tr->commit();
					inTransaction = false;
					if (errcode != 0) {
						*ec << "[QE] error in transaction->commit()";
						*result = new QueryNothingResult();
						return errcode;
					}
					*ec << "[QE] Transaction commited succesfully";
					break;
				}
				case TransactNode::abort: {
					errcode = tr->abort();
					inTransaction = false;
					if (errcode != 0) {
						*ec << "[QE] error in transaction->abort()";
						*result = new QueryNothingResult();
						return errcode;
					}
					*ec << "[QE] Transaction aborted succesfully";
					break;
				}
				default: {
					*ec << "[QE] ERROR! unknown transaction operator";
					*result = new QueryNothingResult();
					*ec << (ErrQExecutor | EUnknownNode);
					return ErrQExecutor | EUnknownNode;
				}
			}
			*result = new QueryNothingResult();
			*ec << "[QE] Nothing else to do. QueryNothingResult created";
			return 0;
		} 
		else if (nodeType == (TreeNode::TNVALIDATION)) {
		    UserData *user_data = session_data->get_user_data();
		    /* if user_data != NULL -> user id logged in, cannot log twice in one sesion */ 
		    if (user_data != NULL) {
			*result = new QueryBoolResult(false);
		    }
		    else {
			ValidationNode *val_node = (ValidationNode *) tree;
			if (priviliged_mode) {
			    set_user_data(val_node);			
			    *result = new QueryBoolResult(true); 
			}
			else {
			    string pass_check_query = QueryBuilder::getHandle()->
					    query_for_password(val_node->get_login(), val_node->get_passwd());
		    	
    			    QueryResult *local_res = NULL;
			    int local_ret = execute_locally(pass_check_query, &local_res);
			    if (local_ret || local_res == NULL || local_res->isBool() == false) {
				*result = new QueryBoolResult(false); 
			    } 		
			    else {
				bool b;
				local_res->getBoolValue(b);
				if (b) { 
				    /* user_data is now NULL value */
				    set_user_data(val_node);
				}
				*result = local_res;
			    }			
			}

		    }
		}
		else if (nodeType == (TreeNode::TNGRANTPRIV)) {
		    GrantPrivNode *grant_node = (GrantPrivNode *) tree;
		    PriviligeListNode *priv_list_or_null = grant_node->get_priv_list();

		    /* check if user has privilige to grant other privilige */
		    bool grant_priv = true; 
		    while (priv_list_or_null != NULL) {
			Privilige *privilige = priv_list_or_null->get_priv();
			string priv_name = privilige->get_priv_name();
			priv_list_or_null = priv_list_or_null->try_get_priv_list();
			    
			NameListNode *name_list_or_null = grant_node->get_name_list();
			while (name_list_or_null != NULL) {
			    string name = name_list_or_null->get_name();
			    name_list_or_null = name_list_or_null->try_get_name_list();
			    
			    grant_priv = grant_priv && assert_grant_priv(priv_name, name);			
			    if (grant_priv == false) break;
			};			
		    };
		    if (grant_priv) {
			string user = grant_node->get_user();
			int grant_option = grant_node->get_with_grant_option();
			priv_list_or_null = grant_node->get_priv_list();

			while (priv_list_or_null != NULL) {
			    Privilige *privilige = priv_list_or_null->get_priv();
			    string priv_name = privilige->get_priv_name();
			    priv_list_or_null = priv_list_or_null->try_get_priv_list();
			    
			    NameListNode *name_list_or_null = grant_node->get_name_list();
			    while (name_list_or_null != NULL) {
				string name = name_list_or_null->get_name();
				name_list_or_null = name_list_or_null->try_get_name_list();
			    
				string query = QueryBuilder::getHandle()->grant_priv_query(priv_name, name, user, grant_option); 
				QueryResult *local_res = NULL;
				execute_locally(query, &local_res);
			    };			
			};		    
			*result = new QueryBoolResult(true);			
		    }
		    else {
			*result = new QueryBoolResult(false);
		    }			    
		}
		else if (nodeType == (TreeNode::TNREVOKEPRIV)) {
		    RevokePrivNode *revoke_node = (RevokePrivNode *) tree;
		    PriviligeListNode *priv_list_or_null = revoke_node->get_priv_list();

		    /* check if user has privilige to revoke other privilige */
		    bool revoke_priv = true; 
		    while (priv_list_or_null != NULL) {
			Privilige *privilige = priv_list_or_null->get_priv();
			string priv_name = privilige->get_priv_name();
			priv_list_or_null = priv_list_or_null->try_get_priv_list();
			    
			NameListNode *name_list_or_null = revoke_node->get_name_list();
			while (name_list_or_null != NULL) {
			    string name = name_list_or_null->get_name();
			    name_list_or_null = name_list_or_null->try_get_name_list();
			    
			    revoke_priv = revoke_priv && assert_revoke_priv(priv_name, name);			
			    if (revoke_priv == false) break;
			};			
		    };
		    if (revoke_priv) {
			string user = revoke_node->get_user();
			priv_list_or_null = revoke_node->get_priv_list();

			while (priv_list_or_null != NULL) {
			    Privilige *privilige = priv_list_or_null->get_priv();
			    string priv_name = privilige->get_priv_name();
			    priv_list_or_null = priv_list_or_null->try_get_priv_list();
			    
			    NameListNode *name_list_or_null = revoke_node->get_name_list();
			    while (name_list_or_null != NULL) {
				string name = name_list_or_null->get_name();
				name_list_or_null = name_list_or_null->try_get_name_list();
			    
				string query = QueryBuilder::getHandle()->revoke_priv_query(priv_name, name, user);
				QueryResult *local_res = NULL;
				execute_locally(query, &local_res);
			    };			
			};		    
			*result = new QueryBoolResult(true);			
		    }
		    else {
			*result = new QueryBoolResult(false);
		    }			    
		}		
		else if (nodeType == (TreeNode::TNCREATEUSER)) {
		    CreateUserNode *createUserNode = (CreateUserNode *) tree;
		    bool create_priv = assert_create_user_priv();
		    
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
		    bool remove_priv = assert_remove_user_priv();
		    
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
		}	
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
			
			// TODO nie moge na zewnatrz oddac QueryVirtualResultow...
			int next_errcode;
			if (errcode == 0) next_errcode = deVirtualize(tmp_result, *result);
			
			int howMany = QueryResult::getCount();
			ec->printf(" QueryResults in use: %d\n", howMany);
			
			if (errcode != 0) return errcode;
			if (next_errcode != 0) return errcode;
			*ec << "[QE] Done!";
			return 0;
		}

	} //if tree != NULL
	else { // tree == NULL; return empty result
		*ec << "[QE] empty tree, nothing to do";
		*result = new QueryNothingResult();
		*ec << "[QE] QueryNothingResult created";
	}
	return 0;
}

void QueryExecutor::set_priviliged_mode(bool mode) {
    this->priviliged_mode = mode;
};

void QueryExecutor::set_user_data(ValidationNode *val_node) {
    string login  = val_node->get_login();
    string passwd = val_node->get_passwd();

    UserData *user_data = new UserData(login, passwd);
    session_data->set_user_data(user_data);    
};
int QueryExecutor::execute_locally(string query, QueryResult **res) {
    
    QueryParser parser;
    TreeNode *tree = NULL;
    system_privilige_checking = true;
    parser.parseIt(query, tree);
    
    int errcode = envs->pushDBsection();
    if (errcode != 0) return errcode;
    errcode = this->executeRecQuery(tree);
    if (errcode != 0) return errcode;
    errcode = qres->pop(*res);
    if (errcode != 0) return errcode;
    //errcode = envs->popDBsection();
    //if (errcode != 0) return errcode;

    system_privilige_checking = false;
    return 0;
};

bool QueryExecutor::is_dba() {
    string login = session_data->get_user_data()->get_login();
    return "root" == login;
};

bool QueryExecutor::assert_privilige(string priv_name, string object) {
    if (is_dba()) return true;
    string query = QueryBuilder::getHandle()->
			query_for_privilige(session_data->get_user_data()->get_login(), priv_name, object);
/*
    ofstream out("privilige.debug", ios::app);
    out << query << endl;
    out.close();    
*/    
    return assert_bool_query(query);    
};

bool QueryExecutor::assert_bool_query(string query) {
    QueryResult *result = NULL;
    int local_ret = execute_locally(query, &result);
    if (local_ret || result == NULL || result->isBool() == false) {
	return false;
    }
    bool b;
    result->getBoolValue(b);
    return b;    
};

bool QueryExecutor::assert_grant_priv(string priv_name, string name) {
    if (is_dba()) return true;
    
    string query = QueryBuilder::getHandle()->
			query_for_privilige_grant(session_data->get_user_data()->get_login(), priv_name, name);
			
    return assert_bool_query(query);
};

bool QueryExecutor::assert_revoke_priv(string priv_name, string name) {
    if (is_dba()) return true;
    return false;
};

bool QueryExecutor::assert_create_user_priv() {
    if (is_dba()) return true;

    string query = QueryBuilder::getHandle()->
			query_for_privilige(session_data->get_user_data()->get_login(), Privilige::CREATE_PRIV, "xuser");
    return assert_bool_query(query);    
};

bool QueryExecutor::assert_remove_user_priv() {
    if (is_dba()) return true;

    string query = QueryBuilder::getHandle()->
			query_for_privilige(session_data->get_user_data()->get_login(), Privilige::DELETE_PRIV, "xuser");
    return assert_bool_query(query);
};





int QueryExecutor::executeRecQuery(TreeNode *tree) {

	int errcode;
	*ec << "[QE] executeRecQuery()";

	if (this->evalStopped()) {
		*ec << "[QE] query evaluating stopped by Server, aborting transaction ";
		errcode = tr->abort();
		inTransaction = false;
		if (errcode != 0) {
			*ec << "[QE] error in transaction->abort()";
			return errcode;
		}
		*ec << "[QE] Transaction aborted succesfully";
		*ec << (ErrQExecutor | EEvalStopped);
		return ErrQExecutor | EEvalStopped;
	}

	if (tree != NULL) {
		int nodeType = tree->type();
		*ec << "[QE] TreeType taken";
		*ec << tree->toString(0, true);
		switch (nodeType)
		{
		case TreeNode::TNNAME: {
			*ec << "[QE] Type: TNNAME";
			string name = tree->getName();
			if (!system_privilige_checking && assert_privilige(Privilige::READ_PRIV, name) == false) {
			    errcode = qres->push(new QueryBagResult());
			    return 0;
			}
			int sectionNumber = ((NameNode *) tree)->getBindSect();
			QueryResult *result = new QueryBagResult();
			errcode = envs->bindName(name, sectionNumber, tr, this, result);
			if (errcode != 0) return errcode;
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Done!";
			return 0;
		}//case TNNAME

		case TreeNode::TNPROC: {
			*ec << "[QE] Type: TNPROC";
			
			string name = ((ProcedureNode *) tree)->getName();
			QueryNode* tmpQN = ((ProcedureNode *) tree)->getCode();
			vector<string> params = ((ProcedureNode *) tree)->getParams();
			unsigned int paramsNumb = ((ProcedureNode *) tree)->getParamsNumb();
			
			string code = tmpQN->deParse();
			delete tmpQN;
			
			QueryResult *strct = new QueryStructResult();
			QueryResult *code_str = new QueryStringResult(code);
			QueryResult *code_bind = new QueryBinderResult("ProcBody", code_str);
			strct->addResult(code_bind);
			for (unsigned int i=0; i<paramsNumb; i++) {
				QueryResult *param_str = new QueryStringResult(params[i]);
				QueryResult *param_bind = new QueryBinderResult("Param", param_str);
				strct->addResult(param_bind);
			}
			QueryResult *final_binder = new QueryBinderResult(name, strct);
			
			ObjectPointer *optr;
			errcode = objectFromBinder(final_binder, optr);
			if (errcode != 0) return errcode;
			
			optr->getValue()->setSubtype(Store::Procedure);
			
			errcode = tr->modifyObject(optr, optr->getValue()); 
			if (errcode != 0) {
				*ec << "[QE] TNPROC - Error in modifyObject.";
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
			*ec << "[QE] Type: TNREGPROC";
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
				*ec << "[QE] TNREGPROC error - execution result is not QueryReference";
				*ec << (ErrQExecutor | ERefExpected);
				return ErrQExecutor | ERefExpected;
			}
			
			ObjectPointer *optr;
			errcode = tr->getObjectPointer (((QueryReferenceResult*)execution_result)->getValue(), Store::Read, optr);
			if (errcode != 0) {
				*ec << "[QE] register proc operation - Error in getObjectPointer.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = tr->addRoot(optr);
			if (errcode != 0) { 
				*ec << "[QE] Error in addRoot";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;
			
			return 0;
		}//case TNREGPROC

		case TreeNode::TNCALLPROC: {
			*ec << "[QE] Type: TNCALLPROC";
			
			vector<QueryNode*> queries = ((CallProcNode *) tree)->getQueries();
			unsigned int queries_size = ((CallProcNode *) tree)->howManyParts();
			string name = ((CallProcNode *) tree)->getName();
			
			vector<string> params;
			string code = "";
			
			errcode = envs->bindProcedureName(name, queries_size, tr, this, code, params);
			if (errcode != 0) return errcode;
			
			if (code == "") {
				qres->push(new QueryBagResult());
				return 0;
			}
			
			QueryResult *new_envs_sect = new QueryBagResult();
			for (unsigned int i=0; i<queries_size; i++) {
				errcode = executeRecQuery(queries[i]);
				if(errcode != 0) return errcode;
				QueryResult *execution_result;
				errcode = qres->pop(execution_result);
				if (errcode != 0) return errcode;
				QueryResult *new_binder = new QueryBinderResult(params[i], execution_result);
				new_envs_sect->addResult(new_binder);
			}
			
			// TODO przed wywolaniem procedury czasem pewnie trzeba wlozyc na stos cos
			// wiecej niz tylko parametry np. dla metod w klasach trzeba wlozyc sekcje z
			// atrybutami obiektu, atrybutami klasowymi etc.
			
			// callProcedure wklada na stos sekcje w takiej kolejnosci jak sa w podanym wektorze
			// envs_sections(0) bedzie najnizej, envs_sections(n) bedzie na wierzcholku stosu
			vector<QueryBagResult*> envs_sections;
			envs_sections.push_back((QueryBagResult *)new_envs_sect);
			
			errcode = callProcedure(code, envs_sections);
			if(errcode != 0) return errcode;
			
			return 0;
		}//case TNCALLPROC

		case TreeNode::TNRETURN: {
			*ec << "[QE] Type: TNRETURN (begin)";
			QueryNode *query = ((ReturnNode *) tree)->getQuery();
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());
			*ec << "[QE] Type: TNRETURN (done)";
			return EEvalStopped;
		}//case TNRETURN
		
		case TreeNode::TNVIEW: {
			*ec << "[QE] Type: TNVIEW";
			
			string view_name = ((ViewNode *) tree)->getName();
			QueryNode* virtual_objects = ((ViewNode *) tree)->getVirtualObjects();
			vector<QueryNode*> procs = ((ViewNode *) tree)->getProcedures();
			unsigned int procs_num = procs.size();
			vector<QueryNode*> views = ((ViewNode *) tree)->getSubviews();
			unsigned int views_num = views.size();
			
			int on_upd_num = 0;
			int on_cre_num = 0;
			int on_del_num = 0;
			int on_ret_num = 0;
			
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
				*ec << "[QE] TNVIEW error - execution result is not QueryReference";
				*ec << (ErrQExecutor | ERefExpected);
				return ErrQExecutor | ERefExpected;
			}
			LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();	
			errcode = tr->getObjectPointer (lid, Store::Read, optr);
			if (errcode != 0) {
				*ec << "[QE] view creating operation - Error in getObjectPointer.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string virt_obj_name = optr->getName();
			if ((virt_obj_name == "on_update") || (virt_obj_name == "on_create") || (virt_obj_name == "on_delete") || (virt_obj_name == "on_retrieve")) {
				*ec << "[QE] TNVIEW error - virtual objects shouldn't be named like \'on_\' procedures";
				*ec << (ErrQExecutor | EBadViewDef);
				return ErrQExecutor | EBadViewDef;
			}
			
			QueryResult *tmp_bndr =new QueryBinderResult("virtual_objects", new QueryStringResult(virt_obj_name));
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
					*ec << "[QE] TNVIEW error - execution result is not QueryReference";
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}
				lid = ((QueryReferenceResult*)execution_result)->getValue();
				errcode = tr->getObjectPointer (lid, Store::Read, optr);
				if (errcode != 0) {
					*ec << "[QE] view creating operation - Error in getObjectPointer.";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				string optr_name = optr->getName();
				if (optr_name == "on_update") on_upd_num++;
				else if (optr_name == "on_create") on_cre_num++;
				else if (optr_name == "on_delete") on_del_num++;
				else if (optr_name == "on_retrieve") on_ret_num++;
				vec_lid.push_back(lid);
			}
			if ((on_upd_num > 1) || (on_cre_num > 1) || (on_del_num > 1) || (on_ret_num > 1)) {
				*ec << "[QE] TNVIEW error - multiple \'on_\' procedure definition found";
				*ec << (ErrQExecutor | EBadViewDef);
				return ErrQExecutor | EBadViewDef;
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
					*ec << "[QE] TNVIEW error - execution result is not QueryReference";
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}
				lid = ((QueryReferenceResult*)execution_result)->getValue();
				vec_lid.push_back(lid);
			}
			
			DBDataValue *dbValue = new DBDataValue();
			dbValue->setVector(&vec_lid);
			DataValue* value = dbValue;
			
			ObjectPointer *newObject;
			if ((errcode = tr->createObject(view_name, value, newObject)) != 0) {
				*ec << "[QE] Error in createObject";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			newObject->getValue()->setSubtype(Store::View);
			errcode = tr->modifyObject(newObject, newObject->getValue()); 
			if (errcode != 0) {
				*ec << "[QE] TNVIEW - Error in modifyObject.";
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
			*ec << "[QE] Type: TNREGVIEW";
			QueryNode *query = ((RegisterViewNode *) tree)->getQuery();
			if(query != NULL) {
				errcode = executeRecQuery(query);
				if(errcode != 0) return errcode;
			}
			else qres->push(new QueryNothingResult());
			
			QueryResult *execution_result;
			errcode = qres->pop(execution_result);
			if (errcode != 0) return errcode;
			
			if (execution_result->type() != QueryResult::QREFERENCE) {
				*ec << "[QE] TNREGVIEW error - execution result is not QueryReference";
				*ec << (ErrQExecutor | ERefExpected);
				return ErrQExecutor | ERefExpected;
			}
			
			ObjectPointer *optr;
			errcode = tr->getObjectPointer (((QueryReferenceResult*)execution_result)->getValue(), Store::Read, optr);
			if (errcode != 0) {
				*ec << "[QE] register view operation - Error in getObjectPointer.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = tr->addRoot(optr);
			if (errcode != 0) { 
				*ec << "[QE] Error in addRoot";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			string virtual_objects_name = "";
			vector<LogicalID*>* inner_vec = (optr->getValue())->getVector();
			for (unsigned int i=0; i < inner_vec->size(); i++) {
				ObjectPointer *inner_optr;
				errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr);
				if (errcode != 0) {
					*ec << "[QE] register view operation - Error in getObjectPointer.";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				if (inner_optr->getName() == "virtual_objects") {
					virtual_objects_name = (inner_optr->getValue())->getString();
					break;
				}
			}
			
			errcode = tr->addView(virtual_objects_name.c_str(), optr);
			if (errcode != 0) { 
				*ec << "[QE] Error in addView";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;
			
			return 0;
		}//case TNREGVIEW
		
		
		
		case TreeNode::TNCREATE: {
			*ec << "[QE] Type: TNCREATE";
			QueryResult *result = new QueryBagResult();
			errcode = executeRecQuery (tree->getArg());
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
				ObjectPointer *optr;
				errcode = this->objectFromBinder(binder, optr);
				if (errcode != 0) return errcode;
				//delete binder;
				string object_name = optr->getName();
				if (!system_privilige_checking && 
					assert_privilige(Privilige::CREATE_PRIV, object_name) == false) {
				/*
					ofstream out("privilige.debug", ios::app);
					out << "create " << object_name << endl;
					out.close();
				*/
				    continue;
				}
				errcode = tr->addRoot(optr);
				if (errcode != 0) { 
					*ec << "[QE] Error in addRoot";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
				((QueryBagResult *) result)->addResult (lidres);
			}
			//delete tmp_result;
			//delete bagRes;
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] CREATE Done!";
			return 0;
		} //case TNCREATE

		case TreeNode::TNINT: {
			int intValue = ((IntNode *) tree)->getValue();
			ec->printf("[QE] TNINT: %d\n", intValue);
			QueryResult *result = new QueryIntResult(intValue);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			ec->printf("[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
		} //case TNINT

		case TreeNode::TNSTRING: {
			string stringValue = ((StringNode *) tree)->getValue();
			*ec << "[QE] TNSTRING: " << stringValue;
			QueryResult *result = new QueryStringResult(stringValue);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] QueryStringResult created";
			return 0;
		} //case TNSTRING

		case TreeNode::TNDOUBLE: {
			double doubleValue = ((DoubleNode *) tree)->getValue();
			ec->printf("[QE] TNDOUBLE: %f\n", doubleValue);
			QueryResult *result = new QueryDoubleResult(doubleValue);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			ec->printf("[QE] QueryDoubleResult (%f) created\n", doubleValue);
			return 0;
		} //case TNDOUBLE

		case TreeNode::TNAS: {
			*ec << "[QE] AS/GROUP_AS operation recognized";
			string name = tree->getName();
			errcode = executeRecQuery(tree->getArg());
			if (errcode != 0) return errcode;
			QueryResult *tmp_result;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			bool grouped = ((NameAsNode *) tree)->isGrouped();
			QueryResult *final_result = new QueryBagResult();
			if (grouped) {
				*ec << "[QE] GROUP AS operation";
				QueryResult *tmp_binder = new QueryBinderResult(name, tmp_result);
				*ec << "[QE] GROUP AS: new binder created and added to final result";
				final_result->addResult(tmp_binder);
			}
			else {
				*ec << "[QE] AS operation";
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
					*ec << "[QE] AS: new binder created and added to final result";
					final_result->addResult(tmp_binder);
				}
				//delete partial_result;
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] AS operation Done!";
			return 0;
		} //case TNAS

		case TreeNode::TNUNOP: {
			UnOpNode::unOp op = ((UnOpNode *) tree)->getOp();
			*ec << "[QE] Unary operator - type recognized";
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
			*ec << "[QE] Unary operation Done!";
			return 0;
		} //case TNUNOP

		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			*ec << "[QE] Algebraic operator - type recognized";
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
			errcode = this->algOperate(op, lResult, rResult, op_result);
			if (errcode != 0) return errcode;
			//delete lResult;
			//delete rResult;
			errcode = qres->push(op_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Algebraic operation Done!";
			return 0;
		} // TNALGOP

		case TreeNode::TNFIX: {
			*ec << "[QE] FIXPOINT operation recognized";
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
					*ec << "[QE] query evaluating stopped by Server, aborting transaction ";
					errcode = tr->abort();
					inTransaction = false;
					if (errcode != 0) {
						*ec << "[QE] error in transaction->abort()";
						return errcode;
					}
					*ec << "[QE] Transaction aborted succesfully";
					*ec << (ErrQExecutor | EEvalStopped);
					return ErrQExecutor | EEvalStopped;
				}
				QueryResult *next_final = new QueryStructResult();
				QueryResult *newStackSection = new QueryBagResult();
				errcode = (final)->nested(tr, newStackSection, this);
				if (errcode != 0) return errcode;
				errcode = envs->push((QueryBagResult *) newStackSection);
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
				errcode = envs->pop();
				if (errcode != 0) return errcode;
				if (not (final->equal(next_final))) {
					//delete final;
					final = next_final;
				}
				else looping = false;
			}
			errcode = qres->push(final);
			if (errcode != 0) return errcode;
			*ec << "[QE] Fixpoint operation Done!";
			return 0;
		} // TNFIX

		case TreeNode::TNPARAM: {
			*ec << "[QE] Parametr opeartion";
			if (prms == NULL) {
				*ec << "[QE] error! Parametr operation - params == NULL";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return ErrQExecutor | EQEUnexpectedErr;
			}
			string paramName = ((ParamNode *) tree)->getName();
			int howMany = prms->count(paramName);
			if (howMany != 1) {
				*ec << "[QE] error! Parametr operation - wrong parametr";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return ErrQExecutor | EQEUnexpectedErr;
			}
			map<string, QueryResult*>::iterator pos;
			QueryResult *res;
			pos = prms->find(paramName);
			if (pos != prms->end()) res = pos->second;
			else {
				*ec << "[QE] error! Parametr operation - index out of range";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return ErrQExecutor | EQEUnexpectedErr;
			} 
			errcode = qres->push(res);
			if (errcode != 0) return errcode;
			*ec << "[QE] Parametr operation Done!";
			return 0;
		} // TNPARAM

		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			*ec << "[QE] NonAlgebraic operator - type recognized";
			QueryResult *l_tmp_Result;
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
			*ec << "[QE] Left argument of NonAlgebraic query has been computed";
			QueryResult *partial_result = new QueryBagResult();
			QueryResult *visited_result = new QueryBagResult();
			QueryResult *final_result = new QueryBagResult();
			if ((op == NonAlgOpNode::closeBy) || (op == NonAlgOpNode::closeUniqueBy) || (op == NonAlgOpNode::leavesBy) || (op == NonAlgOpNode::leavesUniqueBy)) {
				switch (op) {
					case NonAlgOpNode::closeBy: {
						*ec << "[QE] NonAlgebraic recursive operator <closeBy>";
						partial_result->addResult(lResult);
						final_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::closeUniqueBy: {
						*ec << "[QE] NonAlgebraic recursive operator <closeUniqueBy>";
						partial_result->addResult(lResult);
						final_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::leavesBy: {
						*ec << "[QE] NonAlgebraic recursive operator <leavesBy>";
						partial_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::leavesUniqueBy: {
						*ec << "[QE] NonAlgebraic recursive operator <leavesUniqueBy>";
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
						*ec << "[QE] query evaluating stopped by Server, aborting transaction ";
						errcode = tr->abort();
						inTransaction = false;
						if (errcode != 0) {
							*ec << "[QE] error in transaction->abort()";
							return errcode;
						}
						*ec << "[QE] Transaction aborted succesfully";
						*ec << (ErrQExecutor | EEvalStopped);
						return ErrQExecutor | EEvalStopped;
					}

					QueryResult *currentResult;
					errcode = partial_result->getResult(currentResult);
					if (errcode != 0) return errcode;
					QueryResult *newStackSection = new QueryBagResult();
					errcode = (currentResult)->nested(tr, newStackSection, this);
					if (errcode != 0) return errcode;
					errcode = envs->push((QueryBagResult *) newStackSection);
					if (errcode != 0) return errcode;
					QueryResult *newResult;
					errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
					if (errcode != 0) return errcode;
					errcode = qres->pop(newResult);
					if (errcode != 0) return errcode;
					*ec << "[QE] Computing right Argument with a new scope of ES";
					errcode = envs->pop();
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
								errcode = this->isIncluded(current_new_result, final_result, isIncl);
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
									errcode = this->isIncluded(c_result, visited_result, isIncl);
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
					*ec << "[QE] For each row of this score, the right argument will be computed";
					for (unsigned int i = 0; i < (lResult->size()); i++) {
						QueryResult *currentResult;
						QueryResult *newStackSection = new QueryBagResult();
						if ((lResult->type()) == QueryResult::QSEQUENCE)
							errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
						else
							errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
						if (errcode != 0) return errcode;
						ec->printf("[QE] zaczynam nested()\n");
						errcode = (currentResult)->nested(tr, newStackSection, this);
						if (errcode != 0) return errcode;
						ec->printf("[QE] nested(): function calculated for current row number %d\n", i);
						errcode = envs->push((QueryBagResult *) newStackSection);
						if (errcode != 0) return errcode;
						QueryResult *rResult;
						errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(rResult);
						if (errcode != 0) return errcode;
						*ec << "[QE] Computing right Argument with a new scope of ES";
						errcode = this->combine(op,currentResult,rResult,partial_result);
						if (errcode != 0) return errcode;
						*ec << "[QE] Combined partial results";
						errcode = envs->pop();
						if (errcode != 0) return errcode;
					}
				}
				else {
					*ec << "[QE] ERROR! NonAlgebraic operation, bad left argument";
					*ec << (ErrQExecutor | EOtherResExp);
					return ErrQExecutor | EOtherResExp;
				}
				errcode = this->merge(op,partial_result, final_result);
				if (errcode != 0) return errcode;
				*ec << "[QE] Merged partial results into final result";
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] NonAlgebraic operation Done!";
			return 0;
		} //case TNNONALGOP

		case TreeNode::TNCOND: {
			CondNode::condOp op = ((CondNode *) tree)->getOp();
			*ec << "[QE] Conditional operator - type recognized";
			switch (op) {
				case CondNode::iff: {
					*ec << "[QE] IF <q1> THEN <q2> FI";
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
						*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
						*ec << (ErrQExecutor | EBoolExpected);
						return ErrQExecutor | EBoolExpected;
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
					*ec << "[QE] IF <q1> THEN <q2> ELSE <q3> FI";
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
						*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
						*ec << (ErrQExecutor | EBoolExpected);
						return ErrQExecutor | EBoolExpected;
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
					*ec << "[QE] WHILE <q1> DO <q2> OD";
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
						*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
						*ec << (ErrQExecutor | EBoolExpected);
						return ErrQExecutor | EBoolExpected;
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
							*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
							*ec << (ErrQExecutor | EBoolExpected);
							return ErrQExecutor | EBoolExpected;
						}
					}
					QueryResult *result = new QueryNothingResult();
					errcode = qres->push(result);
					if (errcode != 0) return errcode;
					break;
				}
				default: {
					*ec << "[QE] ERROR! Condition operation type not known";
					*ec << (ErrQExecutor | EUnknownNode);
					return ErrQExecutor | EUnknownNode;
				}
			}
			*ec << "[QE] Condition operation Done!";
			return 0;
		} //case TNCOND
		case TreeNode::TNINDEXDML: {
			//IndexNode *id = ;
			QueryResult* result;
			errcode = (dynamic_cast< IndexNode*>(tree))->execute(&result);
			//*result = new QueryNothingResult();
			return errcode;	
		}	
		case TreeNode::TNLINK: {
		*ec << "[QE] Link opeartion";

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
		if (!system_privilige_checking && assert_privilige(Privilige::CREATE_PRIV, object_name) == false) {
		    QueryBagResult * result = new QueryBagResult();
		    /*
		    ofstream out("privilige.debug", ios::app);
		    out << "create " << object_name << endl;
		    out.close();		    
		    */
		    errcode = qres->push(result);
		    if (errcode != 0) return errcode;
		    return 0;
		}
		if ((errcode = tr->addRoot(optr)) != 0) {
		    *ec << "[QE] Error in addRoot";
		    antyStarveFunction(errcode);
		    inTransaction = false;
		    return errcode;
		}

		QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
		QueryBagResult * result = new QueryBagResult();
		result->addResult (lidres);

		errcode = qres->push(result);
		if (errcode != 0) return errcode;

		*ec << "[QE] Link operation Done!";
		return 0;
		}
		case TreeNode::TNREMOTE: {
			
			RemoteNode* rn = (RemoteNode *) tree;
			LogicalID* lid = rn->getLogicalID();
			QueryReferenceResult* qrr = new QueryReferenceResult(lid);
			
			if (rn->isDeref()) {
			  //jesli deref to to!
				ec->printf("[QE] zaczynam prosbe o zdalny deref");
				QueryResult *op_result;
				errcode = this->derefQuery(qrr, op_result);
				if (errcode != 0) return errcode;
				//delete tmp_result;
				errcode = qres->push(op_result);
				if (errcode != 0) return errcode;
				*ec << "[QE] Unary operation Done!";
				return 0;
			}
				
			*ec << "zaczynam wezel TNREMOTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
			vector<ObjectPointer*>* vec;
			
			*ec << "remote 1";
			if (lid != NULL) {
				ec->printf("prosba o nested na obiekcie zdalnym\n");
				QueryResult *newStackSection = new QueryBagResult();
				errcode = qrr->nested(tr, newStackSection, this);
				if (errcode != 0) return errcode;
				errcode = qres->push(newStackSection);
				if (errcode != 0) return errcode;
				ec->printf("nested na obiekcie zdalnym udalo sie. nowa sekcja stosu gotowa");
				return 0;
			}
			*ec << "remote 1";
			if ((errcode = tr->getRoots(vec)) != 0) {
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			QueryBagResult *r = new QueryBagResult();
			int vecSize = vec->size();
			ec->printf("[QE] %d Roots LID by name taken\n", vecSize);
			for (int i = 0; i < vecSize; i++ ) {
				 ObjectPointer *optr = vec->at(i);
				 LogicalID *lid = optr->getLogicalID();
				 string name = optr->getName();
				 
				ec->printf("[QE] LogicalID received: %d\n",  lid->toInteger()); 
				QueryReferenceResult *lidres = new QueryReferenceResult(lid);
				QueryBinderResult *qbr = new QueryBinderResult(name, lidres);
				r->addResult(qbr);
			}
			delete vec;
			qres->push(r);
			return 0;
		}
		default:
			{
			*ec << "Unknown node type";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
			}

		} // end of switch(nodeType)

	} // if tree!=Null
	else { // tree == NULL; return empty result
		*ec << "[QE] empty tree, nothing to do";
		QueryResult *result = new QueryNothingResult();
		errcode = qres->push(result);
		if (errcode != 0) return errcode;
		*ec << "[QE] QueryNothingResult created";
	}
	return 0;
}//executeQuerry



int QueryExecutor::derefQuery(QueryResult *arg, QueryResult *&res) {
	*ec << "[QE] derefQuery()";
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
			LogicalID * lid = ((QueryReferenceResult *) arg)->getValue(); 
			if (lid != NULL && lid->getServer() != "") {
				*ec << "dereferencing remote object";
				RemoteExecutor *rex = new RemoteExecutor(lid->getServer(), lid->getPort(), this);
				rex->setLogicalID(lid);
				rex->setDeref();
				QueryResult* qr;
				errcode = rex->execute(&qr);
				if (errcode != 0) {
					return errcode;
				}
				*ec << "derefQuery on remote object done";
				res = qr;
				break;
			}
			
			if (((QueryReferenceResult *) arg)->wasRefed()) {
				res = arg;
				break;
			}
			LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
			*ec << "[QE] derefQuery() - dereferencing Object";
			ObjectPointer *optr;
			errcode = tr->getObjectPointer(ref_value, Store::Read, optr); 
			if (errcode != 0) {
				*ec << "[QE] Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			if (optr->isRoot()) {
			    string object_name = optr->getName();
			    if (!system_privilige_checking && 
				assert_privilige(Privilige::READ_PRIV, object_name) == false) {
				res = new QueryBagResult();				
				/*
				ofstream out("privilige.debug", ios::app);
				out << "read " << object_name << endl;
				out.close();
				*/
				break;
			    }
			}
			
			DataValue* value = optr->getValue();
			int vType = value->getType();
			switch (vType) {
				case Store::Integer: {
					*ec << "[QE] derefQuery() - ObjectValue = Int";
					int tmp_value = value->getInt();
					res = new QueryIntResult(tmp_value);
					break;
				}
				case Store::Double: {
					*ec << "[QE] derefQuery() - ObjectValue = Double";
					double tmp_value = value->getDouble();
					res = new QueryDoubleResult(tmp_value);
					break;
				}
				case Store::String: {
					*ec << "[QE] derefQuery() - ObjectValue = String";
					string tmp_value = value->getString();
					res = new QueryStringResult(tmp_value);
					break;
				}
				case Store::Pointer: {
					*ec << "[QE] derefQuery() - ObjectValue = Pointer";
					LogicalID *tmp_value = value->getPointer();
					res = new QueryReferenceResult(tmp_value);
					break;
				}
				case Store::Vector: {
					*ec << "[QE] derefQuery() - ObjectValue = Vector";
					vector<LogicalID*>* tmp_vec = value->getVector();
					res = new QueryStructResult();
					int tmp_vec_size = tmp_vec->size();
					for (int i = 0; i < tmp_vec_size; i++) {
						LogicalID *currID = tmp_vec->at(i);
						ObjectPointer *currOptr;
						errcode = tr->getObjectPointer(currID, Store::Read, currOptr); 
						if (errcode != 0) {
							*ec << "[QE] Error in getObjectPointer";
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						/*
						string object_name = currOptr->getName();
						if (!system_privilige_checking && 
							    assert_privilige(Privilige::READ_PRIV, object_name) == false) {
						    
						    //ofstream out("privilige.debug", ios::app);
						    //out << "read " << object_name << endl;
						    //out.close();
						    
						    continue;
						}*/						
						string currName = currOptr->getName();
						QueryResult *currIDRes = new QueryReferenceResult(currID);
						QueryResult *currInside;
						this->derefQuery(currIDRes, currInside);
						QueryResult *currBinder = new QueryBinderResult(currName, currInside);
						res->addResult(currBinder);
					}
					break;
				}
				default: {
					*ec << "[QE] derefQuery() - wrong argument type";
					*ec << (ErrQExecutor | EUnknownValue);
					return ErrQExecutor | EUnknownValue;
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
			//TODO watpie by to dzialalo w pelni tak jak powinno
			LogicalID *view_def = ((QueryVirtualResult*) arg)->view_def;
			vector<QueryResult *> seeds = ((QueryVirtualResult*) arg)->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = getOn_procedure(view_def, "on_retrieve", proc_code, proc_param);
			if (errcode != 0) return errcode;
			if (proc_code == "") {
				*ec << "[QE] derefQuery() - this VirtualObject doesn't have this operation defined";
				*ec << (ErrQExecutor | EOperNotDefined);
				return ErrQExecutor | EOperNotDefined;
			}
			vector<QueryBagResult*> envs_sections;
			//TODO tu pewnie trzeba jakos lepiej stos inicjowac przed wywolaniem virtual_objects
			for (int k = ((seeds.size()) - 1); k >= 0; k-- ) {
				QueryResult *bagged_seed = new QueryBagResult();
				((QueryBagResult *) bagged_seed)->addResult(seeds.at(k));
				envs_sections.push_back((QueryBagResult *) bagged_seed);
			}
			errcode = callProcedure(proc_code, envs_sections);
			if (errcode != 0) return errcode;
			errcode = qres->pop(res);
			if (errcode != 0) return errcode;
			break;
		}
		default: {
			*ec << "[QE] ERROR in derefQuery() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
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
}

int QueryExecutor::refQuery(QueryResult *arg, QueryResult *&res) {
	*ec << "[QE] refQuery()";
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
		default: {
			*ec << "[QE] ERROR in refQuery() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
	}
	return 0;
}

int QueryExecutor::nameofQuery(QueryResult *arg, QueryResult *&res) {
    
    
    *ec << "[QE] nameofQuery()";
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
            LogicalID * lid = ((QueryReferenceResult *) arg)->getValue();
             
            if (lid != NULL && lid->getServer() != "") {
                *ec << "dereferencing remote object";
                RemoteExecutor *rex = new RemoteExecutor(lid->getServer(), lid->getPort(), this);
                rex->setLogicalID(lid);
                rex->setDeref();
                QueryResult* qr;
                errcode = rex->execute(&qr);
                if (errcode != 0) {
                    return errcode;
                }
                *ec << "derefQuery on remote object done";
                res = qr;
                break;
            }
            
            if (((QueryReferenceResult *) arg)->wasRefed()) {
                res = arg;
                break;
            }
            LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
            *ec << "[QE] derefQuery() - dereferencing Object";
            ObjectPointer *optr;
            errcode = tr->getObjectPointer(ref_value, Store::Read, optr); 
            if (errcode != 0) {
                *ec << "[QE] Error in getObjectPointer";
                antyStarveFunction(errcode);
                inTransaction = false;
                return errcode;
            }
            
            if (optr->isRoot()) {
                string object_name = optr->getName();
                if (!system_privilige_checking && 
                assert_privilige(Privilige::READ_PRIV, object_name) == false) {
                res = new QueryBagResult();             
                /*
                ofstream out("privilige.debug", ios::app);
                out << "read " << object_name << endl;
                out.close();
                */
                break;
                }
            }
            
            res = new QueryStringResult(optr->getName());
                        
            break;
        }
        case QueryResult::QNOTHING: {
            res = arg;
            break;
        }
        default: {
            *ec << "[QE] ERROR in derefQuery() - unknown result type";
            *ec << (ErrQExecutor | EOtherResExp);
            return ErrQExecutor | EOtherResExp;
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
			*ec << "[QE] DEREF operation";
			errcode = this->derefQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::ref: {
			*ec << "[QE] REF operation";
			errcode = this->refQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
        case UnOpNode::nameof: {
            *ec << "[QE] NAMEOF operation";
            errcode = this->nameofQuery(arg, final);
            if (errcode != 0) return errcode;
            break;            
        }
		case UnOpNode::unMinus: {
			*ec << "[QE] UN_MINUS operation";
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
				*ec << "[QE] ERROR! UN_MINUS argument must be INT or DOUBLE";
				*ec << (ErrQExecutor | ENumberExpected);
				return ErrQExecutor | ENumberExpected;
			}
			break;
		}
		case UnOpNode::boolNot: {
			*ec << "[QE] NOT operation";
			if (arg->isBool()) {
				bool bool_tmp;
				errcode = arg->getBoolValue(bool_tmp);
				if (errcode != 0) return errcode;
				final = new QueryBoolResult(not bool_tmp);
			}
			else {
				*ec << "[QE] ERROR! NOT argument must be BOOLEAN";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case UnOpNode::count: {
			*ec << "[QE] COUNT operation";
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			int count_res = bagArg->size();
			final = new QueryIntResult(count_res);
			break;
		}
		case UnOpNode::sum: {
			*ec << "[QE] SUM operation";
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
			*ec << "[QE] AVG operation";
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
			*ec << "[QE] MIN operation";
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_min_res = 0;
			bool i_counted = false;
			double d_min_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				*ec << "[QE] MIN operation - can't evaluate min value on this set, QueryNothingResult created";
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
					*ec << "[QE] MIN - can't evaluate min value on this set, QueryNothingResult created";
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::max: {
			*ec << "[QE] MAX operation";
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_max_res = 0;
			bool i_counted = false;
			double d_max_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				*ec << "[QE] MAX operation - can't evaluate max value on this set, QueryNothingResult created";
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
					*ec << "[QE] MAX - can't evaluate max value on this set, QueryNothingResult created";
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::distinct: {
			*ec << "[QE] DISTINCT operation";
			final = new QueryBagResult();
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			if ((bagArg->size()) == 0)
				break;
			QueryBagResult *argSorted;
			errcode = ((QueryBagResult *) bagArg)->sortBag(argSorted);
			if (errcode != 0) return errcode;
			QueryResult *current;
			errcode = argSorted->getResult(current);
			if (errcode != 0) return errcode;
			final->addResult(current);
			while ((argSorted->size()) > 0) {
				QueryResult *next;
				errcode = argSorted->getResult(next);
				if (errcode != 0) return errcode;
				if (next->not_equal(current)) {
					current = next;
					final->addResult(current);
				}
			}
			break;
		}
		case UnOpNode::deleteOp: {
			*ec << "[QE] DELETE operation";
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			for (unsigned int i = 0; i < bagArg->size(); i++) {
				QueryResult* toDelete;  //the object to be deleted
				errcode = ((QueryBagResult *) bagArg)->at(i, toDelete);
				if (errcode != 0) return errcode;
				int toDeleteType = toDelete->type();
				if (toDeleteType == QueryResult::QREFERENCE) {
					LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
					ObjectPointer *optr;
					if ((errcode = tr->getObjectPointer (lid, Store::Write, optr)) !=0) { 
						*ec << "[QE] Error in getObjectPointer.";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					if (optr->getIsRoot()) {
						string object_name = optr->getName();
						if (!system_privilige_checking && 
							assert_privilige(Privilige::DELETE_PRIV, object_name) == false) {
						/*
						ofstream out("privilige.debug", ios::app);
						out << "delete " << object_name << endl;
						out.close();						    
						*/
						continue;
						}
						if (((optr->getValue())->getSubtype()) == Store::View) {
							if ((errcode = tr->removeView(optr)) != 0) { 
								*ec << "[QE] Error in removeView.";
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
						}
						if ((errcode = tr->removeRoot(optr)) != 0) { 
							*ec << "[QE] Error in removeRoot.";
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						*ec << "[QE] Root removed";
					}
					string object_name = optr->getName();
					if (!system_privilige_checking && 
						assert_privilige(Privilige::DELETE_PRIV, object_name) == false) {
						/*
						ofstream out("privilige.debug", ios::app);
						out << "delete " << object_name << endl;
						out.close();					    
						*/
					continue;
					}								
					if ((errcode = tr->deleteObject(optr)) != 0) { 
						*ec << "[QE] Error in deleteObject.";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
				}
				else if (toDeleteType == QueryResult::QVIRTUAL) {
					
					//TODO watpie by to dzialalo w pelni tak jak powinno
					LogicalID *view_def = ((QueryVirtualResult*) toDelete)->view_def;
					vector<QueryResult *> seeds = ((QueryVirtualResult*) toDelete)->seeds;
					string proc_code = "";
					string proc_param = "";
					errcode = getOn_procedure(view_def, "on_delete", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] delete() - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return ErrQExecutor | EOperNotDefined;
					}
					vector<QueryBagResult*> envs_sections;
					//TODO tu pewnie trzeba jakos lepiej stos inicjowac przed wywolaniem virtual_objects
					for (int k = ((seeds.size()) - 1); k >= 0; k-- ) {
						QueryResult *bagged_seed = new QueryBagResult();
						((QueryBagResult *) bagged_seed)->addResult(seeds.at(k));
						envs_sections.push_back((QueryBagResult *) bagged_seed);
					}
					errcode = callProcedure(proc_code, envs_sections);
					if (errcode != 0) return errcode;
					QueryResult *res;
					errcode = qres->pop(res);
					if (errcode != 0) return errcode;
					break;
				}
				else {
					*ec << "[QE] ERROR! DELETE argument must consist of QREFERENCE";
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}	
				*ec << "[QE] Object deleted";
			}
			*ec << "[QE] delete operation complete - QueryNothingResult created";
			final = new QueryNothingResult();
			break;
		}
		default: { // unOperation type not known
			*ec << "[QE] ERROR! Unary operation type not known";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
		}
	}
	return 0;
}

int QueryExecutor::algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final) {
	int errcode;
	if ((op == AlgOpNode::plus) || (op == AlgOpNode::minus) || (op == AlgOpNode::times) || (op == AlgOpNode::divide)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		if ((derefL->type() == QueryResult::QINT) || (derefL->type() == QueryResult::QDOUBLE)) {
			switch (op) {
				case AlgOpNode::plus: {
					*ec << "[QE] + operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->plus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->plus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::minus: {
					*ec << "[QE] - operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->minus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->minus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::times: {
					*ec << "[QE] * operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->times(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->times(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::divide: {
					*ec << "[QE] / operation";
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
					*ec << "[QE] ERROR! + arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
				}
				case AlgOpNode::minus: {
					*ec << "[QE] ERROR! - arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
				}
				case AlgOpNode::times: {
					*ec << "[QE] ERROR! * arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
				}
				case AlgOpNode::divide: {
					*ec << "[QE] ERROR! / arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
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
				*ec << "[QE] = operation";
				bool bool_tmp = derefL->equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::neq: {
				*ec << "[QE] != operation";
				bool bool_tmp = derefL->not_equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::lt: {
				*ec << "[QE] < operation";
				bool bool_tmp = derefL->less_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::gt: {
				*ec << "[QE] > operation";
				bool bool_tmp = derefL->greater_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::le: {
				*ec << "[QE] <= operation";
				bool bool_tmp = derefL->less_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::ge: {
				*ec << "[QE] >= operation";
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
					*ec << "[QE] OR operation";
					bool bool_tmp = (bool_l || bool_r);
					final = new QueryBoolResult(bool_tmp);
					break;
				}
				case AlgOpNode::boolAnd: {
					*ec << "[QE] AND operation";
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
					*ec << "[QE] ERROR! OR arguments must be BOOLEAN";
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
				}
				case AlgOpNode::boolAnd: {
					*ec << "[QE] ERROR! AND arguments must be BOOLEAN";
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
				}
				default : { break; }
			}
		}
		return 0;
	}
	else if ((op == AlgOpNode::bagUnion) || (op == AlgOpNode::bagIntersect) || (op == AlgOpNode::bagMinus)) {
		QueryResult *leftBag = new QueryBagResult();
		QueryResult *rightBag = new QueryBagResult();
		final = new QueryBagResult();
		leftBag->addResult(lArg);
		rightBag->addResult(rArg);
		if (op == (AlgOpNode::bagUnion)) {
			*ec << "[QE] BAG_UNION operation";
			((QueryBagResult *) final)->addResult(leftBag);
			((QueryBagResult *) final)->addResult(rightBag);
			return 0;
		} else {
			if (op == (AlgOpNode::bagIntersect))
				*ec << "[QE] BAG_INTERSECT operation";
			if (op == (AlgOpNode::bagMinus))
				*ec << "[QE] BAG_MINUS operation";
			final = new QueryBagResult();
			QueryBagResult *firstSorted;
			errcode = ((QueryBagResult *) leftBag)->sortBag(firstSorted);
			if (errcode != 0) return errcode;
			QueryBagResult *secondSorted;
			errcode = ((QueryBagResult *) rightBag)->sortBag(secondSorted);
			if (errcode != 0) return errcode;
			while ((firstSorted->size()) != 0) {
				QueryResult *first_elem;
				QueryResult *second_elem;
				if ((secondSorted->size()) != 0) {
					errcode = firstSorted->at(0,first_elem);
					if (errcode != 0) return errcode;
					errcode = secondSorted->at(0,second_elem);
					if (errcode != 0) return errcode;
					if (first_elem->equal(second_elem)) {
						if (op == (AlgOpNode::bagIntersect))
							((QueryBagResult *) final)->addResult(first_elem);
						errcode = firstSorted->getResult(first_elem);
						if (errcode != 0) return errcode;
						errcode = secondSorted->getResult(second_elem);
						if (errcode != 0) return errcode;
					}
					else if (first_elem->sorting_less_eq(second_elem)) {
						if (op == (AlgOpNode::bagMinus))
							((QueryBagResult *) final)->addResult(first_elem);
						errcode = firstSorted->getResult(first_elem);
						if (errcode != 0) return errcode;
					}
					else {
						errcode = secondSorted->getResult(second_elem);
						if (errcode != 0) return errcode;
					}
				}
				else {
					errcode = firstSorted->at(0,first_elem);
					if (errcode != 0) return errcode;
					if (op == (AlgOpNode::bagMinus))
						((QueryBagResult *) final)->addResult(first_elem);
					errcode = firstSorted->getResult(first_elem);
					if (errcode != 0) return errcode;
				}
			}
			return 0;
		}
	}
	else if (op == AlgOpNode::comma) {
		*ec << "[QE] COMMA operation";
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
		*ec << "[QE] SEMICOLON operation";
		final = rArg;
		return 0;
	}
	else if (op == AlgOpNode::insert) {
		*ec << "[QE] INSERT operation";
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			*ec << "[QE] ERROR! Left argument of insert operation must have size 1";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		QueryResult* outer;
		errcode = ((QueryBagResult *) lBag)->at(0, outer);
		if (errcode != 0) return errcode;
		int leftResultType = outer->type();
		if (leftResultType != QueryResult::QREFERENCE) {
			*ec << "[QE] ERROR! Left argument of insert operation must consist of QREFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
		ObjectPointer *optrOut;
		errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut); 
		if (errcode != 0) {
			*ec << "[QE] insert operation - Error in getObjectPointer.";
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
		string object_name = optrOut->getName();
		if (!system_privilige_checking && 
			assert_privilige(Privilige::MODIFY_PRIV, object_name) == false) {
		    final = new QueryNothingResult();
		    /*
		    ofstream out("privilige.debug", ios::app);
		    out << "modify " << object_name << endl;
		    out.close();		    
		    */
		    return 0;
		}				
		*ec << "[QE] insert operation - getObjectPointer on leftArg done";
		DataValue* db_value = optrOut->getValue();
		*ec << "[QE] insert operation - Value taken";
		int vType = db_value->getType();
		if (vType != Store::Vector) {
			*ec << "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
		vector<LogicalID*> *insVector = db_value->getVector();
		if (insVector == NULL) {
			*ec << "[QE] insert operation - insVector == NULL";
			*ec << (ErrQExecutor | EQEUnexpectedErr);
			return ErrQExecutor | EQEUnexpectedErr;
		}
		*ec << "[QE] Vector taken";
		ec->printf("[QE] Vec.size = %d\n", insVector->size());
		QueryResult *rBag = new QueryBagResult();
		rBag->addResult(rArg);
		for (unsigned int i = 0; i < rBag->size(); i++) {
			ec->printf("[QE] trying to INSERT %d element of rightArg into leftArg\n", i);
			QueryResult *toInsert;
			errcode = ((QueryBagResult *) rBag)->at(i, toInsert);
			if (errcode != 0) return errcode;
			ObjectPointer *optrIn;
			LogicalID *lidIn;
			int rightResultType = toInsert->type();
			switch (rightResultType) {
				case QueryResult::QREFERENCE: {
					lidIn = ((QueryReferenceResult *) toInsert)->getValue();
					errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn); 
					if (errcode != 0) {
						*ec << "[QE] insert operation - Error in getObjectPointer.";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					string object_name = optrIn->getName();
					/*if (!system_privilige_checking && 
							assert_privilige(Privilige::MODIFY_PRIV, object_name) == false) {
					    	final = new QueryNothingResult();
						//ofstream out("privilige.debug", ios::app);
						//out << "create " << object_name << endl;
						//out.close();					    
					    	return 0;
					}*/
					ec->printf("[QE] getObjectPointer on %d element of rightArg done\n", i);
					if (optrIn->getIsRoot()) {
						errcode = tr->removeRoot(optrIn); 
						if (errcode != 0) {
							*ec << "[QE] insert operation - Error in removeRoot.";
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						*ec << "[QE] insert operation - Root removed";
					}
					else {
						*ec << "[QE] ERROR in insert operation - right argument must be Root";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return ErrQExecutor | EQEUnexpectedErr;
					}
					break;
				}
				case QueryResult::QBINDER: {
					errcode = this->objectFromBinder(toInsert, optrIn);
					if (errcode != 0) return errcode;
					lidIn = (optrIn->getLogicalID());
					break;
				}
				default: {
					*ec << "[QE] Right argument of insert operation must consist of QREFERENCE or QBINDER";
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}
			}
			*ec << "[QE] Inserting the object";
			insVector->push_back(lidIn);
			ec->printf("[QE] New Vec.size = %d\n", insVector->size());
		}
		DBDataValue *dbDataVal = new DBDataValue();
		dbDataVal->setVector(insVector);
		*ec << "[QE] dataValue: setVector done";
		db_value = dbDataVal;
		errcode = tr->modifyObject(optrOut, db_value); 
		if (errcode != 0) {
			*ec << "[QE] insert operation - Error in modifyObject.";
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
		*ec << "[QE] INSERT operation Done";
		final = new QueryNothingResult();
		return 0;
	}
	else if (op == AlgOpNode::assign) {
		*ec << "[QE] ASSIGN operation";
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			*ec << "[QE] ERROR! operator <assign> expects that left argument is single REFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		errcode = ((QueryBagResult *) lBag)->at(0, lArg);
		if (errcode != 0) return errcode;
		int leftResultType = lArg->type();
		if (leftResultType != QueryResult::QREFERENCE) {
			if (leftResultType != QueryResult::QVIRTUAL) {
				*ec << "[QE] ERROR! operator <assign> expects that left argument is REFERENCE";
				*ec << (ErrQExecutor | ERefExpected);
				return ErrQExecutor | ERefExpected;
			}
			else {
				//TODO watpie by to dzialalo w pelni tak jak powinno
				LogicalID *view_def = ((QueryVirtualResult*) lArg)->view_def;
				vector<QueryResult *> seeds = ((QueryVirtualResult*) lArg)->seeds;
				string proc_code = "";
				string proc_param = "";
				errcode = getOn_procedure(view_def, "on_update", proc_code, proc_param);
				if (errcode != 0) return errcode;
				if (proc_code == "") {
					*ec << "[QE] operator <assign> - this VirtualObject doesn't have this operation defined";
					*ec << (ErrQExecutor | EOperNotDefined);
					return ErrQExecutor | EOperNotDefined;
				}
				vector<QueryBagResult*> envs_sections;
				//TODO tu pewnie trzeba jakos lepiej stos inicjowac przed wywolaniem virtual_objects
				for (int k = ((seeds.size()) - 1); k >= 0; k-- ) {
					QueryResult *bagged_seed = new QueryBagResult();
					((QueryBagResult *) bagged_seed)->addResult(seeds.at(k));
					envs_sections.push_back((QueryBagResult *) bagged_seed);
				}
				QueryResult *param_binder = new QueryBinderResult(proc_param, rArg);
				QueryResult *param_bag = new QueryBagResult();
				((QueryBagResult *) param_bag)->addResult(param_binder);
				envs_sections.push_back((QueryBagResult *) param_bag);
				
				errcode = callProcedure(proc_code, envs_sections);
				if (errcode != 0) return errcode;
				QueryResult *res;
				errcode = qres->pop(res);
				if (errcode != 0) return errcode;
			}
		}
		else {
			LogicalID *old_lid = ((QueryReferenceResult *) lArg)->getValue();
			ObjectPointer *old_optr;
			errcode = tr->getObjectPointer (old_lid, Store::Write, old_optr); 
			if (errcode != 0) {
				*ec << "[QE] operator <assign>: error in getObjectPointer()";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string object_name = old_optr->getName();
			if (!system_privilige_checking && assert_privilige(Privilige::MODIFY_PRIV, object_name) == false) {
				final = new QueryNothingResult();
				return 0;
			}		
			*ec << "[QE] operator <assign>: getObjectPointer on left argument done";
			DBDataValue *dbValue = new DBDataValue();
			QueryResult *derefed;
			errcode = this->derefQuery(rArg, derefed);
			if (errcode != 0) return errcode;
			int derefed_type = derefed->type();
			switch (derefed_type) {
				case QueryResult::QINT: {
					int intValue = ((QueryIntResult *) derefed)->getValue();
					dbValue->setInt(intValue);
					*ec << "[QE] < query := ('integer'); > operation";
					break;
				}
				case QueryResult::QDOUBLE: {
					double doubleValue = ((QueryDoubleResult *) derefed)->getValue();
					dbValue->setDouble(doubleValue);
					*ec << "[QE] < query := ('double'); > operation";
					break;
				}
				case QueryResult::QSTRING: {
					string stringValue = ((QueryStringResult *) derefed)->getValue();
					dbValue->setString(stringValue);
					*ec << "[QE] < query := ('string'); > operation";
					break;
				}
				case QueryResult::QREFERENCE: {
					LogicalID* refValue = ((QueryReferenceResult *) derefed)->getValue();
					dbValue->setPointer(refValue);
					*ec << "[QE] < query := ('reference'); > operation";
					break;
				}
				case QueryResult::QSTRUCT: {
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
					*ec << "[QE] < query := ('struct'); > operation";
					break;
				}
				case QueryResult::QBINDER: {
					vector<LogicalID*> vectorValue;
					ObjectPointer *optr_tmp;
					errcode = this->objectFromBinder(derefed, optr_tmp);
					if (errcode != 0) return errcode;
					LogicalID *lid_tmp = optr_tmp->getLogicalID();
					vectorValue.push_back(lid_tmp);
					dbValue->setVector(&vectorValue);
					*ec << "[QE] < query := ('binder'); > operation";
					break;
				}
				default: {
					*ec << "[QE] operator <assign>: error, bad type right argument evaluated by executeRecQuery";
					*ec << (ErrQExecutor | EOtherResExp);
					return ErrQExecutor | EOtherResExp;
				}
			}
			DataValue* value = dbValue;
			errcode = tr->modifyObject(old_optr, value); 
			if (errcode != 0) {
				*ec << "[QE] operator <assign>: error in modifyObject()";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			*ec << "[QE] operator <assign>: value of the object was changed";
			ExtendedType rArg_extType = Store::None;
			if (rArg->isReferenceValue()) {
				QueryResult *rArg_ref;
				errcode = rArg->getReferenceValue(rArg_ref);
				if (errcode != 0) return errcode;
				LogicalID *rArg_lid = ((QueryReferenceResult *)rArg_ref)->getValue();
				ObjectPointer *rArg_optr;
				errcode = tr->getObjectPointer (rArg_lid, Store::Read, rArg_optr); 
				if (errcode != 0) {
					*ec << "[QE] operator <assign>: error in getObjectPointer()";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				rArg_extType = (rArg_optr->getValue())->getSubtype();
			}
			(old_optr->getValue())->setSubtype(rArg_extType);
			errcode = tr->modifyObject(old_optr, old_optr->getValue()); 
			if (errcode != 0) {
				*ec << "[QE] operator <assign>: error in modifyObject()";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
		}
		*ec << "[QE] ASSIGN operation Done";
		final = new QueryNothingResult();
		return 0;
	}
	else { // algOperation type not known
		*ec << "[QE] ERROR! Algebraic operation type not known";
		*ec << (ErrQExecutor | EUnknownNode);
		return ErrQExecutor | EUnknownNode;
	}
	return 0;
}

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *rRes, QueryResult *&partial) {
	*ec << "[QE] combine() function applied to the partial results";
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			*ec << "[QE] combine(): NonAlgebraic operator <dot>";
			partial->addResult(rRes);
			break;
		}
		case NonAlgOpNode::join: {
			*ec << "[QE] combine(): NonAlgebraic operator <join>";
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
			*ec << "[QE] combine(): NonAlgebraic operator <where>";
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				if (tmp_bool_value)
					partial->addResult(curr);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <where> is not boolean!";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case NonAlgOpNode::orderBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <orderBy>";
			QueryResult *derefR;
			errcode = this->derefQuery(rRes,derefR);
			if (errcode != 0) return errcode;
			QueryResult *currRow = new QueryStructResult(curr, derefR);
			partial->addResult(currRow);
			break;
		}
		case NonAlgOpNode::exists: {
			*ec << "[QE] combine(): NonAlgebraic operator <exists>";
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value);
				partial->addResult(tmp_result);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			*ec << "[QE] combine(): NonAlgebraic operator <forAll>";
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value);
				partial->addResult(tmp_result);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <forAll> is not boolean!";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case NonAlgOpNode::forEach: {
			*ec << "[QE] combine(): NonAlgebraic operator <forEach>";
			break;
		}
		default: {
			*ec << "[QE] combine(): unknown NonAlgebraic operator!";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
			break;
		}
	}
	return 0;
}

int QueryExecutor::merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final) {
	*ec << "[QE] merge() function applied to the partial results, creating final result";
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			*ec << "[QE] merge(): NonAlgebraic operator <dot>";
			final = partial;
			break;
		}
		case NonAlgOpNode::join: {
			*ec << "[QE] merge(): NonAlgebraic operator <join>";
			final = partial;
			break;
		}
		case NonAlgOpNode::where: {
			*ec << "[QE] merge(): NonAlgebraic operator <where>";
			final = partial;
			break;
		}
		case NonAlgOpNode::orderBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <orderBy>";
			QueryResult *tmp_seq = new QuerySequenceResult();
			errcode = ((QuerySequenceResult *) tmp_seq)->sortCollection(partial);
			if (errcode != 0) return errcode;
			errcode = ((QuerySequenceResult *) tmp_seq)->postSort(final);
			if (errcode != 0) return errcode;
			break;
		}
		case NonAlgOpNode::exists: {
			*ec << "[QE] merge(): NonAlgebraic operator <exists>";
			bool tmp_bool = false;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					*ec << "[QE] merge(): ERROR! operator <exists> expects boolean type arguments";
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
				}
				errcode = (tmp_result->getBoolValue(current_bool));
				if (errcode != 0) return errcode;
				if (current_bool)
					tmp_bool = true;
			}
			QueryBoolResult * tmp_bool_res = new QueryBoolResult(tmp_bool);
			final->addResult(tmp_bool_res);
			break;
		}
		case NonAlgOpNode::forAll: {
			*ec << "[QE] merge(): NonAlgebraic operator <forAll>";
			bool tmp_bool = true;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					*ec << "[QE] merge(): ERROR! operator <forAll> expects boolean type arguments";
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
				}
				errcode = (tmp_result->getBoolValue(current_bool));
				if (errcode != 0) return errcode;
				if (not current_bool)
					tmp_bool = false;
			}
			QueryBoolResult *tmp_bool_res = new QueryBoolResult(tmp_bool);
			final->addResult(tmp_bool_res);
			break;
		}
		case NonAlgOpNode::forEach: {
			*ec << "[QE] merge(): NonAlgebraic operator <forEach>";
			final = partial;
			break;
		}
		default: {
			*ec << "[QE] merge(): unknown NonAlgebraic operator!";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
			break;
		}
	}
	return 0;
}

int QueryExecutor::isIncluded(QueryResult *elem, QueryResult *set, bool &score) {
	*ec << "[QE] isIncluded()";
	int errcode;
	if (set->type() != QueryResult::QBAG) {
		*ec << "[QE] isIncluded(): ERROR! set argument must be BAG";
		*ec << (ErrQExecutor | EOtherResExp);
		return ErrQExecutor | EOtherResExp;
	}
	bool tmp_bool = false;
	for (unsigned int i = 0; i < set->size(); i++) {
		QueryResult *tmp_res;
		errcode = ((QueryBagResult *) set)->at(i, tmp_res);
		if (errcode != 0) return errcode;
		bool eq = elem->equal(tmp_res);
		if (eq)
			tmp_bool = true;
	}
	score = tmp_bool;
	return 0;
}

int QueryExecutor::abort() {
	*ec << "[QE] abort()";
	if (inTransaction) {
		int errcode = tr->abort();
		inTransaction = false;
		if (errcode != 0) {
			*ec << "[QE] Error in abort()";
			return errcode;
		}
		*ec << "[QE] Transaction aborted succesfully. Done!";
	}
	else
		*ec << "[QE] Transaction not opened. Closing nevertheless. Done!\n";
	return 0;
}

void QueryExecutor::antyStarveFunction(int errcode) {
	if (errcode == ErrTManager | EDeadlock) {
		antyStarve = true;
		*ec << "[QE] EDeadlock error found => AntyStarve mode is on";
	}
}

int QueryExecutor::objectFromBinder(QueryResult *res, ObjectPointer *&newObject) {
	int errcode;
	*ec << "[QE] objectFromBinder()";
	if ((res->type()) != QueryResult::QBINDER) {
		*ec << "[QE] objectFromBinder() expected a binder, got something else";
		*ec << (ErrQExecutor | EOtherResExp);
		return ErrQExecutor | EOtherResExp;
	}
	string binderName = ((QueryBinderResult *) res)->getName();
	QueryResult *binderItem = ((QueryBinderResult *) res)->getItem();
	DBDataValue *dbValue = new DBDataValue();
	int binderItemType = binderItem->type();
	switch (binderItemType) {
		case QueryResult::QINT: {
			int intValue = ((QueryIntResult *) binderItem)->getValue();
			dbValue->setInt(intValue);
			*ec << "[QE] objectFromBinder(): value is INT type";
			break;
		}
		case QueryResult::QDOUBLE: {
			double doubleValue = ((QueryDoubleResult *) binderItem)->getValue();
			dbValue->setDouble(doubleValue);
			*ec << "[QE] objectFromBinder(): value is DOUBLE type";
			break;
		}
		case QueryResult::QSTRING: {
			string stringValue = ((QueryStringResult *) binderItem)->getValue();
			dbValue->setString(stringValue);
			*ec << "[QE] objectFromBinder(): value is STRING type";
			break;
		}
		case QueryResult::QREFERENCE: {
			LogicalID* refValue = ((QueryReferenceResult *) binderItem)->getValue();
			dbValue->setPointer(refValue);
			*ec << "[QE] objectFromBinder(): value is REFERENCE type";
			break;
		}
		case QueryResult::QBINDER: {
			vector<LogicalID*> vectorValue;
			ObjectPointer *optr;
			errcode = this->objectFromBinder(binderItem, optr);
			if (errcode != 0) return errcode;
			LogicalID *lid = optr->getLogicalID();
			*ec << "[QE] objectFromBinder(): logical ID received";
			vectorValue.push_back(lid);
			*ec << "[QE] objectFromBinder(): added to a vector";
			dbValue->setVector(&vectorValue);
			*ec << "[QE] objectFromBinder(): value is BINDER type";
			break;
		}
		case QueryResult::QBAG: {
			vector<LogicalID*> vectorValue;
			for (unsigned int i=0; i < (binderItem->size()); i++) {
				QueryResult *curr;
				errcode = ((QueryStructResult *) binderItem)->at(i, curr);
				if (errcode != 0) return errcode;
				if (curr->type() != QueryResult::QREFERENCE) {
					*ec << "[QE] objectFromBinder() QueryReference expected";
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}
				LogicalID *lid = ((QueryReferenceResult *) curr)->getValue();
				*ec << "[QE] objectFromBinder(): logical ID received";
				vectorValue.push_back(lid);
				*ec << "[QE] objectFromBinder(): added to a vector";
			}
			dbValue->setVector(&vectorValue);
			*ec << "[QE] objectFromBinder(): value is BAG type";
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
				*ec << "[QE] objectFromBinder(): logical ID received";
				vectorValue.push_back(lid);
				*ec << "[QE] objectFromBinder(): added to a vector";
			}
			dbValue->setVector(&vectorValue);
			*ec << "[QE] objectFromBinder(): value is STRUCT type";
			break;
		}
		default: {
			*ec << "[QE] objectFromBinder(): bad type item in a binder";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
	}
	DataValue* value;
	value = dbValue;
	if ((errcode = tr->createObject(binderName, value, newObject)) != 0) {
		*ec << "[QE] Error in createObject";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	return 0;
}

int QueryExecutor::procCheck(unsigned int qSize, LogicalID *lid, string &code, vector<string> &prms, int &founded) {
	*ec << "[QE] checking if this LogicalID point procedure we look for";
	int errcode;
	ObjectPointer *optr;
	
	errcode = tr->getObjectPointer (lid, Store::Read, optr); 
	if (errcode != 0) {
		*ec << "[QE] procCheck - Error in getObjectPointer.";
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
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr)) != 0) {
				*ec << "[QE] procCheck - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			if (tmp_vType == Store::String) {
				if (tmp_name == "ProcBody") {
					tmp_code = tmp_data_value->getString();
					procBody_count++;
				}
				else if (tmp_name == "Param") {
					tmp_prms.push_back(tmp_data_value->getString());
					params_count++;
				}
				else others_count++;
			}
		}
		if ((procBody_count == 1) && (params_count == qSize) && (others_count == 0)) {
			*ec << "[QE] procCheck - success, this is procedure we've looked for";
			if (founded != 0) {
				*ec << "[QE] error there should be only one procedure in a scope with the same name and number of parameters";
				*ec << (ErrQExecutor | EProcNotSingle);
				return ErrQExecutor | EProcNotSingle;
			}
			founded++; 
			code = tmp_code;
			prms = tmp_prms;
		}
	}
	return 0;
}

int QueryExecutor::checkViewAndGetVirtuals(LogicalID *lid, string &name, string &code) {
	int errcode;
	string actual_name = "";
	int actual_name_count = 0;
	ObjectPointer *optr;
	errcode = tr->getObjectPointer (lid, Store::Read, optr);
	if (errcode != 0) {
		*ec << "[QE] checkViewAndGetVirtuals - Error in getObjectPointer.";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	DataValue* data_value = optr->getValue();
	int vType = data_value->getType();
	ExtendedType extType = data_value->getSubtype();
	if ((vType == Store::Vector) && (extType == Store::View)) {
		vector<LogicalID*>* tmp_vec = (data_value->getVector());
		int vec_size = tmp_vec->size();
		vector<LogicalID*> tmp_vec_copy;
		for (int i = 0; i < vec_size; i++ ) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr)) != 0) {
				*ec << "[QE] checkViewAndGetVirtuals - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			ExtendedType tmp_extType = tmp_data_value->getSubtype();
			if ((tmp_name == "virtual_objects") && (tmp_vType == Store::String) && (tmp_extType == Store::None)) {
				actual_name = tmp_data_value->getString();
				actual_name_count++;
			}
			else tmp_vec_copy.push_back(tmp_logID);
		}
		if (actual_name_count == 0) {
			*ec << "[QE] checkViewAndGetVirtuals error: \"virtual_objects\" not found in this view ";
			*ec << (ErrQExecutor | EBadViewDef);
			return ErrQExecutor | EBadViewDef;
		}
		else if (actual_name_count > 1) {
			*ec << "[QE] checkViewAndGetVirtuals error: multiple \"virtual_objects\" found in this view";
			*ec << (ErrQExecutor | EBadViewDef);
			return ErrQExecutor | EBadViewDef;
		}
		else if ((name != "") && (name != actual_name)) {
			*ec << "[QE] checkViewAndGetVirtuals error: \"virtual_objects\" have different name then we've loooked for";
			*ec << (ErrQExecutor | EBadViewDef);
			return ErrQExecutor | EBadViewDef;
		}
		else name = actual_name;
		int vec_copy_size = tmp_vec_copy.size();
		int proc_code_founded = 0;
		string proc_code = "";
		for (int i = 0; i < vec_copy_size; i++) {
			LogicalID *tmp_logID = tmp_vec_copy.at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr)) != 0) {
				*ec << "[QE] checkViewAndGetVirtuals - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			ExtendedType tmp_extType = tmp_data_value->getSubtype();
			if ((tmp_vType == Store::Vector) && (tmp_extType == Store::Procedure)) {
				if (tmp_name == name) {
					vector<LogicalID*>* tmp_vec_inner = (tmp_data_value->getVector());
					int vec_size_inner = tmp_vec_inner->size();
					if (vec_size_inner != 1) {
						*ec << "[QE] checkViewAndGetVirtuals error: \"virtual_objects\" procedure wrong format";
						*ec << (ErrQExecutor | EBadViewDef);
						return ErrQExecutor | EBadViewDef;
					}
					LogicalID *proc_code_lid = tmp_vec_inner->at(0);
					ObjectPointer *proc_code_optr;
					if ((errcode = tr->getObjectPointer(proc_code_lid, Store::Read, proc_code_optr)) != 0) {
						*ec << "[QE] checkViewAndGetVirtuals - Error in getObjectPointer";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					string proc_code_name = proc_code_optr->getName();
					DataValue* proc_code_data_value = proc_code_optr->getValue();
					int proc_code_vType = proc_code_data_value->getType();
					ExtendedType proc_code_extType = proc_code_data_value->getSubtype();
					if ((proc_code_vType == Store::String) && (proc_code_extType == Store::None) && (proc_code_name == "ProcBody")) {
						proc_code = proc_code_data_value->getString();
					}
					else {
						*ec << "[QE] checkViewAndGetVirtuals error: \"virtual_objects\" procedure wrong format";
						*ec << (ErrQExecutor | EBadViewDef);
						return ErrQExecutor | EBadViewDef;
					}
					proc_code_founded++;
				}
			}
			else if ((tmp_vType != Store::Vector) || (tmp_extType != Store::View)) {
				*ec << "[QE] checkViewAndGetVirtuals error: something unexpected in view object found";
				*ec << (ErrQExecutor | EBadViewDef);
				return ErrQExecutor | EBadViewDef;
			}
		}
		if (proc_code_founded == 0) {
			*ec << "[QE] checkViewAndGetVirtuals error: \"virtual_objects\" procedure not found";
			*ec << (ErrQExecutor | EBadViewDef);
			return ErrQExecutor | EBadViewDef;
		} 
		else if (proc_code_founded > 1) {
			*ec << "[QE] checkViewAndGetVirtuals error: multiple \"virtual_objects\" procedures found";
			*ec << (ErrQExecutor | EBadViewDef);
			return ErrQExecutor | EBadViewDef;
		}
		else code = proc_code;
	}
	else {
		*ec << "[QE] checkViewAndGetVirtuals error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return ErrQExecutor | EBadViewDef;
	}
	return 0;
}





int QueryExecutor::getSubviews(LogicalID *lid, string vo_name, vector<LogicalID *> &subviews, vector<LogicalID *> &others) {
	int errcode;
	ObjectPointer *optr;
	errcode = tr->getObjectPointer (lid, Store::Read, optr);
	if (errcode != 0) {
		*ec << "[QE] getSubviews - Error in getObjectPointer.";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	DataValue* data_value = optr->getValue();
	int vType = data_value->getType();
	ExtendedType extType = data_value->getSubtype();
	if ((vType == Store::Vector) && (extType == Store::View)) {
		vector<LogicalID*>* tmp_vec = (data_value->getVector());
		int vec_size = tmp_vec->size();
		for (int i = 0; i < vec_size; i++ ) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr)) != 0) {
				*ec << "[QE] getSubviews - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			ExtendedType tmp_extType = tmp_data_value->getSubtype();
			if ((tmp_vType == Store::Vector) && (tmp_extType == Store::View)) {
				subviews.push_back(tmp_logID);
			}
			else if ((tmp_vType == Store::Vector) && (tmp_extType == Store::Procedure) &&
				(tmp_name != "on_update") && (tmp_name != "on_delete") && (tmp_name != vo_name) && 
				(tmp_name != "on_create") && (tmp_name != "on_retrieve")) {
				others.push_back(tmp_logID);
			}
		}
	}
	else {
		*ec << "[QE] getSubviews error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return ErrQExecutor | EBadViewDef;
	}
	return 0;
}

int QueryExecutor::getOn_procedure(LogicalID *lid, string procName, string &code, string &param) {
	code = "";
	param = "";
	int errcode;
	ObjectPointer *optr;
	errcode = tr->getObjectPointer (lid, Store::Read, optr);
	if (errcode != 0) {
		*ec << "[QE] getOn_procedure - Error in getObjectPointer.";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
	DataValue* data_value = optr->getValue();
	int vType = data_value->getType();
	ExtendedType extType = data_value->getSubtype();
	if ((vType == Store::Vector) && (extType == Store::View)) {
		vector<LogicalID*>* tmp_vec = (data_value->getVector());
		int vec_size = tmp_vec->size();
		int founded = 0;
		for (int i = 0; i < vec_size; i++ ) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr)) != 0) {
				*ec << "[QE] getOn_procedure - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			ExtendedType tmp_extType = tmp_data_value->getSubtype();
			if ((procName == tmp_name) && (tmp_vType == Store::Vector) && (tmp_extType == Store::Procedure)) {
				founded++;
				vector<LogicalID*>* proc_vec = tmp_data_value->getVector();
				int proc_vec_size = proc_vec->size();
				if ((proc_vec_size == 0) || (proc_vec_size > 2)) {
					ec->printf("[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
					*ec << (ErrQExecutor | EBadViewDef);
					return ErrQExecutor | EBadViewDef;
				}
				LogicalID *first_logID = proc_vec->at(0);
				ObjectPointer *first_optr;
				if ((errcode = tr->getObjectPointer(first_logID, Store::Read, first_optr)) != 0) {
					*ec << "[QE] getOn_procedure - Error in getObjectPointer";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				string first_name = first_optr->getName();
				DataValue* first_data_value = first_optr->getValue();
				int first_vType = first_data_value->getType();
				ExtendedType first_extType = first_data_value->getSubtype();
				if ((first_name == "ProcBody") && (first_vType == Store::String) && (first_extType == Store::None)) {
					code = first_data_value->getString();
				}
				else if ((first_name == "Param") && (first_vType == Store::String) && (first_extType == Store::None)) {
					param = first_data_value->getString();
				}
				else {
					ec->printf("[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
					*ec << (ErrQExecutor | EBadViewDef);
					return ErrQExecutor | EBadViewDef;
				}
				
				if (proc_vec_size == 2) {
					LogicalID *second_logID = proc_vec->at(1);
					ObjectPointer *second_optr;
					if ((errcode = tr->getObjectPointer(second_logID, Store::Read, second_optr)) != 0) {
						*ec << "[QE] getOn_procedure - Error in getObjectPointer";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					string second_name = second_optr->getName();
					DataValue* second_data_value = second_optr->getValue();
					int second_vType = second_data_value->getType();
					ExtendedType second_extType = second_data_value->getSubtype();
					if ((second_name == "ProcBody") && (second_vType == Store::String) && (second_extType == Store::None) && code == "") {
						code = second_data_value->getString();
					}
					else if ((second_name == "Param") && (second_vType == Store::String) && (second_extType == Store::None) && param == "") {
						param = second_data_value->getString();
					}
					else {
						ec->printf("[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
						*ec << (ErrQExecutor | EBadViewDef);
						return ErrQExecutor | EBadViewDef;
					}
				}
				
			}
		}
		if (founded > 1) {
			ec->printf("[QE] getOn_procedure error: Multiple %s procedures found\n", procName.c_str());
			*ec << (ErrQExecutor | EBadViewDef);
			return ErrQExecutor | EBadViewDef;
		}
	}
	else {
		*ec << "[QE] getOn_procedure error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return ErrQExecutor | EBadViewDef;
	}
	return 0;
}

int QueryExecutor::callProcedure(string code, vector<QueryBagResult*> sections) {
	int errcode;
	QueryParser* tmpQP = new QueryParser();
	TreeNode* tmpTN;
	errcode = tmpQP->parseIt(code, tmpTN);
	if (errcode != 0) {
		*ec << "[QE] TNCALLPROC error while parsing procedure code";
		return errcode;
	}
	
	envs->actual_prior++;
	for (unsigned int i = 0; i < sections.size(); i++) {
		errcode = envs->push((QueryBagResult *) sections.at(i));
		if (errcode != 0) return errcode;
	}
		
	if(tmpTN != NULL) {
		errcode = executeRecQuery(tmpTN);
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
	}
	
	for (unsigned int i = 0; i < sections.size(); i++) {
		errcode = envs->pop();
		if (errcode != 0) return errcode;
	}
	envs->actual_prior--;
	
	delete tmpTN;
	delete tmpQP;
	return 0;
}

int QueryExecutor::deVirtualize(QueryResult *arg, QueryResult *&res) {
	*ec << "[QE] deVirtualize()";
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
				errcode = this->deVirtualize(tmp_item, tmp_res);
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
				errcode = this->deVirtualize(tmp_item, tmp_res);
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
				errcode = this->deVirtualize(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->deVirtualize(((QueryBinderResult *) arg)->getItem(), tmp_item);
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
			break;
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		case QueryResult::QVIRTUAL: {
			if ((((QueryVirtualResult *)arg)->seeds.size() > 0) && (((QueryVirtualResult *)arg)->seeds.at(0) != NULL))
				res = ((QueryVirtualResult *)arg)->seeds.at(0);
			break;
		}
		default: {
			*ec << "[QE] ERROR in deVirtualize() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
	}
	return 0;
}

QueryExecutor::~QueryExecutor() {
	if (inTransaction) tr->abort();
	inTransaction = false;
	delete envs;
	delete qres;
	delete session_data;
	*ec << "[QE] QueryExecutor shutting down\n";
	delete ec;
	delete QueryBuilder::getHandle();
}



/* RESULT STACK */


ResultStack::ResultStack() { ec = new ErrorConsole("QueryExecutor"); }
ResultStack::~ResultStack() { this->deleteAll(); if (ec != NULL) delete ec; }

int ResultStack::push(QueryResult *r) {
	rs.push_back(r);
	*ec << "[QE] Result Stack pushed";
	return 0;
}

int ResultStack::pop(QueryResult *&r){
	if (rs.empty()) {
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	}
	else {
		r=(rs.back());
		rs.pop_back();
	};
	*ec << "[QE] Result Stack popped";
	return 0;
}

bool ResultStack::empty() { return rs.empty(); }
int ResultStack::size() { return rs.size(); }

void ResultStack::deleteAll() {
	for (unsigned int i = 0; i < (rs.size()); i++) {
		delete rs.at(i);
	};
}

    /*
     *	Query Builder begin
     */
    QueryBuilder* QueryBuilder::builder = new QueryBuilder();
    QueryBuilder* QueryBuilder::getHandle() {
	return builder;
    };
    QueryBuilder::QueryBuilder()  {}
    QueryBuilder::~QueryBuilder() {}
    string QueryBuilder::create_user_query(string login, string passwd) {
	string query = "create (\"" + login + "\" as login, \"" + passwd + "\" as password) as xuser";
        return query;
    };
    string QueryBuilder::remove_user_query(string login) {
	string query = "delete (xuser where login = \"" + login + "\")";
        return query;     
    };
    string QueryBuilder::grant_priv_query(string priv, string object, string user, int grant_option) {
	
	char str_grant_option[2];
	if (grant_option) sprintf(str_grant_option, "%d", 1);
	else		  sprintf(str_grant_option, "%d", 0);
	
	string lvalue = "(xuser where login=\"" + user + "\")";
	string rvalue = "(create (\"" + priv + "\" as priv_name, \"" + object + "\" as object_name, "
			+ str_grant_option + " as grant_option) as xprivilige)";
	string oper   = " :< ";

	string query = lvalue + oper + rvalue;
        return query;
    };
    string QueryBuilder::revoke_priv_query(string priv, string object, string user) {
	string user_query = "(xuser where login=\"" + user + "\")";
	string priv_query = "(xprivilige where priv_name=\"" + priv + "\" and object_name=\"" + object + "\")";
	string query = "delete (" + user_query + "." + priv_query + ")";
	return query;     
    };     
    string QueryBuilder::query_for_privilige(string user, string priv, string object) {
	string user_query = "(xuser where login=\"" + user + "\")";
	string priv_query = "(xprivilige where priv_name=\"" + priv + "\" and object_name=\"" + object + "\")";
	string query = "count (" + user_query + "." + priv_query + ") > 0";
	return query;     
    };
    string QueryBuilder::query_for_privilige_grant(string user, string priv, string object) {
	string user_query = "(xuser where login=\"" + user + "\")";
	string priv_query = "(xprivilige where priv_name=\"" + priv + "\" and object_name=\"" + object + "\" and "
			    + "grant_option=1)";
	string query = "count (" + user_query + "." + priv_query + ") > 0";
	return query;     
    };    
    string QueryBuilder::query_for_password(string user, string password) {
	string query = "count (xuser where login=\"" + user + "\" and password=\"" + password + "\") > 0";
	return query;
    };
    /*
     *	Query Builder end
     */

}
