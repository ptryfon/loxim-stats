#include "QueryBuilder.h"
#include "TypeCheck/TCConstants.h"

namespace QExecutor
{
	QueryBuilder* QueryBuilder::builder = NULL;

	void QueryBuilder::startup() 
	{
		builder = new QueryBuilder();
	}

	void QueryBuilder::shutdown() 
	{
		delete builder;
	}

	QueryBuilder* QueryBuilder::getHandle() 
	{
		return builder;
	}

	QueryBuilder::QueryBuilder()  {}

	QueryBuilder::~QueryBuilder() {}

	string QueryBuilder::create_user_query(string login, string passwd) 
	{
		string query = "create (\"" + login + "\" as login, \"" + passwd + "\" as password) as xuser";
		return query;
	}

	string QueryBuilder::remove_user_query(string login) 
	{
		string query = "delete (xuser where login = \"" + login + "\")";
		return query;
	}

	string QueryBuilder::grant_priv_query(string priv, string object, string user, int grant_option) 
	{
		char str_grant_option[2];
		if (grant_option) 
			sprintf(str_grant_option, "%d", 1);
		else
			sprintf(str_grant_option, "%d", 0);

		string lvalue = "(xuser where login=\"" + user + "\")";
		string rvalue = "(create (\"" + priv + "\" as priv_name, \"" + object + "\" as object_name, "
		+ str_grant_option + " as grant_option) as xprivilige)";
		string oper   = " :< ";

		string query = lvalue + oper + rvalue;
		return query;
	}

	string QueryBuilder::revoke_priv_query(string priv, string object, string user) 
	{
		string user_query = "(xuser where login=\"" + user + "\")";
		string priv_query = "(xprivilige where priv_name=\"" + priv + "\" and object_name=\"" + object + "\")";
		string query = "delete (" + user_query + "." + priv_query + ")";
		return query;
	}
	
	string QueryBuilder::revoke_all_privs_query(string user)
	{
		string query = "delete ((xuser where login=\"" + user + "\").xprivilige)";
		return query;
	}

	string QueryBuilder::query_for_user_priviliges(string login)
	{
		string query = "deref((xuser where login=\"" + login + "\").xprivilige)";
		return query;		
	}
	
	string QueryBuilder::query_for_privilige(string user, string priv, string object) 
	{
		string user_query = "(xuser where login=\"" + user + "\")";
		string priv_query = "(xprivilige where priv_name=\"" + priv + "\" and object_name=\"" + object + "\")";
		string query = "count (" + user_query + "." + priv_query + ") > 0";
		return query;
	}

	string QueryBuilder::query_for_privilige_grant(string user, string priv, string object) 
	{
		string user_query = "(xuser where login=\"" + user + "\")";
		string priv_query = "(xprivilige where priv_name=\"" + priv + "\" and object_name=\"" + object + "\" and "
		+ "grant_option=1)";
		string query = "count (" + user_query + "." + priv_query + ") > 0";
		return query;
	}

	string QueryBuilder::query_for_password(string user, string password) 
	{
		string query = "count (xuser where login=\"" + user + "\" and password=\"" + password + "\") > 0";
		return query;
	}

	string QueryBuilder::query_create_obj_mdn (string name, string card, string interior, string mainStr) 
	{
		return "create (\"" + name + "\" as name, \"" + card + "\" as card, " + interior + ") as " + mainStr;
	}

	string QueryBuilder::query_create_typedef(string name, bool distinct, string interior) 
	{
		string isDistinct = (distinct ? "1" : "0");
		return "create (\"" + name + "\" as name, \"" + isDistinct + "\" as isDistinct, " + interior + ") as " + TC_MDNT_NAME;
	}

	string QueryBuilder::query_insert_target(int base, string baseName, bool isTgtTypedef, string tgtName) 
	{
		string query = "("+baseStr(base)+" where name=\"" +baseName + "\")";
		query += " :< (create (";
		//if (isTgtTypedef) query += TC_MDNT_NAME; else query += TC_MDN_NAME;
		query += (isTgtTypedef ? TC_MDNT_NAME : TC_MDN_NAME);
		query += " where name=\""+tgtName+"\") as target)";
		return query;
	}

	string QueryBuilder::query_insert_owner(string name, string ownerName, int ownerBase) 
	{
		string query = "(subobject where name=\""+name+"\") :< ";
		query += "(create (" + baseStr(ownerBase)+" where name=\""+ownerName+"\") as owner);";
		return query;
	}

	string QueryBuilder::query_insert_subobj(int ownerBase, string ownerName, string subName) 
	{
		string query = "(" + baseStr(ownerBase) + " where name=\""+ownerName+"\") :< ";
		query += "(subobject where name = \"" + subName +"\");";
		return query;
	}

	string QueryBuilder::baseStr(int baseCd) 
	{
		switch (baseCd) 
		{
			case 0 : return TC_MDN_NAME;
			case 1 : return TC_MDNT_NAME;
			case 2 : return TC_SUB_OBJ_NAME;
			default: return TC_MDN_NAME;
		}
	}
}
