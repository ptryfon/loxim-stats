#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <ios>
#include <list>
#include <map>
//#include <pair>

#include "QueryResult.h"
#include "../QueryParser/ClassNames.h"
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
#include "InterfaceMatcher.h"
#include "TypeCheck/DecisionTable.h"
#include "LoximServer/LoximSession.h"
#include "BindNames.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace Indexes;
using namespace std;
using namespace LoximServer;

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
	//printf("Debug: EQ: tree type is: %d\n", tree->type());
	errcode = executeQuery(tree, result);
	*ec << "[QE] past inner executeQuery";
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
			errcode = (dynamic_cast< IndexDDLNode*>(tree))->execute(session->get_id(), result);
			return errcode;
		} else
		if (! inTransaction) {
			if (nodeType == TreeNode::TNTRANS) {
				if (((TransactNode *) tree)->getOp() == TransactNode::begin) {
					if (antyStarve) {
						ec->printf("[QE] Asking TransactionManager to REOPEN transaction number : %d\n", transactionNumber);
						errcode = TransactionManager::getHandle()->createTransaction(session->get_id(), tr, transactionNumber);
						antyStarve = false;
					}
					else {
						*ec << "[QE] Asking TransactionManager for a NEW transaction";
						errcode = TransactionManager::getHandle()->createTransaction(session->get_id(), tr);
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
					*ec << (ErrQExecutor | ETransactionOpened);
					return (ErrQExecutor | ETransactionOpened);
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
					return (ErrQExecutor | EUnknownNode);
				}
			}
			*result = new QueryNothingResult();
			*ec << "[QE] Nothing else to do. QueryNothingResult created";
			return 0;
		}
		else if (nodeType == (TreeNode::TNVALIDATION)) {
		    UserData *user_data = session->get_user_data();
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
		}	/** Type declarations / definitions, TC nodes */
		else if (nodeType == TreeNode::TNOBJDECL) {
			*ec << "Will try execute object declaration\n";
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
    session->set_user_data(user_data);
};
int QueryExecutor::execute_locally(string query, QueryResult **res) {

    QueryParser parser(session->get_id());
    TreeNode *tree = NULL;
    system_privilige_checking = true;
    parser.parseIt(session->get_id(), query, tree);

    int errcode = envs->pushDBsection();
    if (errcode != 0) return errcode;
    errcode = this->executeRecQuery(tree);
    if (errcode != 0) return errcode;
    errcode = qres->pop(*res);
    if (errcode != 0) return errcode;
    //errcode = envs->popDBsection();
    //if (errcode != 0) return errcode;
/*	Or...: ?
	errcode = envs->popDBsection();
	if (errcode != 0) return errcode;
	errcode = qres->pop(*res);
	if (errcode != 0) return errcode;
*/
    system_privilige_checking = false;
    return 0;
};

int QueryExecutor::execute_locally(string query, QueryResult **res, QueryParser &parser) {
	TreeNode *tree = NULL;

	int parsercode = parser.parseIt(session->get_id(), query, tree);
	if (parsercode != 0) {
		*ec << "exec. locally, parser code aint 0, error! , query: " + query + ". \n";
	}
	*ec << "IN EXEC_LOCALLY: will push dbsection; \n";
	int errcode = envs->pushDBsection();
	if (errcode != 0) {
		*ec << "!!!!! pushDB doesn't work\n";
		return errcode;
	}
	errcode = this->executeRecQuery(tree);
	if (errcode != 0) {
		*ec << "!!!!! executeRecQuery ended with error, so cant popDB section properly!!! so doing it here.\n";
		envs->popDBsection();
		return errcode;
	}
	errcode = envs->popDBsection();
	if (errcode != 0) {*ec << "Error on popDBsection, in exec. locally"; return errcode;}
	errcode = qres->pop(*res);
	if (errcode != 0) {*ec << "Error on pop from qres, in exec. locally"; return errcode;}

	return 0;
}

bool QueryExecutor::is_dba() {
    string login = session->get_user_data()->get_login();
    return "root" == login;
};

bool QueryExecutor::assert_privilige(string priv_name, string object) {
    if (is_dba()) return true;
    string query = QueryBuilder::getHandle()->
			query_for_privilige(session->get_user_data()->get_login(), priv_name, object);
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
			query_for_privilige_grant(session->get_user_data()->get_login(), priv_name, name);

    return assert_bool_query(query);
};

bool QueryExecutor::assert_revoke_priv(string priv_name, string name) {
    if (is_dba()) return true;
    return false;
};

bool QueryExecutor::assert_create_user_priv() {
    if (is_dba()) return true;

    string query = QueryBuilder::getHandle()->
			query_for_privilige(session->get_user_data()->get_login(), Privilige::CREATE_PRIV, "xuser");
    return assert_bool_query(query);
};

bool QueryExecutor::assert_remove_user_priv() {
    if (is_dba()) return true;

    string query = QueryBuilder::getHandle()->
			query_for_privilige(session->get_user_data()->get_login(), Privilige::DELETE_PRIV, "xuser");
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
		return (ErrQExecutor | EEvalStopped);
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
			
			/* not working well yet
			if (((QueryBagResult *)result)->isEmpty())
			{	//check if name refers to an interface object  
			    QueryNode *replaced = NULL;
			    errcode = Schemas::Matcher::QueryForInterfaceObjectName(name, this, ec, tr, replaced);
			    //TODO - errcode
			    if (replaced)
			    {
				ec->printf("[QE] name %s recognized as interface object name\n", name.c_str());
				return executeRecQuery(replaced);
			    //TODO - push methods to ES 
			    }
			}
			*/
			
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
				return (ErrQExecutor | ERefExpected);
			}

			ObjectPointer *optr;
			errcode = tr->getObjectPointer (((QueryReferenceResult*)execution_result)->getValue(), Store::Read, optr, false);
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

			int bindSectionNumber = -1;
			LogicalID* bindClassLid;
			errcode = envs->bindProcedureName(name, queries_size, tr, this, code, params, bindSectionNumber, bindClassLid);
			if (errcode != 0) return errcode;

			if ((code == "") || (bindSectionNumber == -1)) {
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

			// callProcedure wklada na stos sekcje w takiej kolejnosci jak sa w podanym wektorze
			// envs_sections(0) bedzie najnizej, envs_sections(n) bedzie na wierzcholku stosu
			vector<QueryBagResult*> envs_sections;
			envs_sections.push_back((QueryBagResult *)new_envs_sect);

			int oldBindSectionPrior = envs->es_priors[bindSectionNumber];
			LogicalID* oldBindClassLid =  envs->actualBindClassLid;
			envs->es_priors[bindSectionNumber] = envs->actual_prior + 1;
			envs->actualBindClassLid = bindClassLid;

			errcode = callProcedure(code, envs_sections);

			envs->actualBindClassLid = oldBindClassLid;
			envs->es_priors[bindSectionNumber] = oldBindSectionPrior;
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

		case TreeNode::TNTHROWEXCEPTION: {
			*ec << "[QE] Type: TN THROW EXCEPTION (begin)";
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
				*ec << "[QE] USER EXCEPTION THROWN (done)";
				return (ErrUserProgram | EUserWrongParam);
			}
			else {
				*ec << "[QE] UNKNOWN EXCEPTION THROWN (done)";
				return (ErrUserProgram | EUserUnknown);
			}

			return 0;

		}//case TNTHROWEXCEPTION

		case TreeNode::TNVIEW: {
			*ec << "[QE] Type: TNVIEW";

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
				*ec << "[QE] TNVIEW error - execution result is not QueryReference";
				*ec << (ErrQExecutor | ERefExpected);
				return (ErrQExecutor | ERefExpected);
			}
			LogicalID* lid = ((QueryReferenceResult*)execution_result)->getValue();
			errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
			if (errcode != 0) {
				*ec << "[QE] view creating operation - Error in getObjectPointer.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string virt_obj_name = optr->getName();
			if ((virt_obj_name == "on_update") || (virt_obj_name == "on_create")
				|| (virt_obj_name == "on_delete") || (virt_obj_name == "on_retrieve")
				|| (virt_obj_name == "on_navigate") || (virt_obj_name == "on_virtualize")
				|| (virt_obj_name == "on_store") || (virt_obj_name == "on_insert")) {
				*ec << "[QE] TNVIEW error - virtual objects shouldn't be named like \'on_\' procedures";
				*ec << (ErrQExecutor | EBadViewDef);
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
					*ec << "[QE] TNVIEW error - execution result is not QueryReference";
					*ec << (ErrQExecutor | ERefExpected);
					return (ErrQExecutor | ERefExpected);
				}
				lid = ((QueryReferenceResult*)execution_result)->getValue();
				errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
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
				else if (optr_name == "on_insert") on_ins_num++;
				else if (optr_name == "on_navigate") on_nav_num++;
				else if (optr_name == "on_virtualize") on_vir_num++;
				else if (optr_name == "on_store") on_sto_num++;
				vec_lid.push_back(lid);
			}
			if ((on_upd_num > 1) || (on_cre_num > 1) || (on_del_num > 1) || (on_ret_num > 1)
				|| (on_ins_num > 1) || (on_nav_num > 1) || (on_vir_num > 1) || (on_sto_num > 1)) {
				*ec << "[QE] TNVIEW error - multiple \'on_\' procedure definition found";
				*ec << (ErrQExecutor | EBadViewDef);
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
					*ec << "[QE] TNVIEW error - execution result is not QueryReference";
					*ec << (ErrQExecutor | ERefExpected);
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
			
			int errcode2;
			
			if (execution_result->type() != QueryResult::QREFERENCE) {
				*ec << "[QE] TNREGVIEW error - execution result is not QueryReference";
				*ec << (ErrQExecutor | ERefExpected);
				return (ErrQExecutor | ERefExpected);
			}

			ObjectPointer *optr;
			LogicalID *execution_result_lid = ((QueryReferenceResult*)execution_result)->getValue();
			errcode = tr->getObjectPointer (execution_result_lid, Store::Read, optr, false);
			if (errcode != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				*ec << "[QE] register view operation - Error in getObjectPointer.";
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
					*ec << "[QE] register view operation - Error in getObjectPointer.";
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
				*ec << "[QE] register view operation - error in getSystemViewsLID";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			int vecSize_sysvirt = vec_sysvirt->size();
			ec->printf("[QE] %d SystemViews LID by name taken\n", vecSize_sysvirt);
			
			if (vecSize_sysvirt > 0) {
				*ec << "[QE] register view operation error: new virtual objects and a system view have the same name";
				
				errcode = persistDelete(execution_result_lid);
				if(errcode != 0) return errcode;
				
				*ec << (ErrQExecutor | EBadViewDef);
				return (ErrQExecutor | EBadViewDef);
			}
			
			vector<LogicalID*>* vec_virt;
			if ((errcode = tr->getViewsLID(virtual_objects_name, vec_virt)) != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				*ec << "[QE] register view operation - error in getViewsLID";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			int vecSize_virt = vec_virt->size();
			ec->printf("[QE] %d Views LID by name taken\n", vecSize_virt);
			
			if (vecSize_virt > 0) {
				*ec << "[QE] register view operation error: virtual objects name already taken";
				
				errcode = persistDelete(execution_result_lid);
				if(errcode != 0) return errcode;
				
				*ec << (ErrQExecutor | EBadViewDef);
				return (ErrQExecutor | EBadViewDef);
			}
			
			vector<LogicalID*>* vec_roots;
			if ((errcode = tr->getRootsLID(virtual_objects_name, vec_roots)) != 0) {
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				*ec << "[QE] register view operation - error in getRootsLID";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			unsigned int vecSize_roots = vec_roots->size();
			
			if (vecSize_roots > 0) {
				ec->printf("[QE] %d Roots LID by name taken\n", vecSize_roots);
				*ec << "[QE] WARNING, root objects with the same name as new virtual objects found. These objects will be transformed into virtuals.";
				
				string proc_code = "";
				string proc_param = "";
				errcode = getOn_procedure(execution_result_lid, "on_create", proc_code, proc_param);
				if (errcode != 0) return errcode;
				if (proc_code == "") {
					*ec << "[QE] operator <create> - this VirtualObject doesn't have this operation defined";
					
					errcode = persistDelete(execution_result_lid);
					if(errcode != 0) return errcode;
					
					*ec << (ErrQExecutor | EOperNotDefined);
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
					errcode = createNewSections(NULL, (QueryBinderResult*) param_binder, execution_result_lid, envs_sections);
					if (errcode != 0) {
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
				*ec << "[QE] Error in addView";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = tr->addRoot(optr);
			if (errcode != 0) { 
				errcode2 = persistDelete(execution_result_lid);
				if(errcode2 != 0) return errcode2;
				*ec << "[QE] Error in addRoot";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			
			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;

			return 0;
		}//case TNREGVIEW
		
		case TreeNode::TNVIRTUALIZEAS: {
			*ec << "[QE] Type: TNVIRTUALIZEAS";
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
					*ec << "[QE] error in getViewsLID";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				int vecSize_virt = vec_virt->size();
				ec->printf("[QE] %d Views LID by name taken\n", vecSize_virt);
				if (vecSize_virt != 1) {
					*ec << "[QE] bindName error, while searching for a view";
					*ec << (ErrQExecutor | EBadBindName);
					return (ErrQExecutor | EBadBindName);
				}

				referenced_view_lid = vec_virt->at(0);
				errcode = getOn_procedure(referenced_view_lid, "on_virtualize", referenced_view_code, referenced_view_param);
				if (errcode != 0) return errcode;
				if (referenced_view_code == "") {
					*ec << "[QE] operator <virtualize as> - this VirtualObject doesn't have this operation defined";
					*ec << (ErrQExecutor | EOperNotDefined);
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
						errcode = getSubview(sub_lid, virtualizeAsName, referenced_view_lid);
						if (errcode != 0) return errcode;
						if (referenced_view_lid != NULL) {
							errcode = getOn_procedure(referenced_view_lid, "on_virtualize", referenced_view_code, referenced_view_param);
							if (errcode != 0) return errcode;
							if (referenced_view_code == "") {
								*ec << "[QE] operator <virtualize as> - this VirtualObject doesn't have this operation defined";
								*ec << (ErrQExecutor | EOperNotDefined);
								return (ErrQExecutor | EOperNotDefined);
							}
							subqueryResult = tmp_subquery_res;
						}
						else {
							*ec << "[QE] <virtualize as> - error evaluating subquery";
							*ec << (ErrQExecutor | EQEUnexpectedErr);
							return (ErrQExecutor | EQEUnexpectedErr);
						}
					}
					else if (tmp_subquery_res->type() == QueryResult::QREFERENCE) {
						LogicalID *maybe_parent = ((QueryReferenceResult *) tmp_subquery_res)->getValue();

						ObjectPointer *optrOut;
						errcode = tr->getObjectPointer (maybe_parent, Store::Read, optrOut, false);
						if (errcode != 0) {
							*ec << "[QE] <virtualize as> - Error in getObjectPointer.";
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
							*ec << "[QE] <virtualize as> - ERROR! looking for subview in referenced object, object is not vector type";
							*ec << (ErrQExecutor | EOtherResExp);
							return (ErrQExecutor | EOtherResExp);
						}
						vector<LogicalID*> *insVector = db_value->getVector();
						if (insVector == NULL) {
							*ec << "[QE] <virtualize as> - insVector == NULL";
							*ec << (ErrQExecutor | EQEUnexpectedErr);
							return (ErrQExecutor | EQEUnexpectedErr);
						}

						for (unsigned int j = 0; j < insVector->size(); j++) {
							LogicalID *maybe_subview_logID = insVector->at(j);
							ObjectPointer *maybe_subview_optr;
							if ((errcode = tr->getObjectPointer(maybe_subview_logID, Store::Read, maybe_subview_optr, false)) != 0) {
								*ec << "[QE] <virtualize as> - Error in getObjectPointer";
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
								errcode = checkViewAndGetVirtuals(maybe_subview_logID, maybe_subview_name, maybe_subview_code);
								if (errcode != 0) return errcode;
								if (maybe_subview_name == virtualizeAsName) {
									if (referenced_view_lid != NULL){
										*ec << "[QE] ERROR! More then one subview defining virtual objects with same name";
										*ec << (ErrQExecutor | EBadViewDef);
										return (ErrQExecutor | EBadViewDef);
									}
									else referenced_view_lid = maybe_subview_logID;
								}
							}
						}

						if (referenced_view_lid == NULL) {
							*ec << "[QE] operator <virtualize as> - this object doesn't have this operation defined, subview missing";
							*ec << (ErrQExecutor | EOperNotDefined);
							return (ErrQExecutor | EOperNotDefined);
						}
						else {
							errcode = getOn_procedure(referenced_view_lid, "on_virtualize", referenced_view_code, referenced_view_param);
							if (errcode != 0) return errcode;
							if (referenced_view_code == "") {
								*ec << "[QE] operator <virtualize as> - this VirtualObject doesn't have this operation defined";
								*ec << (ErrQExecutor | EOperNotDefined);
								return (ErrQExecutor | EOperNotDefined);
							}
							subquery_view_parent = maybe_parent;
						}
					}
					else {
						*ec << "[QE] <virtualize as> - subquery should evaluate to single virtual result or single reference";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return (ErrQExecutor | EQEUnexpectedErr);
					}
				}
				else {
					*ec << "[QE] operator <virtualize as> - subquery should evaluate to one element bag";
					*ec << (ErrQExecutor | EQEUnexpectedErr);
					return (ErrQExecutor | EQEUnexpectedErr);
				}
			}

			if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {
				*ec << "[QE] For each row of this score, the right argument will be computed";
				for (unsigned int i = 0; i < (lResult->size()); i++) {
					QueryResult *currentResult;
					if ((lResult->type()) == QueryResult::QSEQUENCE)
						errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
					else
						errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
					if (errcode != 0) return errcode;

					vector<QueryBagResult*> envs_sections;

					QueryResult *param_binder = new QueryBinderResult(referenced_view_param, currentResult);
					errcode = createNewSections((QueryVirtualResult*) subqueryResult, (QueryBinderResult*) param_binder, referenced_view_lid, envs_sections);
					if (errcode != 0) return errcode;

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
				*ec << "[QE] ERROR! Virtualize As operation, bad left argument";
				*ec << (ErrQExecutor | EOtherResExp);
				return (ErrQExecutor | EOtherResExp);
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Virtualize As operation Done!";
			return 0;
		}//case TNVIRTUALIZEAS

		//ADTODO
		case TreeNode::TNINTERFACEBIND: {
		    *ec << "[QE] Type: TNINTERFACEBIND";
		    string interfaceName = ((InterfaceBind *)tree)->getInterfaceName();
		    string implementationName = ((InterfaceBind *)tree)->getImplementationName();

		    /**********************************************
		    Check if interface and implementation are in DB
		    ***********************************************/

		    ec->printf("[QE] TNINTERFACEBIND: Checking interface presence by name: %s\n", interfaceName.c_str());
		    bool taken;
		    if ((errcode = interfaceNameTaken(interfaceName, taken)) != 0) {
		        *ec << "[QE] TNINTERFACEBIND: returning error from interfaceNameTaken";
		        return errcode;
		    }
    		    if (!taken)
		        return qeErrorOccur("[QE] TNINTERFACEBIND: interface \"" + interfaceName + "\" does not exist", ENoInterfaceFound);

		    *ec << "[QE] TNINTERFACEBIND: interface found, checking implementation presence";

		    /* ADTODO - check views also! */
		    bool impNameTaken;
		    errcode = implementationNameTaken(implementationName, impNameTaken);
		    if(errcode != 0) {
		    	*ec << "[QE] TNINTERFACEBIND: returning error from implementationNameTaken";
		    	return errcode;
		    }
		    if(!impNameTaken)
		    	return qeErrorOccur("[QE] Implementation: \"" + implementationName + "\" does not exist.", ENoImplementationFound);

		    /**********************************************
		    Check if implementation fits interface
		    **********************************************/
		    
		    bool matches;
		    errcode = Schemas::Schema::interfaceMatchesImplementation(interfaceName, implementationName, this, matches);
		    if (errcode) 
		    {
			ec->printf("[QE] INTERFACESBIND - error in interfaceMatchesImplementation\n"); 
		    }
		    string resS = matches ? "fits": "does not fit";
		    ec->printf("[QE] TNINTERFACEBIND - interface %s %s implementation %s\n",interfaceName.c_str(),resS.c_str(),implementationName.c_str());
		    
		    /****************
		    Bind interface
		    ****************/
		    
		    if (matches)
		    {
			errcode = tr->bindInterface(interfaceName, implementationName);
			//TODO - error handling
		    }		    
		    /******************
		    Cleanup and return
		    *******************/

		    QueryResult *result = new QueryIntResult(0);
		    errcode = qres->push(result);
		    if (errcode != 0) return errcode;
		    *ec << "[QE] TNINTERFACEBIND done";
		    return 0;
		}

		case TreeNode::TNREGINTERFACE: {

		    *ec << "[QE] Type: TNREGINTERFACE";

		    QueryNode *query = ((RegisterInterfaceNode *) tree)->getQuery();
		    ObjectPointer *optr;

		    if(query != NULL) {
			errcode = executeRecQuery(query);
			if(errcode != 0) return errcode;
		    }
		    else qres->push(new QueryNothingResult());

		    ec->printf("[QE] TNREGINTERFACE inner result pushed\n");

		    QueryResult *execution_result;
		    errcode = qres->pop(execution_result);
		    if (errcode != 0) return errcode;
		    if (execution_result->type() != QueryResult::QREFERENCE) {
			return qeErrorOccur( "[QE] TNREGINTERFACE error - execution result is not QueryReference", ERefExpected );
		    }

		    ec->printf("[QE] TNREGINTERFACE got reference\n");

		    errcode = tr->getObjectPointer (((QueryReferenceResult*)execution_result)->getValue(), Store::Read, optr, false);
		    if (errcode != 0) {
		    	return trErrorOccur("[QE] register interface operation - Error in getObjectPointer.", errcode);
		    }

		    ec->printf("[QE] TNREGINTERFACE got object pointer\n");

		    errcode = tr->addRoot(optr);
		    if (errcode != 0) {
		    	return trErrorOccur("[QE] Error in addRoot", errcode);
		    }

		    ec->printf("[QE] TNREGINTERFACE addRoot processed\n");

		    *ec << optr->getName().c_str();
		    ec->printf("[QE] TNREGINTERFACE got name");

		    string object_name = "";
		    vector<LogicalID*>* inner_vec = (optr->getValue())->getVector();
		    for (unsigned int i=0; i < inner_vec->size(); i++)
		    {
			ObjectPointer *inner_optr;
			errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
			if (errcode != 0)
			    return trErrorOccur("[QE] register interface operation - Error in getObjectPointer.", errcode);
			if (inner_optr->getName() == QE_OBJECT_NAME_BIND_NAME)
			{
			    object_name = (inner_optr->getValue())->getString();
			    break;
			}
		    }
		    ec->printf("[QE] TNREGINTERFACE got object name");
		    *ec << object_name;

		    errcode = tr->addInterface(optr->getName().c_str(), object_name.c_str(), optr);
		    if (errcode != 0) {
		    	return trErrorOccur("[QE] Error in addInterface", errcode);
		    }

		    errcode = qres->push(execution_result);
		    if (errcode != 0) return errcode;

		    *ec << "[QE] TNREGINTERFACE returning";

		    return 0;
		}

		case TreeNode::TNINTERFACEMETHOD: {
		    *ec << "[QE] Type: TNINTERFACEMETHOD";
		    string name = ((InterfaceMethod *) tree)->getName();
		    vector<LogicalID *> methodLids;

		    ec->printf("[QE] Type: TNINTERFACEMETHOD name=%s\n", name.c_str());

		    QueryResult *methodStruct = new QueryStructResult();
		    QueryResult *paramName, *paramNameBinder;
		    
		    // get and process arguments (ADTODO-check name uniqueness)
		    TAttributes params = ((InterfaceMethod *) tree)->getParams();
		    ec->printf("[QE] TNINTERFACEMETHOD parameter count = %d\n", params.size());
		    for(TAttributes::iterator it = params.begin(); it != params.end(); ++it) 
		    {
			paramName = new QueryStringResult(it->getName());
			paramNameBinder = new QueryBinderResult(QE_METHOD_PARAM_BIND_NAME, paramName);
			methodStruct->addResult(paramNameBinder);
			ec->printf("[QE] TNINTERFACEMETHOD parameter (%s) added\n", it->getName().c_str());
		    }

		    //ADTODO-check memory!

		    QueryResult *methodBinder = new QueryBinderResult(name, methodStruct);
		    ObjectPointer *optr;

		    ec->printf("[QE] TNINTERFACEMETHOD making object from binder\n");

		    if ((errcode = objectFromBinder(methodBinder, optr))!=0)
			return errcode;

		    ec->printf("[QE] TNINTERFACEMETHOD object made\n");

		    QueryResult *lidres = new QueryReferenceResult(optr->getLogicalID());
		    if ((errcode = qres->push(lidres))!=0)
			return errcode;

		    *ec << "[QE] TNINTERFACEMETHOD proccessing complete";
		    return 0;
		}

		case TreeNode::TNINTERFACEATTRIBUTE: {
		    string name = ((InterfaceAttribute *) tree)->getName();
		    ec->printf("[QE] Type: TNINTERFACEATTRIBUTE Name = %s\n", name.c_str());
		    		    
		    QueryResult *nameString = new QueryStringResult(name);
		    QueryResult *nameBinder = new QueryBinderResult(QE_NAME_BIND_NAME, nameString);

		    ObjectPointer *optr;
		    if ((errcode = objectFromBinder(nameBinder, optr))!=0)
			return errcode;

		    QueryResult *lidres = new QueryReferenceResult(optr->getLogicalID());
		    if ((errcode = qres->push(lidres))!=0)
			return errcode;

		    *ec << "[QE] TNINTERFACEATTRIBUTE proccessing complete";
		    return 0;
		}

		case TreeNode::TNINTERFACENODE: {
			*ec << "[QE] Type: TNINTERFACENODE";
			string interfaceName = ((InterfaceNode *) tree)->getInterfaceName();
			string objectName = ((InterfaceNode *) tree)->getObjectName();
			*ec << "[QE] Interface name: " << interfaceName;

			// check if interface name is already used in database - move it to reg
			*ec << "[QE] Checking if name is already used in database..";

			bool taken;
			if ((errcode = interfaceNameTaken(interfaceName, taken)) != 0) {
			    *ec << "[QE] TNINTERFACENODE: returning error from interfaceNameTaken";
			    return errcode;
			}
    			if (taken)
			    return qeErrorOccur("[QE] TNINTERFACENODE: interface name \"" + interfaceName + "\" already in use", ENotUniqueInterfaceName);

			*ec << "[QE] TNINTERFACENODE: Interface name does not exist in database";
			InterfaceNode *node = (InterfaceNode *) tree;
			
			// vector that holds all the logical ids
			vector<LogicalID *> interfaceLids;
			pushStringToLIDs(objectName, QE_OBJECT_NAME_BIND_NAME, interfaceLids);

			// get and process supers
			TSupers supers = node->getSupers();
			ec->printf("[QE] TNINTERFACENODE: supers count: %d\n", supers.size());
			TSupers::iterator it;
			for (it = supers.begin(); it != supers.end(); ++it)
			{
			    string super = *it;
			    pushStringToLIDs(super, QE_EXTEND_BIND_NAME, interfaceLids);
			}
			
			// get and process attributes (ADTODO-check name uniqueness)
			TAttributes attributes = node->getAttributes();
			ec->printf("[QE] TNINTERFACENODE: attributes count: %d\n", attributes.size());
			for(TAttributes::iterator it = attributes.begin(); it != attributes.end(); it++) 
			{
			    InterfaceAttribute *a = new InterfaceAttribute(*it);
			    ec->printf("[QE] TNINTERFACENODE: adding attribute\n");
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
			    ec->printf("[QE] TNINTERFACENODE: attribute added, lid = %d\n", newLid);
			    interfaceLids.push_back(newLid);
			}

			// get and process methods (ADTODO-check name uniqueness)
			TMethods methods = node->getMethods();
			
			for(TMethods::iterator it = methods.begin(); it != methods.end(); it++) 
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
			DBDataValue *dbValue = new DBDataValue();
			dbValue->setVector(&interfaceLids);
			LogicalID* newLid;
			if ((errcode = createObjectAndPutOnQRes(dbValue, interfaceName, Store::Interface, newLid))!=0)
			    return errcode;

			*ec << "[QE] TNINTERFACENODE processing complete";
			return 0;
		}

		case TreeNode::TNEXCLUDES:
		case TreeNode::TNINCLUDES: {
			if(nodeType == TreeNode::TNINCLUDES) {
				*ec << "[QE] Type: TNINCLUDES";
			} else {
				*ec << "[QE] Type: TNEXCLUDES";
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
			*ec << "[QE] Type: TNINSTANCEOF";
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
			*ec << "[QE] TNINSTANCEOF processing complete";
			return 0;
		}

		case TreeNode::TNCAST: {
			*ec << "[QE] Type: TNCAST";
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
			*ec << "[QE] TNCAST processing complete";
			return 0;
		}

		case TreeNode::TNCLASS: {
			*ec << "[QE] Type: TNCLASS";
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
			errcode = classExists(className, classExist);
			if(errcode != 0) {
				return errcode;
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
				errcode = classesLIDsFromNames(classesNames, extendedclassesLIDs);
				if(errcode != 0) {
					return errcode;
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
			DBDataValue *dbValue = new DBDataValue();
			dbValue->setVector(&classVector);
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
				errcode = removePersistFromSuperclasses(upOptr);
				if(errcode != 0) return errcode;

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
			*ec << "[QE] Type: TNREGCLASS";
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
			} else {
				errcode = cg->addClass(optr, tr, this);
				if(errcode != 0) return errcode;
			}

			*ec << cg->toString();

			errcode = qres->push(execution_result);
			if (errcode != 0) return errcode;
			return 0;
		}

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

				if (needsDeepCardCheck) {
					int cum;
					errcode = this->checkSubCardsRec(tree->getCoerceSig(), binder, true, cum);
					if (errcode != 0) return errcode;
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

				vector<LogicalID*>* vec_virt;
				if ((errcode = tr->getViewsLID(newobject_name, vec_virt)) != 0) {
					*ec << "[QE] bindName - error in getViewsLID";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				int vecSize_virt = vec_virt->size();
				ec->printf("[QE] %d Views LID by name taken\n", vecSize_virt);
				if (vecSize_virt > 1) {
					*ec << "[QE] Multiple views defining virtual objects with the same name";
					*ec << (ErrQExecutor | EBadBindName);
					return (ErrQExecutor | EBadBindName);
				}
				else if (vecSize_virt == 1) {
					LogicalID *view_def = vec_virt->at(0);
					string proc_code = "";
					string proc_param = "";
					errcode = getOn_procedure(view_def, "on_create", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] operator <create> - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}
					vector<QueryBagResult*> envs_sections;

					QueryResult *create_arg = ((QueryBinderResult*)binder)->getItem();
					QueryResult *param_binder = new QueryBinderResult(proc_param, create_arg);

					errcode = createNewSections(NULL, (QueryBinderResult*) param_binder, view_def, envs_sections);
					if (errcode != 0) return errcode;

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

						QueryResult *virt_res = new QueryVirtualResult(newobject_name, view_defs, seeds, NULL);
						((QueryBagResult *) result)->addResult(virt_res);
					}
				}
				else {
					ObjectPointer *optr;
					errcode = this->objectFromBinder(binder, optr);
					if (errcode != 0) return errcode;

					string object_name = optr->getName();
					if (!system_privilige_checking &&
						assert_privilige(Privilige::CREATE_PRIV, object_name) == false) {
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
				//delete binder;
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
			errcode = this->algOperate(op, lResult, rResult, op_result, (AlgOpNode *) tree);
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
			QueryResult *res;
			string paramName = ((ParamNode *) tree)->getName();
			if (prms == NULL) {
				*ec << "[QE] error! Parametr operation - params == NULL";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			int howMany = prms->count(paramName);
			if (howMany != 1) {
				*ec << "[QE] error! Parametr operation - wrong parametr";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			map<string, QueryResult*>::iterator pos;
			pos = prms->find(paramName);
			if (pos != prms->end()) res = pos->second;
			else {
				*ec << "[QE] error! Parametr operation - index out of range";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			QueryResult *final_res;
			errcode = reVirtualize(res, final_res);
			if (errcode != 0) return errcode;
			errcode = qres->push(final_res);
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

						if ((lResult->type()) == QueryResult::QSEQUENCE)
							errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
						else
							errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
						if (errcode != 0) return errcode;

						QueryResult *rResult;

						ec->printf("[QE] nested operation started()\n");
						errcode = (currentResult)->nested_and_push_on_envs(this, tr);
						if (errcode != 0) return errcode;

						*ec << "[QE] Computing right Argument with a new scope of ES";
						errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(rResult);
						if (errcode != 0) return errcode;

						errcode = envs->pop();
						if (errcode != 0) return errcode;

						errcode = this->combine(op,currentResult,rResult,partial_result);
						if (errcode != 0) return errcode;
						*ec << "[QE] Combined partial results";
					}
				}
				else {
					*ec << "[QE] ERROR! NonAlgebraic operation, bad left argument";
					*ec << (ErrQExecutor | EOtherResExp);
					return (ErrQExecutor | EOtherResExp);
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
							*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
							*ec << (ErrQExecutor | EBoolExpected);
							return (ErrQExecutor | EBoolExpected);
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
					return (ErrQExecutor | EUnknownNode);
				}
			}
			*ec << "[QE] Condition operation Done!";
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

				QueryResult *newStackSection;
				vector<DataValue*> _tmp_dataVal_vec;
				errcode = (qrr)->nested(const_cast<QueryExecutor*>(this), tr, newStackSection, _tmp_dataVal_vec);
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
		case TreeNode::TNCOERCE : { // May appear only through TC coerce augments
			CoerceNode *cn = (CoerceNode *) tree;
			int cType = cn->getCType();
			*ec << "[QE] COERCE Node to be processed... ";
			errcode = executeRecQuery(cn->getArg());
			if (errcode != 0) return errcode;
			QueryResult *tmp_result;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;

			QueryResult *op_result;
			errcode = this->coerceOperate(cType, tmp_result, op_result, tree);
			if (errcode != 0) return errcode;

			errcode = qres->push(op_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] COERCE Node Done!";
			return 0;
		}
		case TreeNode::TNCASTTO : {// May appear when variant result appears (eg. through union)...
			//QE ignores this node - it assumes typechecker checked it correct.
			*ec << "[QE] CAST TO Node ...";
			errcode = executeRecQuery(((SimpleCastNode *)tree)->getArg());
			if (errcode != 0) return errcode;
			*ec << "[QE] CAST TO Node Done!";
			return 0;
			//do not touch the qres stack - let superior node read what arg left there.
			//if the loopCHECK was introduced - it would just throw 'CastNode' error on error...
		}
		default:
			{
			*ec << "Unknown node type";
			*ec << (ErrQExecutor | EUnknownNode);
			return (ErrQExecutor | EUnknownNode);
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
			errcode = tr->getObjectPointer(ref_value, Store::Read, optr, true);
			if (errcode != 0) {
				*ec << "[QE] Error in getObjectPointer";
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
						errcode = tr->getObjectPointer(currID, Store::Read, currOptr, false);
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
						//MH set direct parent id...
						currID->setDirectParent(ref_value);
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
			if (((QueryVirtualResult *) arg)->refed) {
				res = arg;
				break;
			}
			LogicalID *view_def = ((QueryVirtualResult*) arg)->view_defs.at(0);
			vector<QueryResult *> seeds = ((QueryVirtualResult*) arg)->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = getOn_procedure(view_def, "on_retrieve", proc_code, proc_param);
			if (errcode != 0) return errcode;
			if (proc_code == "") {
				*ec << "[QE] derefQuery() - this VirtualObject doesn't have this operation defined";
				*ec << (ErrQExecutor | EOperNotDefined);
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			errcode = createNewSections((QueryVirtualResult*) arg, NULL, NULL, envs_sections);
			if (errcode != 0) return errcode;

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;

			errcode = qres->pop(res);
			if (errcode != 0) return errcode;
			break;
		}
		default: {
			*ec << "[QE] ERROR in derefQuery() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
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
		case QueryResult::QVIRTUAL: {
			res = arg;
			((QueryVirtualResult *) res)->refed = true;
			break;
		}
		default: {
			*ec << "[QE] ERROR in refQuery() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return (ErrQExecutor | EOtherResExp);
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
                *ec << "nameof() dereferencing remote object";
                RemoteExecutor *rex = new RemoteExecutor(lid->getServer(), lid->getPort(), this);
                rex->setLogicalID(lid);
                rex->setDeref();
                QueryResult* qr;
                errcode = rex->execute(&qr);
                if (errcode != 0) {
                    return errcode;
                }
                *ec << "nameof() on remote object done";
                res = qr;
                break;
            }

            if (((QueryReferenceResult *) arg)->wasRefed()) {
                res = arg;
                break;
            }
            LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
            *ec << "[QE] nameof() - dereferencing Object";
            ObjectPointer *optr;
            errcode = tr->getObjectPointer(ref_value, Store::Read, optr, true);
            if (errcode != 0) {
                *ec << "[QE] Error in getObjectPointer";
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
        case QueryResult::QVIRTUAL: {
		string vo_name = ((QueryVirtualResult*) arg)->vo_name;
		res = new QueryStringResult(vo_name);
		break;
	}
        default: {
            *ec << "[QE] ERROR in derefQuery() - unknown result type";
            *ec << (ErrQExecutor | EOtherResExp);
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
				return (ErrQExecutor | ENumberExpected);
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
				return (ErrQExecutor | EBoolExpected);
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
			*ec << "[QE] DELETE operation";
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			errcode = persistDelete(bagArg);
			if(errcode != 0) return errcode;
			*ec << "[QE] delete operation complete - QueryNothingResult created";
			final = new QueryNothingResult();
			break;
		}
		default: { // unOperation type not known
			*ec << "[QE] ERROR! Unary operation type not known";
			*ec << (ErrQExecutor | EUnknownNode);
			return (ErrQExecutor | EUnknownNode);
		}
	}
	return 0;
}

int QueryExecutor::removePersistFromSubclasses(ObjectPointer *superOptr) {
	SetOfLids* objectToChange = superOptr->getValue()->getSubclasses();
	if(objectToChange == NULL) return 0;
	for(SetOfLids::iterator i = objectToChange->begin(); i != objectToChange->end(); ++i) {
		ObjectPointer* optr;
		int errcode = tr->getObjectPointer( *i, Store::Read, optr, false);
		if(errcode != 0) {
			return trErrorOccur("[QE] Can't get object to remove classMark.", errcode);
		}
		DataValue* newDV = optr->getValue()->clone();
		newDV->removeClassMark(superOptr->getLogicalID());
		errcode = tr->modifyObject(optr, newDV);
		if(errcode != 0) {
			return trErrorOccur("[QE] Can't modify object and set classMark.", errcode);
		}
	}
	return 0;
}

int QueryExecutor::removePersistFromSuperclasses(ObjectPointer *subOptr) {
	SetOfLids* objectToChange = subOptr->getValue()->getClassMarks();
	if(objectToChange == NULL) return 0;
	for(SetOfLids::iterator i = objectToChange->begin(); i != objectToChange->end(); ++i) {
		ObjectPointer* optr;
		int errcode = tr->getObjectPointer( *i, Store::Read, optr, false);
		if(errcode != 0) {
			return trErrorOccur("[QE] Can't get object to remove classMark.", errcode);
		}
		DataValue* newDV = optr->getValue()->clone();
		newDV->removeSubclass(subOptr->getLogicalID());
		errcode = tr->modifyObject(optr, newDV);
		if(errcode != 0) {
			return trErrorOccur("[QE] Can't modify object and set classMark.", errcode);
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
		*ec << "[QE] Error in getObjectPointer.";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}

	if (optr == NULL) {
		return 0;
	}

	string object_name = optr->getName();
	//Class does not have to be root.
	if (((optr->getValue())->getSubtype()) == Store::Class) {
		//Class removing means removing of all its static members too.
		errcode = persistStaticMembersDelete(object_name);
		if (errcode != 0) return errcode;
		//Removing class from superclasses and subclasses and updating class graph.
		errcode = removePersistFromSuperclasses(optr);
		if (errcode != 0) return errcode;
		errcode = removePersistFromSubclasses(optr);
		if (errcode != 0) return errcode;
		cg->removeClass(optr->getLogicalID());
		//Removing class from class file.
		errcode = tr->removeClass(optr);
		if (errcode != 0) {
			return trErrorOccur("[QE] Error in class removing.", errcode);
		}
	}

	if (optr->getIsRoot()) {
		if (!system_privilige_checking &&
			assert_privilige(Privilige::DELETE_PRIV, object_name) == false) {
			/*
			ofstream out("privilige.debug", ios::app);
			out << "delete " << object_name << endl;
			out.close();
			*/
			return 0;
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
	if (!system_privilige_checking &&
		assert_privilige(Privilige::DELETE_PRIV, object_name) == false) {
		/*
		ofstream out("privilige.debug", ios::app);
		out << "delete " << object_name << endl;
		out.close();
		*/
		return 0;
	}
	if ((errcode = tr->deleteObject(optr)) != 0) {
		*ec << "[QE] Error in deleteObject.";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}
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
			errcode = getOn_procedure(view_def, "on_delete", proc_code, proc_param);
			if (errcode != 0) return errcode;

			if (proc_code == "") {
				*ec << "[QE] delete() - this VirtualObject doesn't have this operation defined";
				*ec << (ErrQExecutor | EOperNotDefined);
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			errcode = createNewSections((QueryVirtualResult*) toDelete, NULL, NULL, envs_sections);
			if (errcode != 0) return errcode;

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;

			QueryResult *res;
			errcode = qres->pop(res);
			if (errcode != 0) return errcode;
		}
		else {
			*ec << "[QE] ERROR! DELETE argument must consist of QREFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return (ErrQExecutor | ERefExpected);
		}
		*ec << "[QE] Object deleted";
	}
	return 0;
}

// May appear only through TC coerce augments
int QueryExecutor::coerceOperate(int cType, QueryResult *arg, QueryResult *&final, TreeNode *tree) {
	int errcode = 0;

	switch (cType) {
		case CoerceNode::to_string : {
			*ec << "	Coercing to string";
			string final_value = "";
			stringstream ss;
			switch(arg->type()) {
				case QueryResult::QINT: {
					ss << (((QueryIntResult *)arg)->getValue());
					ss >> final_value;
					break;
				}
				case QueryResult::QDOUBLE: {
					ss << (((QueryDoubleResult *)arg)->getValue());
					ss >> final_value;
					break;
				}
				case QueryResult::QSTRING: {
					final_value = ((QueryStringResult *)arg)->getValue();
					break;
				}
				case QueryResult::QBOOL: {
					final_value = (((QueryBoolResult *)arg)->getValue() ? "true" : "false");
					break;
				}
				default: final = arg; return 0; // ignore the coerce
			}
			final = new QueryStringResult(final_value);
			return 0;
		}
		case CoerceNode::to_double : { // from int/double/string/bool
			*ec << "	Coercing to double";
			double final_value = 0.0;
			stringstream ss;
			switch (arg->type()) {
				case QueryResult::QINT: {
					final_value = double(((QueryIntResult *)arg)->getValue());
					break;
				}
				case QueryResult::QDOUBLE: {
					final_value = ((QueryDoubleResult *)arg)->getValue();
					break;
				}
				case QueryResult::QSTRING: {
					stringstream ss(((QueryStringResult *)arg)->getValue());
					string rest = "";
					bool wellDone = (ss >> final_value);
					ss >> rest;
					if (not wellDone || not rest.empty()) return (ErrQExecutor | ECrcToDouble);
					break;
				}
				case QueryResult::QBOOL: {
					final_value = (((QueryBoolResult *)arg)->getValue() ? 1.0 : 0.0);
					break;
				}
				default: final = arg; return 0; // ignore the coerce
			}
			final = new QueryDoubleResult(final_value);
			return 0;
		}
		case CoerceNode::to_int : { // from int/double/string/bool (!) rounds doubles down to their int part
			*ec << "	Coercing to integer";
			int final_value = 0;
			stringstream ss;
			switch (arg->type()) {
				case QueryResult::QINT: {
					final_value = ((QueryIntResult *)arg)->getValue();
					break;
				}
				case QueryResult::QDOUBLE: {
					final_value = int(((QueryDoubleResult *)arg)->getValue());
					break;
				}
				case QueryResult::QSTRING: {
					stringstream ss(((QueryStringResult *)arg)->getValue());
					string rest = "";
					bool wellDone = (ss >> final_value);
					ss >> rest;
					if (not wellDone || (!rest.empty() && rest.at(0) != '.')) return (ErrQExecutor | ECrcToInt);
					break;
				}
				case QueryResult::QBOOL : {
					final_value = (((QueryBoolResult *)arg)->getValue() ? 1 : 0);
					break;
				}
				default: final = arg; return 0; // ignore the coerce
			}
			final = new QueryDoubleResult(final_value);
			return 0;
		}
		case CoerceNode::to_bool : { // from int / double / string
			*ec << "	Coercing to bool";
			string fval = "";
			stringstream ss;
			switch(arg->type()) {
				case QueryResult::QINT: {
					ss << (((QueryIntResult *)arg)->getValue());
					ss >> fval;
					break;
				}
				case QueryResult::QDOUBLE: {
					ss << (((QueryDoubleResult *)arg)->getValue());
					ss >> fval;
					break;
				}
				case QueryResult::QSTRING: {
					fval = ((QueryStringResult *)arg)->getValue();
					break;
				}
				default: final = arg; return 0; // ignore the coerce /  for QBOOL: no need to cast
			}
			if (fval == "true" || fval == "TRUE" || fval == "1" || fval == "yes" || fval == "YES")
				final = new QueryBoolResult(true);
			else if(fval == "false" || fval == "FALSE" || fval == "0" || fval == "no" || fval == "NO")
				final = new QueryBoolResult(false);
			else return (ErrQExecutor | ECrcToBool);
			return 0;
		}
		case CoerceNode::element : {
			*ec << "	Coercing - element()";
			//Arg is already a single element...
			if (arg->type() != QueryResult::QBAG && arg->type() != QueryResult::QSEQUENCE) {
				final = arg;
				return 0;
			}
			if (arg->type() == QueryResult::QBAG) {
				QueryBagResult *bagArg = ((QueryBagResult *) arg);
				//If collection is empty or has more than 1 elt - return error.
				if (bagArg->isEmpty())	return (ErrQExecutor | ECrcEltEmptySet);
				if (bagArg->size() > 1) return (ErrQExecutor | ECrcEltMultiple);
				QueryResult *elem;
				errcode = bagArg->at(0, elem);
				if (errcode != 0) return errcode;
				final = elem;
				return 0;
			}
			if (arg->type() == QueryResult::QSEQUENCE) {
				QuerySequenceResult *seqArg = ((QuerySequenceResult *) arg);
				//If collection is empty or has more than 1 elt - return error.
				if (seqArg->isEmpty())	return (ErrQExecutor | ECrcEltEmptySet);
				if (seqArg->size() > 1) return (ErrQExecutor | ECrcEltMultiple);
				QueryResult *elem;
				errcode = seqArg->at(0, elem);
				if (errcode != 0) return errcode;
				final = elem;
				return 0;
			}
			break;
		}
		case CoerceNode::to_bag : {
			*ec << "	Coercing to bag";
			// AddResult() takes care of everything!
			final = new QueryBagResult();
			final->addResult(arg);
			return 0;
			// If arg is already a bag - nothing to be done.
			//if (arg->type() == QueryResult::QBAG) { final = arg; return 0;}
			// If arg is a seq - iterate through its elts, adding to a new bag.
			//if (arg->type() == QueryResult::QSEQUENCE) {}
		}
		case CoerceNode::to_seq : {
			*ec << "	Coercing to sequence";
			final = new QuerySequenceResult();
			final->addResult(arg);
			return 0;
		}
		case CoerceNode::can_del : {
			*ec << "	Coerce checking if delete allowed, with card 1..*";
			return checkDelCard(arg, final);
		}
		case CoerceNode::ext_crt : {
			*ec << "	Coerce checking if cardinality of created object allows more creates.";
			QueryResult *bagRes = new QueryBagResult();
			bagRes->addResult(arg);
			map<string, int> rootCdMap;
			//Assumption: this dynamic coerce only made when card of this root is != '?..*'. So
			//it means there can only be 1 such obj. If one day more cards are available (eg. 0..3 ?)
			//this check below would have to be enhanced, size() would need be checked against upper bound.
			int maxAllowed = 1;
			for (unsigned int i = 0; i < bagRes->size(); i++) {
				QueryResult *binder;
				errcode = ((QueryBagResult *) bagRes)->at(i, binder);
				if (errcode != 0) return errcode;
				if ((binder->type()) != QueryResult::QBINDER) {
					return qeErrorOccur("[QE] [TC] ext_create check() expected a binder, got something else", EOtherResExp);
				}
				string crtName = ((QueryBinderResult*)binder)->getName();

				errcode = checkUpInRootCardMap(crtName, rootCdMap);
				if (errcode != 0) return errcode;
				if ((++rootCdMap[crtName]) > maxAllowed) {
					*ec << "[QE] would create too many '" + crtName + "' roots. Returning coerce error.";
					return (ErrQExecutor | ECrcCrtExtTooMany);
				}
			}
			break;
		}
		default: *ec << "	Coerce type not recognized"; break;
	}
	final = arg;
	return 0;
}

int QueryExecutor::checkDelCard(QueryResult *arg, QueryResult *&final) {
	int errcode = 0;
	QueryBagResult* bagArg = new QueryBagResult();
	bagArg->addResult(arg);
	//Map below holds: for each parent-log-id, for a name of its subobject: how many are there. (so if -
	// after delete - the nr was to be zero - throw error - can't delete non-optional object.
	map<std::pair<unsigned int, string>, int> delSubMap;
	map<string, int> delRootMap;
	int objectsLeft = 0;
	for (unsigned int i = 0; i < bagArg->size(); i++) {
		QueryResult* toDelete;  //the object to be deleted
		errcode = bagArg->at(i, toDelete);
		if (errcode != 0) return errcode;
		if (toDelete->type() != QueryResult::QREFERENCE) return (ErrQExecutor | ERefExpected);
		LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
		ObjectPointer *optr;
		errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
		if (errcode != 0) {
			*ec << "[QE] check delete card - Error in getObjectPointer.";
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
		string optrName = optr->getName();
		if (optr->getIsRoot()) {
			errcode = checkUpInRootCardMap(optrName, delRootMap);
			if (errcode != 0) return errcode;
			if ((objectsLeft = --delRootMap[optrName]) == 0) {
				*ec << "[QE] would leave no more roots with this name, but their card is >= 1.";
				return (ErrQExecutor | ECrcDelNonOptional);
			}
		} else {// complex ! - reach for the parent and check local card...
			if (lid->getDirectParent() == NULL) {
				*ec << "[QE] check delete card - Cannot delete child element, because parent not set.";
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			LogicalID *ptLid = lid->getDirectParent();
			errcode = checkUpInDelSubMap(ptLid, optrName, delSubMap);
			if (errcode != 0) return errcode;
			std::pair<unsigned int, string> key(ptLid->toInteger(), optrName);
			if ((objectsLeft = --delSubMap[key]) == 0) {
				*ec << "[QE] would delete all subobjects with this name, but their card is >= 1.";
				return (ErrQExecutor | ECrcDelNonOptional);
			}
		}
	}
	// May proceed with delete...
	final = arg;
	return 0;
}

int QueryExecutor::checkUpInDelSubMap(LogicalID *ptLid, string name, map<std::pair<unsigned int, string>, int> &delMap) {
	int errcode = 0;
	std::pair<unsigned int, string> key(ptLid->toInteger(),name);
	cout << "[in QE]::checkUpInDelSubMap !!\n";
	if (delMap.find(key) == delMap.end()) {
		cout << "		[QE]::checkUpInDelSubMap - no entry for key: (" << key.first << ", " << key.second <<")! \n";
		ObjectPointer *optr;
		errcode = tr->getObjectPointer (ptLid, Store::Read, optr, false);
		if (errcode != 0) {
			*ec << "[QE] check delete non-root - Error in getObjectPointer.";
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
		int vType = optr->getValue()->getType();
		if (vType != Store::Vector) {
			*ec << "[QE] check delete non-root  - ERROR! the l-Value has to be a reference to Vector";
			return (ErrQExecutor | EOtherResExp);
		}
		vector<LogicalID*> *vec = optr->getValue()->getVector();
		int sameNameObjectsNum = 0;
		for (unsigned int i = 0; i < vec->size(); i++) {
			errcode = tr->getObjectPointer(vec->at(i), Store::Read, optr, false);
			if (errcode != 0) {
				*ec << "[QE] check delete non-root - Error in getObjectPointer for one of the siblings.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			if (optr->getName() == name) sameNameObjectsNum++;
		}
		delMap[key] = sameNameObjectsNum;
	}
	return 0;
}

int QueryExecutor::checkUpInRootCardMap(string optrName, map<string, int> &delRootMap) {
	int errcode = 0;
	if (delRootMap.find(optrName) == delRootMap.end()) {
		//add root with its number to map.
		vector<LogicalID*>* vec;
		if ((errcode = tr->getRootsLID(optrName, vec)) != 0) {
			*ec << "[QE] check up in root Card map card - error in getRootsLID";
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}
		delRootMap[optrName] = vec->size();
		delete vec;
	}
	return 0;
}

int QueryExecutor::checkSubCardsRec(Signature *sig, ObjectPointer *optr) {
	cout << "[QE][TC] checkSubCardsRec(optr) START\n";
	if (optr == NULL) cout <<"optr is null!\n";
	if (sig == NULL) cout <<"sig is null!\n";
	if (sig == NULL || optr == NULL) return (ErrQExecutor | EOtherResExp);
	cout << "[QE][TC] checkSubCardsRec(optr) not nulls...\n";
	int errcode = 0;
	int result = 0;
	int vType = optr->getValue()->getType();
	if (vType != Store::Vector) return 0;
	*ec << "[QE][TC] checkSubCardsRec(optr) - ObjectValue = Vector";
	vector<LogicalID*>* tmp_vec = optr->getValue()->getVector();
	int tmp_vec_size = tmp_vec->size();

	SigColl *sigc = (sig->type() == Signature::SBINDER ? (SigColl *) ((StatBinder *)sig)->getValue() : (SigColl *) sig);
	map<string, int> subMap;
	for (int i = 0; i < tmp_vec_size; i++) {
		cout << "[QE][TC] checkSubCardsRec(optr) AT son: " << i << "\n";
		LogicalID *currID = tmp_vec->at(i);
		ObjectPointer *currOptr;
		errcode = tr->getObjectPointer(currID, Store::Read, currOptr, false);
		if (errcode != 0) {
			*ec << "[QE] Error in getObjectPointer, in checkSubCardsRec(optr)";
			antyStarveFunction(errcode);
			inTransaction = false;
			return errcode;
		}

		string currName = currOptr->getName();
		Signature *mptr = sigc->getMyList();
		bool foundName = false;
		Signature *match = NULL;
		while (mptr != NULL && not foundName) {
			if (((StatBinder *) mptr)->getName() == currName) {
				foundName = true;
				match = mptr;
			}
			mptr = mptr->getNext();
		}
		cout << "[QE][TC] checkSubCardsRec(optr) looked through left sigs children to find: '" <<currName<<"'..\n";
		if (not foundName || match == NULL) return (ErrQExecutor | EOtherResExp);
		//recursively check each elt
		result = checkSubCardsRec(match, currOptr);
		if (result != 0) return result;
		//register subobject in map. (adjusting the nr of same objects registered)
		if (subMap.find(currName) == subMap.end()) {
			subMap[currName] = 0;
		}
		subMap[currName]++;

	}
	Signature *ptr = sigc->getMyList();
	while (ptr != NULL) {
		string name = ((StatBinder *)ptr)->getName();
		string card = ptr->getCard();
		if (subMap.find(name) == subMap.end()) {
			subMap[name] = 0;
		}
		//check if its missing: not found in subMap  -> error
		if (card[0] != '0' && subMap[name] == 0) return (ErrQExecutor | ESigMissedSubs);

		//check for overflow: if rightBound of card '<' subMap[name].first -> error
		if (card[3]!= '*' && (card[3]-'0') < subMap[name]) return (ErrQExecutor | ESigCdOverflow);

		ptr = ptr->getNext();
	}
	return 0;
}

int QueryExecutor::checkSubCardsRec(Signature *sig, QueryResult *res, bool isBinder, int &obtsSize) {
	if (sig == NULL || res == NULL) return (ErrQExecutor | EOtherResExp);
	if (sig->type() != Signature::SBINDER && sig->type() != Signature::SSTRUCT) { obtsSize = 1; return 0; }
	int result = 0;
	if (sig->type() == Signature::SBINDER) {
		QueryResult *toCompare = (isBinder ? ((QueryBinderResult *)res)->getItem() : res);
		Signature *sigVal = ((StatBinder *)sig)->getValue();
		int cmpType = toCompare->type();

		//in case of multiple objects ...
		if (cmpType == QueryResult::QBAG || cmpType == QueryResult::QSEQUENCE) {
			string card = sig->getCard();
			int compSize = toCompare->size();
			obtsSize = 0;
			for (int pos = 0; pos < compSize; pos++) {
				QueryResult *single;
				int singleSize = 0;
				if (cmpType == QueryResult::QBAG) {
					result = ((QueryBagResult *)toCompare)->at(pos, single);
				} else {
					result = ((QuerySequenceResult *)toCompare)->at(pos, single);
				}
				if (result == 0) {
					result = checkSubCardsRec(sigVal, single, true, singleSize);
				}
				if (result != 0) return result;
				obtsSize += singleSize;
			}
			if ((card[0] != '0' && obtsSize == 0) || (card[3] != '*' && obtsSize > 1))
				return (ErrQExecutor | EBadInternalCd);
			return 0;
		}
		//'toCompare' is some single object...
		obtsSize = 1;
		return checkSubCardsRec(sigVal, toCompare, true, obtsSize);
	}
	//sig is a STRUCT signature, res - a QueryStructResult
	SigColl *sigc = (SigColl *) sig;
	QueryStructResult *obj = (QueryStructResult *)res;
	obtsSize = 1;
	map<string, int> subMap;
	//for each elt of guest obj - check it doesn't violate card constraints of some subobject of sig.
	for (unsigned int i = 0; i < (obj->size()); i++) {
		QueryResult *tmp_res = NULL;
		result = obj->at(i, tmp_res);
		if (result != 0) return result;
		//in case of multiple objects ...
		if (tmp_res->type() == QueryResult::QBAG || tmp_res->type() == QueryResult::QSEQUENCE) {
			for (unsigned int pos = 0; pos < tmp_res->size(); pos++) {
				QueryResult *single;
				if (tmp_res->type() == QueryResult::QBAG) {
					result = ((QueryBagResult *)tmp_res)->at(pos, single);
				} else {
					result = ((QuerySequenceResult *)tmp_res)->at(pos, single);
				}
				if (result != 0) return result;
				result = checkSingleSubCard(sigc, single, subMap);
				if (result != 0) return result;
			}
		} else {
			result = checkSingleSubCard(sigc, tmp_res, subMap);
			if (result != 0) return result;
		}
	}
	//now -knowing the subMap - check for: subOverflows, subsMissing.
	Signature *ptr = sigc->getMyList();
	while (ptr != NULL) {
		string name = ((StatBinder *)ptr)->getName();
		string card = ptr->getCard();
		if (subMap.find(name) == subMap.end()) {
			subMap[name] = 0;
		}
		//check if its missing: not found in subMap  -> error
		if (card[0] != '0' && subMap[name] == 0) return (ErrQExecutor | ESigMissedSubs);

		//check for overflow: if rightBound of card '<' subMap[name].first -> error
		if (card[3]!= '*' && (card[3]-'0') < subMap[name]) return (ErrQExecutor | ESigCdOverflow);

		ptr = ptr->getNext();
	}
	return 0;
}

int QueryExecutor::checkSingleSubCard(SigColl *sigc, QueryResult *tmp_res, map<string, int> &subMap) {
	int result = 0;
	if (tmp_res == NULL || (tmp_res->type()) != QueryResult::QBINDER) return (ErrQExecutor | EOtherResExp);
	string binderName = ((QueryBinderResult *) tmp_res)->getName();
		//QueryResult *binderItem = ((QueryBinderResult *) tmp_res)->getItem();
	Signature *mptr = sigc->getMyList();
	bool foundName = false;
	Signature *match = NULL;
	while (mptr != NULL && not foundName) {
		if (((StatBinder *) mptr)->getName() == binderName) {
			foundName = true;
			match = mptr;
		}
		mptr = mptr->getNext();
	}
	if (not foundName || match == NULL) return (ErrQExecutor | EOtherResExp);
		//recursively check each elt
	int eltSize = 0;
	result = checkSubCardsRec(match, tmp_res, true, eltSize);
	if (result != 0) return result;
		//register subobject in map. (adjusting the nr of same objects registered)
	if (subMap.find(binderName) == subMap.end()) {
		subMap[binderName] = 0;
	}
	subMap[binderName] += eltSize;
	cout << "[QE][TC] ----- subMap[" << binderName << "] := " << subMap[binderName] << "...\n";
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
			}
			string final_value = leftString + rightString;
			final = new QueryStringResult(final_value);
		}
		else if ((derefL->type() == QueryResult::QINT) || (derefL->type() == QueryResult::QDOUBLE)) {
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
					return (ErrQExecutor | ENumberExpected);
				}
				case AlgOpNode::minus: {
					*ec << "[QE] ERROR! - arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return (ErrQExecutor | ENumberExpected);
				}
				case AlgOpNode::times: {
					*ec << "[QE] ERROR! * arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return (ErrQExecutor | ENumberExpected);
				}
				case AlgOpNode::divide: {
					*ec << "[QE] ERROR! / arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
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
					return (ErrQExecutor | EBoolExpected);
				}
				case AlgOpNode::boolAnd: {
					*ec << "[QE] ERROR! AND arguments must be BOOLEAN";
					*ec << (ErrQExecutor | EBoolExpected);
					return (ErrQExecutor | EBoolExpected);
				}
				default : { break; }
			}
		}
		return 0;
	}
	else if (op == AlgOpNode::bagUnion) {
		*ec << "[QE] BAG_UNION operation";
		final = new QueryBagResult();
		((QueryBagResult *) final)->addResult(lArg);
		((QueryBagResult *) final)->addResult(rArg);
		return 0;
	}
	else if ((op == AlgOpNode::bagIntersect) || (op == AlgOpNode::bagMinus)) {
		if (op == (AlgOpNode::bagIntersect)) *ec << "[QE] BAG_INTERSECT operation";
		if (op == (AlgOpNode::bagMinus)) *ec << "[QE] BAG_MINUS operation";

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
	else if ((op == AlgOpNode::insert) || (op == AlgOpNode::insert_viewproc)) {
		*ec << "[QE] INSERT operation";
		if (tn->isCoerced()) {
			*ec << "[QE][TC] INSERT operation is COERCED !";
			for (int i = 0; i < tn->nrOfCoerceActions(); i++) {
				int actId = tn->getCoerceAction(i);
				cout << actId << "- ";
				switch (actId) {
					case TypeCheck::DTable::CD_CAN_ASGN : cout << "insert: ASSIGN CHECK\n";
					case TypeCheck::DTable::CD_CAN_INS : cout << "insert: INSERT CHECK\n";
					case TypeCheck::DTable::CD_EXT_INS : cout << "insert: INSERT SHALLOW CD CHECK\n";
				}
			}
		}
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			*ec << "[QE] ERROR! Left argument of insert operation must have size 1";
			*ec << (ErrQExecutor | ERefExpected);
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
				ec->printf("[QE] trying to INSERT %d element of rightArg into leftArg\n", i);
				QueryResult *toInsert;
				errcode = ((QueryBagResult *) rBag)->at(i, toInsert);
				if (errcode != 0) return errcode;

				while (toInsert->type() == QueryResult::QVIRTUAL) {
					LogicalID *view_def = ((QueryVirtualResult*) toInsert)->view_defs.at(0);
					vector<QueryResult *> seeds = ((QueryVirtualResult*) toInsert)->seeds;
					string proc_code = "";
					string proc_param = "";
					errcode = getOn_procedure(view_def, "on_store", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] INSERT on_store - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					errcode = createNewSections((QueryVirtualResult*) toInsert, NULL, NULL, envs_sections);
					if (errcode != 0) return errcode;

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *on_store_res;
					errcode = qres->pop(on_store_res);
					if (errcode != 0) return errcode;

					while (on_store_res->type() == QueryResult::QBAG) {
						if (on_store_res->size() != 1) {
							*ec << "[QE] insert operation: on_store procedure result is not single";
								*ec << (ErrQExecutor | EOtherResExp);
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
					errcode = getOn_procedure(main_view_def, "on_insert", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] operator <insert> - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					QueryResult *param_binder = new QueryBinderResult(proc_param, toInsert);
					errcode = createNewSections((QueryVirtualResult*) outer, (QueryBinderResult*) param_binder, NULL, envs_sections);
					if (errcode != 0) return errcode;

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *res;
					errcode = qres->pop(res);
					if (errcode != 0) return errcode;
				}
				else if (toInsert->type() == QueryResult::QBINDER) {
					vector<LogicalID *> subviews;
					errcode = getSubviews(main_view_def, main_vo_name, subviews);
					if (errcode != 0) return errcode;

					string toInsert_name = ((QueryBinderResult *)toInsert)->getName();
					QueryResult *toInsert_value = ((QueryBinderResult *)toInsert)->getItem();

					LogicalID *sub_view_def = NULL;
					for (unsigned int j = 0; j < subviews.size(); j++ ) {
						LogicalID *subview_lid = subviews.at(j);
						string subview_name = "";
						string subview_code = "";
						errcode = checkViewAndGetVirtuals(subview_lid, subview_name, subview_code);
						if (errcode != 0) return errcode;
						if (subview_name == toInsert_name) {
							if (sub_view_def != NULL){
								*ec << "[QE] ERROR! More then one subview defining virtual objects with same name";
								*ec << (ErrQExecutor | EBadViewDef);
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
						*ec << "[QE] operator <insert into> - this virtualObject doesn't have this operation defined, subview missing";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}
					string proc_code = "";
					string proc_param = "";
					errcode = getOn_procedure(sub_view_def, "on_create", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] operator <insert into> - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					QueryResult *param_binder = new QueryBinderResult(proc_param, toInsert_value);
					errcode = createNewSections((QueryVirtualResult*) outer, (QueryBinderResult*) param_binder, sub_view_def, envs_sections);
					if (errcode != 0) return errcode;

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *res;
					errcode = qres->pop(res);
					if (errcode != 0) return errcode;
				}
				else {
					*ec << "[QE] Right argument of insert operation must consist of QREFERENCE or QBINDER";
					*ec << (ErrQExecutor | ERefExpected);
					return (ErrQExecutor | ERefExpected);
				}
			}
		}
		else if (leftResultType == QueryResult::QREFERENCE) {
			LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
			ObjectPointer *optrOut;
			errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut, false);
			if (errcode != 0) {
				*ec << "[QE] insert operation - Error in getObjectPointer.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string outer_name = optrOut->getName();
			if (!system_privilige_checking &&
				assert_privilige(Privilige::MODIFY_PRIV, outer_name) == false) {
				final = new QueryNothingResult();
				return 0;
			}
			*ec << "[QE] insert operation - getObjectPointer on leftArg done";
			DataValue* db_value = optrOut->getValue();
			ExtendedType optrOut_extType = db_value->getSubtype();
			*ec << "[QE] insert operation - Value taken";
			int vType = db_value->getType();
			if (vType != Store::Vector) {
				*ec << "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector";
				*ec << (ErrQExecutor | EOtherResExp);
				return (ErrQExecutor | EOtherResExp);
			}
			vector<LogicalID*> *insVector = db_value->getVector();
			if (insVector == NULL) {
				*ec << "[QE] insert operation - insVector == NULL";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return (ErrQExecutor | EQEUnexpectedErr);
			}
			*ec << "[QE] Vector taken";
			ec->printf("[QE] Vec.size = %d\n", insVector->size());

			//----------	TC dynamic coerce input 	---------- //
			bool needsExtCoerce = tn->needsCoerce(TypeCheck::DTable::CD_EXT_INS);
			bool needsDeepCardCheck = tn->needsCoerce(TypeCheck::DTable::CD_CAN_INS);
			map<string, int> subCardMap;
			if (needsExtCoerce) {
				*ec << "[QE][TC] Fill maps that help check card constraints are met on inserted objects.";
				for (unsigned int j = 0;  j < insVector->size(); j++) {
					ObjectPointer *subPtr;
					errcode = tr->getObjectPointer (insVector->at(j), Store::Read, subPtr, true);
					if (errcode != 0) {
						*ec << "[QE] insert operation - ext coerce - Error in getObjectPointer.";
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
				ec->printf("[QE] trying to INSERT %d element of rightArg into leftArg\n", i);
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
					errcode = getOn_procedure(view_def, "on_store", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] INSERT on_store - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					errcode = createNewSections((QueryVirtualResult*) toInsert, NULL, NULL, envs_sections);
					if (errcode != 0) return errcode;

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *on_store_res;
					errcode = qres->pop(on_store_res);
					if (errcode != 0) return errcode;

					while (on_store_res->type() == QueryResult::QBAG) {
						if (on_store_res->size() != 1) {
							*ec << "[QE] insert operation: on_store procedure result is not single";
								*ec << (ErrQExecutor | EOtherResExp);
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
							*ec << "[QE] insert operation - Error in getObjectPointer.";
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						object_name = checkIn->getName();
						//Check external card constraints on inserted object
						if (needsExtCoerce) {
							*ec << "[QE][TC] insert:ref:: checking that no '"+object_name+"' here yet.";
							if (subCardMap.find(object_name) == subCardMap.end())
								subCardMap[object_name] = 0;
							int subNum = (++subCardMap[object_name]);
							if (subNum > 1) {
								*ec << "[QE][TC] insert:ref:: coerce ERROR - card of inserted object exceeded.";
								*ec << (ErrQExecutor | ECrcInsExtTooMany);
								return (ErrQExecutor | ECrcInsExtTooMany);
							}
						}
						//Check internal cards constraints (for subobjects)
						if (needsDeepCardCheck) {
							string txt = (optrIn == NULL ? " NULL!" : "NOT null...");
							*ec << "optr is " + txt;
							txt = (tn->getCoerceSig() == NULL ? " NULL!" : "NOT null...");
							*ec << "coerceSig is " + txt;
							errcode = this->checkSubCardsRec(tn->getCoerceSig(), checkIn);
							if (errcode != 0) return errcode;
						}

						errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn, true);
						if (errcode != 0) {
							*ec << "[QE] insert operation - Error in getObjectPointer.";
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
						if (optrIn == NULL) {
							*ec << "[QE] ERROR in insert operation - objectPointer is NULL";
							*ec << (ErrQExecutor | EQEUnexpectedErr);
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
						/*if (!system_privilige_checking &&
								assert_privilige(Privilige::MODIFY_PRIV, object_name) == false) {
							final = new QueryNothingResult();
							//ofstream out("privilige.debug", ios::app);
							//out << "create " << object_name << endl;
							//out.close();
							return 0;
						}*/
						ec->printf("[QE] getObjectPointer on %d element of rightArg done\n", i);
						
						if ((((optrIn->getValue())->getType()) == Store::Vector) && (((optrIn->getValue())->getSubtype()) == Store::View)) {
							string virtual_objects_name = "";
							vector<LogicalID*>* view_inner_vec = (optrIn->getValue())->getVector();
							
							for (unsigned int k=0; k < view_inner_vec->size(); k++) {
								ObjectPointer *view_inner_optr;
								errcode = tr->getObjectPointer (view_inner_vec->at(k), Store::Read, view_inner_optr, false);
								if (errcode != 0) {
									*ec << "[QE] insert view operation - Error in getObjectPointer.";
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
								*ec << "[QE] insert operation - Error in getObjectPointer.";
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							db_value = optrOut->getValue();
							optrOut_extType = db_value->getSubtype();
							vType = db_value->getType();
							if (vType != Store::Vector) {
								*ec << "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector";
								*ec << (ErrQExecutor | EOtherResExp);
								return (ErrQExecutor | EOtherResExp);
							}
							insVector = db_value->getVector();
							if (insVector == NULL) {
								*ec << "[QE] insert operation - insVector == NULL";
								*ec << (ErrQExecutor | EQEUnexpectedErr);
								return (ErrQExecutor | EQEUnexpectedErr);
							}
							
							vector<LogicalID*> subobjects_to_virtualize;
							for (unsigned int k=0; k < insVector->size(); k++) {
								ObjectPointer *to_virtualize_optr;
								errcode = tr->getObjectPointer (insVector->at(k), Store::Read, to_virtualize_optr, false);
								if (errcode != 0) {
									*ec << "[QE] insert view operation - Error in getObjectPointer.";
									antyStarveFunction(errcode);
									inTransaction = false;
									return errcode;
								}
								if (to_virtualize_optr->getName() == virtual_objects_name) {
									subobjects_to_virtualize.push_back(insVector->at(k));;
								}
							}
							
							if (subobjects_to_virtualize.size() > 0) {
								*ec << "[QE] WARNING, subobjects with the same name as new virtual objects found. These objects will be transformed into virtuals.";
								string proc_code = "";
								string proc_param = "";
								errcode = getOn_procedure(lidIn, "on_create", proc_code, proc_param);
								if (errcode != 0) {
									return errcode;
								}
								if (proc_code == "") {
									*ec << "[QE] insert view <create operation> - this View doesn't have this operation defined";
									
									errcode = persistDelete(lidIn);
									if(errcode != 0) return errcode;
									
									*ec << (ErrQExecutor | EOperNotDefined);
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
									
									errcode = createNewSections(NULL, (QueryBinderResult*) param_binder, lidIn, envs_sections);
									if (errcode != 0) return errcode;
									
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
									*ec << "[QE] insert operation - Error in removeView.";
									antyStarveFunction(errcode);
									inTransaction = false;
									return errcode;
								}
							}

							errcode = tr->removeRoot(optrIn);
							if (errcode != 0) {
								*ec << "[QE] insert operation - Error in removeRoot.";
								antyStarveFunction(errcode);
								inTransaction = false;
								return errcode;
							}
							*ec << "[QE] insert operation - Root removed";
						}
						else if (op != AlgOpNode::insert_viewproc) {
							*ec << "[QE] ERROR in insert operation - right argument must be Root or a new object (or procedure, view)";
							*ec << (ErrQExecutor | EQEUnexpectedErr);
							return (ErrQExecutor | EQEUnexpectedErr);
						}
						toInsVector.push_back(lidIn);
						break;
					}
					case QueryResult::QBINDER: {
						object_name = ((QueryBinderResult *)toInsert)->getName();
						LogicalID *maybe_sub_view_def = NULL;

						if (needsExtCoerce) {
							*ec << "[QE][TC] insert:binder:: checking that no '"+object_name+"' here yet.";
							if (subCardMap.find(object_name) == subCardMap.end())
								subCardMap[object_name] = 0;
							int subNum = (++subCardMap[object_name]);
							if (subNum > 1) {
								*ec << "[QE][TC] insert:binder:: coerce ERROR- card of inserted object exceeded.";
								*ec << (ErrQExecutor | ECrcInsExtTooMany);
								return (ErrQExecutor | ECrcInsExtTooMany);
							}
						}
						if (needsDeepCardCheck) {
							int cum;
							errcode = this->checkSubCardsRec(tn->getCoerceSig(), toInsert, true, cum);
							if (errcode != 0) return errcode;
						}

						//TODO powinna byc mozliwosc zapisania w objectPointerze ze zawiera jako podobiekty perspektywy
						//ObjectPointer powinien miec funkcje nazwa->LID perspektywy definiujacej obiekty o takiej nazwie
						//cos na ksztalt getViews(name), wtedy ponizszy kawalek kodu moznaby uspawnic znacznie
						int insVector_size = insVector->size();
						for (int j = 0; j < insVector_size; j++ ) {
							LogicalID *maybe_subview_logID = insVector->at(j);
							ObjectPointer *maybe_subview_optr;
							if ((errcode = tr->getObjectPointer(maybe_subview_logID, Store::Read, maybe_subview_optr, false)) != 0) {
								*ec << "[QE] INSERT operator - Error in getObjectPointer";
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
								errcode = checkViewAndGetVirtuals(maybe_subview_logID, maybe_subview_name, maybe_subview_code);
								if (errcode != 0) return errcode;
								if (maybe_subview_name == object_name) {
									if (maybe_sub_view_def != NULL){
										*ec << "[QE] ERROR! More then one subview defining virtual objects with same name";
										*ec << (ErrQExecutor | EBadViewDef);
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
							errcode = getOn_procedure(maybe_sub_view_def, "on_create", proc_code, proc_param);
							if (errcode != 0) return errcode;
							if (proc_code == "") {
								*ec << "[QE] operator <insert into> - this VirtualObject doesn't have this operation defined";
								*ec << (ErrQExecutor | EOperNotDefined);
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
							errcode = createNewSections(NULL, (QueryBinderResult*) param_binder, maybe_sub_view_def, envs_sections);
							if (errcode != 0) return errcode;

							errcode = callProcedure(proc_code, envs_sections);
							if(errcode != 0) return errcode;

							QueryResult *maybe_sub_view_res;
							errcode = qres->pop(maybe_sub_view_res);
							if (errcode != 0) return errcode;
						}
						break;
					}
					default: {
						*ec << "[QE] Right argument of insert operation must consist of QREFERENCE or QBINDER or QVIRTUAL";
						*ec << (ErrQExecutor | ERefExpected);
						return (ErrQExecutor | ERefExpected);
					}
				}
			}

			errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut, false);
			if (errcode != 0) {
				*ec << "[QE] insert operation - Error in getObjectPointer.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			db_value = optrOut->getValue();
			optrOut_extType = db_value->getSubtype();
			vType = db_value->getType();
			if (vType != Store::Vector) {
				*ec << "[QE] insert operation - ERROR! the l-Value has to be a reference to Vector";
				*ec << (ErrQExecutor | EOtherResExp);
				return (ErrQExecutor | EOtherResExp);
			}
			insVector = db_value->getVector();
			if (insVector == NULL) {
				*ec << "[QE] insert operation - insVector == NULL";
				*ec << (ErrQExecutor | EQEUnexpectedErr);
				return (ErrQExecutor | EQEUnexpectedErr);
			}

			for (unsigned int i = 0; i < toInsVector.size(); i++) {
				*ec << "[QE] Inserting the object";
				insVector->push_back(toInsVector.at(i));
				ec->printf("[QE] New Vec.size = %d\n", insVector->size());
			}

			DBDataValue *dbDataVal = new DBDataValue();
			dbDataVal->setVector(insVector);
			dbDataVal->setClassMarks(db_value->getClassMarks());
			dbDataVal->setSubclasses(db_value->getSubclasses());
			*ec << "[QE] dataValue: setVector done";
			db_value = dbDataVal;
			errcode = tr->modifyObject(optrOut, db_value);
			if (errcode != 0) {
				*ec << "[QE] insert operation - Error in modifyObject.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			(optrOut->getValue())->setSubtype(optrOut_extType);
			errcode = tr->modifyObject(optrOut, optrOut->getValue());
			if (errcode != 0) {
				*ec << "[QE] insert operation - Error in modifyObject.";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			*ec << "[QE] insert operation: value of the object was changed";
		}
		else {
			*ec << "[QE] ERROR! Left argument of insert operation must consist of QREFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return (ErrQExecutor | ERefExpected);
		}
		*ec << "[QE] INSERT operation Done";
		final = new QueryNothingResult();
		return 0;
	}
	else if ((op == AlgOpNode::assign) || (op == AlgOpNode::assign_viewproc)) {
		*ec << "[QE] ASSIGN operation";
		if (tn->isCoerced()) {
			*ec << "[QE][TC] ASSIGN operation is COERCED !";
			for (int i = 0; i < tn->nrOfCoerceActions(); i++) {
				int actId = tn->getCoerceAction(i);
				cout << actId << "- ";
				switch (actId) {
					case TypeCheck::DTable::CD_CAN_ASGN : cout << "ASSIGN CHECK\n";
					case TypeCheck::DTable::CD_CAN_INS : cout << "INSERT CHECK\n";
					case TypeCheck::DTable::CD_EXT_INS : cout << "INSERT SHALLOW CD CHECK\n";
				}
			}
		}
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			*ec << "[QE] ERROR! operator <assign> expects that left argument is single REFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
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
			errcode = getOn_procedure(view_def, "on_update", proc_code, proc_param);
			if (errcode != 0) return errcode;
			if (proc_code == "") {
				*ec << "[QE] operator <assign> - this VirtualObject doesn't have this operation defined";
				*ec << (ErrQExecutor | EOperNotDefined);
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			QueryResult *param_binder = new QueryBinderResult(proc_param, rArg);
			errcode = createNewSections((QueryVirtualResult*) lArg, (QueryBinderResult*) param_binder, NULL, envs_sections);
			if (errcode != 0) return errcode;

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
			if ((old_optr->getIsRoot()) && ((old_optr->getValue())->getSubtype() == Store::View)) {
				errcode = tr->removeView(old_optr);
				if (errcode != 0) {
					*ec << "[QE] Error in removeView.";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
			}
			DBDataValue *dbValue = new DBDataValue();
			if (op == AlgOpNode::assign) {
				QueryResult *derefed;
				errcode = this->derefQuery(rArg, derefed);
				if (errcode != 0) return errcode;
				while (derefed->type() == QueryResult::QVIRTUAL) {
					LogicalID *view_def = ((QueryVirtualResult*) derefed)->view_defs.at(0);
					vector<QueryResult *> seeds = ((QueryVirtualResult*) derefed)->seeds;
					string proc_code = "";
					string proc_param = "";
					errcode = getOn_procedure(view_def, "on_store", proc_code, proc_param);
					if (errcode != 0) return errcode;
					if (proc_code == "") {
						*ec << "[QE] UPDATE on_store - this VirtualObject doesn't have this operation defined";
						*ec << (ErrQExecutor | EOperNotDefined);
						return (ErrQExecutor | EOperNotDefined);
					}

					vector<QueryBagResult*> envs_sections;

					errcode = createNewSections((QueryVirtualResult*) derefed, NULL, NULL, envs_sections);
					if (errcode != 0) return errcode;

					errcode = callProcedure(proc_code, envs_sections);
					if(errcode != 0) return errcode;

					QueryResult *on_store_res;
					errcode = qres->pop(on_store_res);
					if (errcode != 0) return errcode;

					while (on_store_res->type() == QueryResult::QBAG) {
						if (on_store_res->size() != 1) {
							*ec << "[QE] update operation: on_store procedure result is not single";
								*ec << (ErrQExecutor | EOtherResExp);
								return (ErrQExecutor | EOtherResExp);
							}
						QueryResult *curr;
						errcode = ((QueryBagResult *) on_store_res)->at(0, curr);
						on_store_res = curr;
					}
					derefed = on_store_res;
					*ec << "[QE] < query := ('virtual'); > operation";
				}
				int derefed_type = derefed->type();
				//TC dynamic coerce... might need only if derefed_type is struct or binder...
				bool needsDeepCardCheck = tn->needsCoerce(TypeCheck::DTable::CD_CAN_ASGN);
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
						if (needsDeepCardCheck) {
							int cum;
							errcode = this->checkSubCardsRec(tn->getCoerceSig(), derefed, false, cum);
							if (errcode != 0) return errcode;
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
						*ec << "[QE] < query := ('struct'); > operation";
						break;
					}
					case QueryResult::QBINDER: {
						if (needsDeepCardCheck) {
							int cum;
							errcode = this->checkSubCardsRec(tn->getCoerceSig(), derefed, true, cum);
							if (errcode != 0) return errcode;
						}
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
					*ec << "[QE] operator <assign>: error in getObjectPointer()";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				rArg_extType = (rArg_optr->getValue())->getSubtype();

				if (op == AlgOpNode::assign_viewproc) {
					string rArg_name = rArg_optr->getName();
					if (object_name != rArg_name) {
						*ec << "[QE] operator <assign>: ERROR left and right arguments have different names";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return (ErrQExecutor | EQEUnexpectedErr);
					}

					vector<LogicalID*>* inner_vec = (rArg_optr->getValue())->getVector();
					if ((old_optr->getIsRoot()) && (rArg_extType == Store::View)) {
						string virtual_objects_name = "";
						for (unsigned int i=0; i < inner_vec->size(); i++) {
							ObjectPointer *inner_optr;
							errcode = tr->getObjectPointer (inner_vec->at(i), Store::Read, inner_optr, false);
							if (errcode != 0) {
								*ec << "[QE] register view operation - Error in getObjectPointer.";
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
							*ec << "[QE] Error in addView";
							antyStarveFunction(errcode);
							inTransaction = false;
							return errcode;
						}
					}

					dbValue->setVector(inner_vec);
					errcode = tr->deleteObject(rArg_optr);
					if (errcode != 0) {
						*ec << "[QE] Error in deleteObject.";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
				}
				else if ((rArg_extType == Store::Procedure) || (rArg_extType == Store::View)) {
					rArg_extType = Store::None;
					string rArg_name = rArg_optr->getName();
					if (object_name != rArg_name) *ec << "[QE] operator <assign>: warning left and right arguments have different names";
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
			(old_optr->getValue())->setSubtype(rArg_extType);
			errcode = tr->modifyObject(old_optr, old_optr->getValue());
			if (errcode != 0) {
				*ec << "[QE] operator <assign>: error in modifyObject()";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			*ec << "[QE] operator <assign>: value of the object was changed";
		}
		else {
			*ec << "[QE] ERROR! operator <assign> expects that left argument is REFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return (ErrQExecutor | ERefExpected);
		}
		*ec << "[QE] ASSIGN operation Done";
		final = new QueryNothingResult();
		return 0;
	}
	else { // algOperation type not known
		*ec << "[QE] ERROR! Algebraic operation type not known";
		*ec << (ErrQExecutor | EUnknownNode);
		return (ErrQExecutor | EUnknownNode);
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
				return (ErrQExecutor | EBoolExpected);
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
				return (ErrQExecutor | EBoolExpected);
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
				return (ErrQExecutor | EBoolExpected);
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
			return (ErrQExecutor | EUnknownNode);
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
			((QueryBagResult *) partial)->sortBag_in_orderBy();
			errcode = ((QueryBagResult *) partial)->postSort(final);
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
					return (ErrQExecutor | EBoolExpected);
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
					return (ErrQExecutor | EBoolExpected);
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
			return (ErrQExecutor | EUnknownNode);
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
		return (ErrQExecutor | EOtherResExp);
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
	if (errcode == (ErrTManager | EDeadlock)) {
		antyStarve = true;
		*ec << "[QE] EDeadlock error found => AntyStarve mode is on";
	}
}

int QueryExecutor::objectFromBinder(QueryResult *res, ObjectPointer *&newObject) {
	int errcode;
	*ec << "[QE] objectFromBinder()";
	ec->printf("[QE] QueryResult type: %d\n", res->type());
	if ((res->type()) != QueryResult::QBINDER) {
		ec->printf("[QE] objectFromBinder() expected a binder, got something else: %d\n", res->type());
		*ec << (ErrQExecutor | EOtherResExp);
		return (ErrQExecutor | EOtherResExp);
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
				errcode = ((QueryBagResult *) binderItem)->at(i, curr);
				if (errcode != 0) return errcode;
				if (curr->type() != QueryResult::QREFERENCE) {
					*ec << "[QE] objectFromBinder() QueryReference expected";
					*ec << (ErrQExecutor | ERefExpected);
					return (ErrQExecutor | ERefExpected);
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
		case QueryResult::QVIRTUAL: {

			LogicalID *view_def = ((QueryVirtualResult*) binderItem)->view_defs.at(0);
			vector<QueryResult *> seeds = ((QueryVirtualResult*) binderItem)->seeds;
			string proc_code = "";
			string proc_param = "";
			errcode = getOn_procedure(view_def, "on_store", proc_code, proc_param);
			if (errcode != 0) return errcode;
			if (proc_code == "") {
				*ec << "[QE] CREATE on_store - this VirtualObject doesn't have this operation defined";
				*ec << (ErrQExecutor | EOperNotDefined);
				return (ErrQExecutor | EOperNotDefined);
			}

			vector<QueryBagResult*> envs_sections;

			errcode = createNewSections((QueryVirtualResult*) binderItem, NULL, NULL, envs_sections);
			if (errcode != 0) return errcode;

			errcode = callProcedure(proc_code, envs_sections);
			if(errcode != 0) return errcode;

			QueryResult *on_store_res;
			errcode = qres->pop(on_store_res);
			if (errcode != 0) return errcode;

			while (on_store_res->type() == QueryResult::QBAG) {
				if (on_store_res->size() != 1) {
					*ec << "[QE] objectFromBinder(): on_store procedure result is not single";
					*ec << (ErrQExecutor | EOtherResExp);
					return (ErrQExecutor | EOtherResExp);
				}
				QueryResult *curr;
				errcode = ((QueryBagResult *) on_store_res)->at(0, curr);
				on_store_res = curr;
			}

			QueryResult *next_binder = new QueryBinderResult(binderName, on_store_res);
			errcode = this->objectFromBinder(next_binder, newObject);
			if (errcode != 0) return errcode;
			*ec << "[QE] objectFromBinder(): value is VIRTUAL type";
			return 0;
		}
		default: {
			*ec << "[QE] objectFromBinder(): bad type item in a binder";
			*ec << (ErrQExecutor | EOtherResExp);
			return (ErrQExecutor | EOtherResExp);
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

int QueryExecutor::createNewSections(QueryVirtualResult *qvirt, QueryBinderResult *param, LogicalID *viewdef, vector<QueryBagResult*> &sections) {

	int errcode;
	vector<DataValue*> _tmp_dataVal_vec;

	if ((qvirt != NULL) && (qvirt->view_parent != NULL)) {
		QueryResult *tmp_result_1 = new QueryReferenceResult(qvirt->view_parent);
		QueryResult *nested_tmp_result_1;
		errcode = tmp_result_1->nested(this, tr, nested_tmp_result_1, _tmp_dataVal_vec);
		if(errcode != 0) return errcode;
		sections.push_back((QueryBagResult *) nested_tmp_result_1);
	}

	if (qvirt != NULL) {
		for (int i = ((qvirt->view_defs.size()) - 1); i >= 0; i-- ) {
			QueryResult *tmp_result_2 = new QueryReferenceResult(qvirt->view_defs.at(i));
			QueryResult *nested_tmp_result_2;
			errcode = tmp_result_2->nested(this, tr, nested_tmp_result_2, _tmp_dataVal_vec);
			if(errcode != 0) return errcode;
			sections.push_back((QueryBagResult *) nested_tmp_result_2);
		}
	}

	if (viewdef != NULL) {
		QueryResult *tmp_result_3 = new QueryReferenceResult(viewdef);
		QueryResult *nested_tmp_result_3;
		errcode = tmp_result_3->nested(this, tr, nested_tmp_result_3, _tmp_dataVal_vec);
		if(errcode != 0) return errcode;
		sections.push_back((QueryBagResult *) nested_tmp_result_3);
	}

	if (qvirt != NULL) {
		for (int i = ((qvirt->seeds.size()) - 1); i >= 0; i-- ) {
			QueryResult *tmp_result_4 = qvirt->seeds.at(i);
			QueryResult *nested_tmp_result_4;
			errcode = tmp_result_4->nested(this, tr, nested_tmp_result_4, _tmp_dataVal_vec);
			if(errcode != 0) return errcode;
			sections.push_back((QueryBagResult *) nested_tmp_result_4);
		}
	}

	if (param != NULL) {
		QueryBagResult *param_bag = new QueryBagResult();
		param_bag->addResult(param);
		sections.push_back(param_bag);
	}

	return 0;
}

int QueryExecutor::procCheck(unsigned int qSize, LogicalID *lid, string &code, vector<string> &prms, int &founded) {
	*ec << "[QE] checking if this LogicalID point procedure we look for";
	int errcode;
	ObjectPointer *optr;

	errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
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
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
				*ec << "[QE] procCheck - Error in getObjectPointer";
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
			*ec << "[QE] procCheck - success, this is procedure we've looked for";
			if (founded != 0) {
				*ec << "[QE] error there should be only one procedure in a scope with the same name and number of parameters";
				*ec << (ErrQExecutor | EProcNotSingle);
				return (ErrQExecutor | EProcNotSingle);
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
	errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
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
		for (int i = 0; i < vec_size; i++ ) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
				*ec << "[QE] checkViewAndGetVirtuals - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			ExtendedType tmp_extType = tmp_data_value->getSubtype();
			if ((tmp_name == QE_VIEWDEF_VIRTUALOBJECTS_NAME) && (tmp_vType == Store::String) && (tmp_extType == Store::None)) {
				actual_name = tmp_data_value->getString();
				actual_name_count++;
			}
		}
		if (actual_name_count == 0) {
			*ec << "[QE] checkViewAndGetVirtuals error: \"VirtualObjects\" not found in this view ";
			*ec << (ErrQExecutor | EBadViewDef);
			return (ErrQExecutor | EBadViewDef);
		}
		else if (actual_name_count > 1) {
			*ec << "[QE] checkViewAndGetVirtuals error: multiple \"VirtualObjects\" found in this view";
			*ec << (ErrQExecutor | EBadViewDef);
			return (ErrQExecutor | EBadViewDef);
		}
		else if ((name != "") && (name != actual_name)) {
			*ec << "[QE] checkViewAndGetVirtuals error: \"VirtualObjects\" have wrong value";
			*ec << (ErrQExecutor | EBadViewDef);
			return (ErrQExecutor | EBadViewDef);
		}
		else name = actual_name;
		int proc_code_founded = 0;
		string proc_code = "";
		for (int i = 0; i < vec_size; i++) {
			LogicalID *tmp_logID = tmp_vec->at(i);
			ObjectPointer *tmp_optr;
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
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
						*ec << "[QE] checkViewAndGetVirtuals error: \"virtual objects\" procedure wrong format";
						*ec << (ErrQExecutor | EBadViewDef);
						return (ErrQExecutor | EBadViewDef);
					}
					LogicalID *proc_code_lid = tmp_vec_inner->at(0);
					ObjectPointer *proc_code_optr;
					if ((errcode = tr->getObjectPointer(proc_code_lid, Store::Read, proc_code_optr, false)) != 0) {
						*ec << "[QE] checkViewAndGetVirtuals - Error in getObjectPointer";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					string proc_code_name = proc_code_optr->getName();
					DataValue* proc_code_data_value = proc_code_optr->getValue();
					int proc_code_vType = proc_code_data_value->getType();
					ExtendedType proc_code_extType = proc_code_data_value->getSubtype();
					if ((proc_code_vType == Store::String) && (proc_code_extType == Store::None) && (proc_code_name == QE_METHOD_PROCBODY_BIND_NAME)) {
						proc_code = proc_code_data_value->getString();
					}
					else {
						*ec << "[QE] checkViewAndGetVirtuals error: \"virtual objects\" procedure wrong format";
						*ec << (ErrQExecutor | EBadViewDef);
						return (ErrQExecutor | EBadViewDef);
					}
					proc_code_founded++;
				}
			}
		}
		if (proc_code_founded == 0) {
			*ec << "[QE] checkViewAndGetVirtuals error: \"virtual objects\" procedure not found";
			*ec << (ErrQExecutor | EBadViewDef);
			return (ErrQExecutor | EBadViewDef);
		}
		else if (proc_code_founded > 1) {
			*ec << "[QE] checkViewAndGetVirtuals error: multiple \"virtual objects\" procedures found";
			*ec << (ErrQExecutor | EBadViewDef);
			return (ErrQExecutor | EBadViewDef);
		}
		else code = proc_code;
	}
	else {
		*ec << "[QE] checkViewAndGetVirtuals error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return (ErrQExecutor | EBadViewDef);
	}
	return 0;
}

int QueryExecutor::getSubviews(LogicalID *lid, string vo_name, vector<LogicalID *> &subviews) {
	int errcode;
	ObjectPointer *optr;
	errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
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
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
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
		}
	}
	else {
		*ec << "[QE] getSubviews error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return (ErrQExecutor | EBadViewDef);
	}
	return 0;
}

int QueryExecutor::getSubview(LogicalID *lid, string name, LogicalID *&subview_lid) {
	int errcode;
	ObjectPointer *optr;
	errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
	if (errcode != 0) {
		*ec << "[QE] getSubview - Error in getObjectPointer.";
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
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
				*ec << "[QE] getSubview - Error in getObjectPointer";
				antyStarveFunction(errcode);
				inTransaction = false;
				return errcode;
			}
			string tmp_name = tmp_optr->getName();
			DataValue* tmp_data_value = tmp_optr->getValue();
			int tmp_vType = tmp_data_value->getType();
			ExtendedType tmp_extType = tmp_data_value->getSubtype();
			if ((tmp_vType == Store::Vector) && (tmp_extType == Store::View)) {
				vector<LogicalID*>* tmp_vec2 = (tmp_data_value->getVector());
				int vec_size2 = tmp_vec2->size();
				for (int j = 0; j < vec_size2; j++) {
					LogicalID *tmp_logID2 = tmp_vec2->at(j);
					ObjectPointer *tmp_optr2;
					if ((errcode = tr->getObjectPointer(tmp_logID2, Store::Read, tmp_optr2, false)) != 0) {
						*ec << "[QE] getSubview - Error in getObjectPointer";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					string tmp_name2 = tmp_optr2->getName();
					if (tmp_name2 == QE_VIEWDEF_VIRTUALOBJECTS_NAME) {
						DataValue* tmp_data_value2 = tmp_optr2->getValue();
						int tmp_vType2 = tmp_data_value2->getType();
						if (tmp_vType2 == Store::String) {
							string curr_subwiew_name = tmp_data_value2->getString();
							if (curr_subwiew_name == name) {
								subview_lid = tmp_logID;
								return 0;
							}
						}
					}
				}
			}
		}
	}
	else {
		*ec << "[QE] getSubview error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return (ErrQExecutor | EBadViewDef);
	}
	return 0;
}

int QueryExecutor::getOn_procedure(LogicalID *lid, string procName, string &code, string &param) {
	code = "";
	param = "";
	int errcode;
	ObjectPointer *optr;
	errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
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
			if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
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
					return (ErrQExecutor | EBadViewDef);
				}
				LogicalID *first_logID = proc_vec->at(0);
				ObjectPointer *first_optr;
				if ((errcode = tr->getObjectPointer(first_logID, Store::Read, first_optr, false)) != 0) {
					*ec << "[QE] getOn_procedure - Error in getObjectPointer";
					antyStarveFunction(errcode);
					inTransaction = false;
					return errcode;
				}
				string first_name = first_optr->getName();
				DataValue* first_data_value = first_optr->getValue();
				int first_vType = first_data_value->getType();
				ExtendedType first_extType = first_data_value->getSubtype();
				if ((first_name == QE_METHOD_PROCBODY_BIND_NAME) && (first_vType == Store::String) && (first_extType == Store::None)) {
					code = first_data_value->getString();
				}
				else if ((first_name == QE_METHOD_PARAM_BIND_NAME) && (first_vType == Store::String) && (first_extType == Store::None)) {
					param = first_data_value->getString();
				}
				else {
					ec->printf("[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
					*ec << (ErrQExecutor | EBadViewDef);
					return (ErrQExecutor | EBadViewDef);
				}

				if (proc_vec_size == 2) {
					LogicalID *second_logID = proc_vec->at(1);
					ObjectPointer *second_optr;
					if ((errcode = tr->getObjectPointer(second_logID, Store::Read, second_optr, false)) != 0) {
						*ec << "[QE] getOn_procedure - Error in getObjectPointer";
						antyStarveFunction(errcode);
						inTransaction = false;
						return errcode;
					}
					string second_name = second_optr->getName();
					DataValue* second_data_value = second_optr->getValue();
					int second_vType = second_data_value->getType();
					ExtendedType second_extType = second_data_value->getSubtype();
					if ((second_name == QE_METHOD_PROCBODY_BIND_NAME) && (second_vType == Store::String) && (second_extType == Store::None) && code == "") {
						code = second_data_value->getString();
					}
					else if ((second_name == QE_METHOD_PARAM_BIND_NAME) && (second_vType == Store::String) && (second_extType == Store::None) && param == "") {
						param = second_data_value->getString();
					}
					else {
						ec->printf("[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
						*ec << (ErrQExecutor | EBadViewDef);
						return (ErrQExecutor | EBadViewDef);
					}
				}

			}
		}
		if (founded > 1) {
			ec->printf("[QE] getOn_procedure error: Multiple %s procedures found\n", procName.c_str());
			*ec << (ErrQExecutor | EBadViewDef);
			return (ErrQExecutor | EBadViewDef);
		}
	}
	else {
		*ec << "[QE] getOn_procedure error: This is not a view";
		*ec << (ErrQExecutor | EBadViewDef);
		return (ErrQExecutor | EBadViewDef);
	}
	return 0;
}

int QueryExecutor::callProcedure(string code, vector<QueryBagResult*> sections) {

	*ec << "[QE] TNCALLPROC";
	*ec << code;

	int errcode;
	QueryParser* tmpQP = new QueryParser(session->get_id());
	TreeNode* tmpTN;
	errcode = tmpQP->parseIt(session->get_id(), code, tmpTN);
	if (errcode != 0) {
		*ec << "[QE] TNCALLPROC error while parsing procedure code";
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

		for (unsigned int i = 0; i < sections.size(); i++) {
			errcode = envs->pop();
			if (errcode != 0) return errcode;
		}
		envs->es_priors[globalSectionNumber] = oldGlobalSectionPrior;
		envs->actual_prior--;

		delete tmpTN;
	}

	if (tmpQP != NULL) delete tmpQP;
	return 0;
}

int QueryExecutor::deVirtualize(QueryResult *arg, QueryResult *&res) {
	//*ec << "[QE] deVirtualize()";
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
			LogicalID *tmp_lid;
			string virtualToString = arg->toString(0, true);
			if (fakeLid_map.find(virtualToString) != fakeLid_map.end()) {
				tmp_lid = fakeLid_map[virtualToString];
			}
			else {
				unsigned int current_virtual_index = sent_virtuals.size();
				if (current_virtual_index >= QE_VIRTUALS_TO_SEND_MAX_COUNT) {
					*ec << "[QE] No more space left to remember next virtual object.";
					*ec << (ErrQExecutor | EQEUnexpectedErr);
					return (ErrQExecutor | EQEUnexpectedErr);
				}
				sent_virtuals.push_back(arg);
				unsigned int new_id = QE_VIRTUALS_TO_SEND_MIN_ID + current_virtual_index;
				tmp_lid = new DBLogicalID(new_id);
				fakeLid_map[virtualToString] = tmp_lid;
			}
			res = new QueryReferenceResult(tmp_lid);
			break;
		}
		default: {
			*ec << "[QE] ERROR in deVirtualize() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return (ErrQExecutor | EOtherResExp);
		}
	}
	return 0;
}

int QueryExecutor::reVirtualize(QueryResult *arg, QueryResult *&res) {
	//*ec << "[QE] reVirtualize()";
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
				errcode = this->reVirtualize(tmp_item, tmp_res);
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
				errcode = this->reVirtualize(tmp_item, tmp_res);
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
				errcode = this->reVirtualize(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->reVirtualize(((QueryBinderResult *) arg)->getItem(), tmp_item);
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
			unsigned int lid_number = (((QueryReferenceResult *) arg)->getValue())->toInteger();
			if ((lid_number >= QE_VIRTUALS_TO_SEND_MIN_ID) &&
			(lid_number < (QE_VIRTUALS_TO_SEND_MIN_ID + QE_VIRTUALS_TO_SEND_MAX_COUNT))) {
				res = sent_virtuals.at(lid_number - QE_VIRTUALS_TO_SEND_MIN_ID);
			}
			else res = arg;
			break;
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		default: {
			*ec << "[QE] ERROR in reVirtualize() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return (ErrQExecutor | EOtherResExp);
		}
	}
	return 0;
}

int QueryExecutor::otherErrorOccur( string msg, int errcode ) {
	*ec << msg;
	return errcode;
}

int QueryExecutor::qeErrorOccur( string msg, int errcode ) {
	*ec << msg;
	*ec << (ErrQExecutor | errcode);
	return (ErrQExecutor | errcode);
}

int QueryExecutor::trErrorOccur( string msg, int errcode ) {
	*ec << msg;
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

int QueryExecutor::classesLIDsFromNames(set<string>* names, vector<LogicalID*>& lids) {
	for(set<string>::iterator i = names->begin(); i != names->end(); i++) {
		vector<LogicalID *>* addedClasses;
		int errcode = tr->getClassesLID(*i, addedClasses);
		if(errcode != 0) {
			delete addedClasses;
			return trErrorOccur("[QE] Error in getClassesLID", errcode);
		}
		if(addedClasses->size() == 0) {
			delete addedClasses;
			return qeErrorOccur("[QE] Class: \"" + (*i) + "\" not found.", ENoClassDefFound);
		}
		lids.push_back(addedClasses->at(0));
		delete addedClasses;
	}
	return 0;
}


/* ADTODO - if both class and view of that name exist! */
int QueryExecutor::implementationNameTaken(string name, bool &taken) {
    int errorCode = classExists(name, taken);
    if (errorCode !=0)
	return errorCode;
    if (taken)
	return 0;
    errorCode = viewExists(name, taken);
    if (errorCode !=0)
	return errorCode;
    return 0;
}

int QueryExecutor::interfaceNameTaken(string name, bool& taken) {
    LogicalID *lid = NULL;
    int errorcode = getInterfaceLID(name, lid);
    taken = false;
    if (lid)
	taken = true;
    return errorcode;
}

int QueryExecutor::getInterfaceLID(string name, LogicalID *&lid)
{
    vector<LogicalID *>* interfaces;
    int errorcode;
    if ((errorcode = tr->getInterfacesLID(name, interfaces))!=0) {
	if (interfaces != NULL)
	    delete interfaces;
	return trErrorOccur("[QE] Error in getInterfacesLID", errorcode);
    }
    if (interfaces->size() == 0)
    {
	*ec << "[QE] getInterfaceLID - no interface " << name;
    }
    else
	lid = interfaces->at(0);
    return 0;
}


int QueryExecutor::classExists(string className, bool& exist) {
	vector<LogicalID *>* addedClasses;
	int errcode = tr->getClassesLID(className, addedClasses);
	if(errcode != 0) {
		delete addedClasses;
		return trErrorOccur("[QE] Error in getClassesLID", errcode);
	}
	exist = addedClasses->size() != 0;
	delete addedClasses;
	return 0;
}

/* ADTODO - implement */
int QueryExecutor::viewExists(string viewName, bool& exists) {
    exists = false;
    return 0;
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

int QueryExecutor::lidFromBinder( string bindName, QueryResult* result, LogicalID*& lid) {
	QueryResult *tmp_bndr =new QueryBinderResult(bindName, result);
	ObjectPointer *tmp_optr;
	int errcode = objectFromBinder(tmp_bndr, tmp_optr);
	if (errcode != 0) return errcode;
	lid= tmp_optr->getLogicalID();
	return 0;
}

int QueryExecutor::createObjectAndPutOnQRes(DBDataValue* dbValue, string objectName, int type, LogicalID*& newLid) {
	int errcode = 0;
	ObjectPointer *newObject;
	DataValue* value = dbValue;
	if ((errcode = tr->createObject(objectName, value, newObject)) != 0) {
		*ec << "[QE] Error in createObject";
		antyStarveFunction(errcode);
		inTransaction = false;
		return errcode;
	}

	if(Store::None != type) {
		newObject->getValue()->setSubtype(Store::Class);
		errcode = tr->modifyObject(newObject, newObject->getValue());
		if (errcode != 0) {
			*ec << "[QE] TNCLASS - Error in modifyObject.";
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

QueryExecutor::~QueryExecutor() {
	if (inTransaction) tr->abort();
	inTransaction = false;
	delete envs;
	delete qres;
	sent_virtuals.clear();
	fakeLid_map.clear();
	*ec << "[QE] QueryExecutor shutting down\n";
	delete ec;
	//delete QueryBuilder::getHandle();
}


/* Typedef and object declaration execution methods */

int QueryExecutor::executeObjectDeclarationOrTypeDefinition(DeclareDefineNode *obdNode, bool isTypedef) {
	int errcode = 0;
	vector<string> *vec = new vector<string>();
	//we COULD,instead, replace the node if it was same as isTypedef; and throw ERROR only if it was the opposite.
	if ((tr->getDmlStct()->findRoot(obdNode->getName())) != DMLControl::absent) {
		return (ErrQExecutor | ESuchMdnExists);
	}
	errcode = objDeclRec(obdNode, obdNode->getName(), isTypedef, obdNode->getName(), vec, true);
	if (errcode != 0) return errcode;

	for (unsigned int i = 0; i < vec->size(); i++) {*ec << "qry: " + vec->at(i);}
	QueryParser parser(false);
	QueryResult *local_res = NULL;
	int locResult = 0;
	for (unsigned int i = 0; i < vec->size(); i++) {
		locResult += execute_locally(vec->at(i), &local_res, parser);
	}
	if (locResult != 0)	return (ErrQExecutor | EMdnCreateError);
	tr->getDmlStct()->addRoot(obdNode->getName(), (isTypedef ? DMLControl::type : DMLControl::object));
	*ec << tr->getDmlStct()->depsToString();
	*ec << tr->getDmlStct()->rootMdnsToString();
	DataScheme::dScheme(session->get_id(), tr)->setUpToDate(false);
	return 0;
}

int QueryExecutor::objDeclRec(DeclareDefineNode *obd, string rootName, bool typeDef, string ownerName, vector<string> *queries, bool isTopLevel) {
	string query = "";
	string objName = (isTopLevel ? TC_MDN_NAME : TC_SUB_OBJ_NAME);
	int errcode = 0;
	if (isTopLevel && typeDef) {
		query = QueryBuilder::getHandle()->query_create_typedef(obd->getName(), ((TypeDefNode *)obd)->getIsDistinct(), obd->getSigNode()->getHeadline());
	} else {
		query = QueryBuilder::getHandle()->query_create_obj_mdn(obd->getName(), ((ObjectDeclareNode *)obd)->getCard(), obd->getSigNode()->getHeadline(), objName);
	}
	queries->push_back(query);	// base query for declaration

	int sigKind =  obd->getSigNode()->getSigKind();
	string tName = "";
	vector<ObjectDeclareNode*> *subs = NULL;
	switch (sigKind) {
		case SignatureNode::atomic : break; //nothing needs be done here.
		case SignatureNode::ref : break;//Not inserting target-not needed by new DScheme.(though still be handled well)
	//QBuilder::query_insert_target(0, obdNode->getName(), false, obdNode->getSigNode()->getTypeName());push_back(query);
		case SignatureNode::defType :
			tName = obd->getSigNode()->getTypeName();
			//check if its an object. (if so - well, error, can't do that, only defined type name allowed here).
			if ((tr->getDmlStct()->findRoot(tName)) == DMLControl::object) {
				*ec << "Found MDN object while looking for type - have to report error";
				return (ErrQExecutor | EObjectInsteadOfType);
			}
			if (typeDef) {//If not in typedef -were done. else - check for recurrence & update dependency structs.
				*ec << "will look for dependencies..\n";
				if (tr->getDmlStct()->findDependency(tName, rootName) != 0) {
					*ec << "Attempt to define inftly recurrent types: " + tName + " <--> " + obd->getName();
					return (ErrQExecutor | ERecurrentTypes);
				}
				tr->getDmlStct()->markDependency (rootName, tName);
			}
			break;
		case SignatureNode::complex :
			subs = obd->getSigNode()->getStructArg()->getSubDeclarations();
			for (unsigned int i = 0; i < subs->size(); i++) {
				errcode = objDeclRec(subs->at(i), rootName, typeDef, obd->getName(), queries, false);
				if (errcode != 0) {
					return errcode;
				}
			}
			break;
		default: *ec << "Unknown signature node kind"; break;
	}
	if (!isTopLevel) {
		//decide whether owner(parent node) is __MDN__, or __MDNT__, or some subobject somewhere...
		int ownerBase = (ownerName == rootName ? (typeDef ? 1: 0) : 2);
		queries->push_back(QueryBuilder::getHandle()->query_insert_owner(obd->getName(), ownerName, ownerBase));
		queries->push_back(QueryBuilder::getHandle()->query_insert_subobj(ownerBase, ownerName, obd->getName()));
	}
	return 0;
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
		return (ErrQExecutor | EQEmptySet);
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

int QueryExecutor::initCg() { return ClassGraph::getHandle(cg); }

ClassGraph* QueryExecutor::getCg() { return cg; }


    /*
     *	Query Builder begin
     */
    QueryBuilder* QueryBuilder::builder = NULL;

    void QueryBuilder::startup() {
    	builder = new QueryBuilder();
    }

    void QueryBuilder::shutdown() {
    	delete builder;
    }

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

	string QueryBuilder::query_create_obj_mdn (string name, string card, string interior, string mainStr) {
		return "create (\"" + name + "\" as name, \"" + card + "\" as card, " + interior + ") as " + mainStr;
	}

	string QueryBuilder::query_create_typedef(string name, bool distinct, string interior) {
		string isDistinct = (distinct ? "1" : "0");
		return "create (\"" + name + "\" as name, \"" + isDistinct + "\" as isDistinct, " + interior + ") as " + TC_MDNT_NAME;
	}

	string QueryBuilder::query_insert_target(int base, string baseName, bool isTgtTypedef, string tgtName) {
		string query = "("+baseStr(base)+" where name=\"" +baseName + "\")";
		query += " :< (create (";
		//if (isTgtTypedef) query += TC_MDNT_NAME; else query += TC_MDN_NAME;
		query += (isTgtTypedef ? TC_MDNT_NAME : TC_MDN_NAME);
		query += " where name=\""+tgtName+"\") as target)";
		return query;
	}
	string QueryBuilder::query_insert_owner(string name, string ownerName, int ownerBase) {
		string query = "(subobject where name=\""+name+"\") :< ";
		query += "(create (" + baseStr(ownerBase)+" where name=\""+ownerName+"\") as owner);";
		return query;
	}

	string QueryBuilder::query_insert_subobj(int ownerBase, string ownerName, string subName) {
		string query = "(" + baseStr(ownerBase) + " where name=\""+ownerName+"\") :< ";
		query += "(subobject where name = \"" + subName +"\");";
		return query;
	}

	string QueryBuilder::baseStr(int baseCd) {
		switch (baseCd) {
			case 0 : return TC_MDN_NAME;
			case 1 : return TC_MDNT_NAME;
			case 2 : return TC_SUB_OBJ_NAME;
			default: return TC_MDN_NAME;
		}
	}
    /*
     *	Query Builder end
     */

}
