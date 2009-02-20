#ifndef _QUERYBUILDER_H_
#define _QUERYBUILDER_H_

#include <string>

using namespace std;

namespace QExecutor
{
	class QueryBuilder 
	{
		private:
			static QueryBuilder *builder;
			QueryBuilder();		
		public:
			static QueryBuilder* getHandle();
			static void startup();
			static void shutdown();
			~QueryBuilder();

			string create_user_query(string user, string passwd);
			string remove_user_query(string user);
			string grant_priv_query(string priv, string object, string user, int grant_option = 0);
			string revoke_priv_query(string priv, string object, string user);
			string query_for_privilige(string user, string privilige, string object);
			string query_for_privilige_grant(string user, string privilige, string object);
			string query_for_password(string user, string password);

			string query_create_obj_mdn(string name, string card, string interior, string mainStr);		
			string query_insert_target(int base, string baseName, bool isTgtTypedef, string tgtName);
			string query_insert_owner(string name, string ownerName, int ownerBase);
			string query_insert_subobj(int ownerBase, string ownerName, string subName);
			string query_create_typedef(string name, bool distinct, string interior);
			string baseStr(int baseCd);
	};
}

#endif //_QUERYBUILDER_H_
