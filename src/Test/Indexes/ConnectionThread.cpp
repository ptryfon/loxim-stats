#include <Test/Indexes/ConnectionThread.h>
#include <assert.h>

using namespace QParser;
namespace IndexTesting {
	
			
	ConnectionThread::ConnectionThread() {
		qPa.reset(new QueryParser());
		qEx.reset(new QueryExecutor());
		
		//ustawienie pelnych uprawnien
		qEx->set_priviliged_mode(true);
		UserData *user_data = new UserData("root", "");
	    qEx->session_data->set_user_data(user_data);
	    int errcode = qEx->initCg();
	    assert(errcode == 0);
	}

	int ConnectionThread::process(string query, QueryResult* &qResult) {
		TreeNode *tNode;
		int res;
		res = (qPa->parseIt(query, tNode));
		if (res) {
			return res;
		}
		
		return qEx->executeQuery(tNode, &qResult);
		
	}
	
	int ConnectionThread::process(string query) {
		QueryResult* qres;
		return process(query, qres);
	}

	int ConnectionThread::process(string query, QueryResult::QueryResultType resultType) {
		QueryResult* res;
		int err = process(query, res);
		if (err) {
			return err;
		}
		return res->type() == resultType ? 0 : -53; //zwrocono inny typ niz oczekiwano
	}

	int ConnectionThread::begin() {
		return process("begin", QueryResult::QNOTHING);
	}

	int ConnectionThread::end() {
		return process("end", QueryResult::QNOTHING);
	}

	int ConnectionThread::abort() {
		return process("abort", QueryResult::QNOTHING);
	}
		
}
