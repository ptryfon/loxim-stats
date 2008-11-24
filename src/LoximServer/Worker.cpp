#include <protocol/packages/VSCAbortPackage.h>
#include <protocol/packages/ASCByePackage.h>
#include <protocol/packages/VSCSendValuesPackage.h>
#include <protocol/packages/QCStatementPackage.h>
#include <protocol/packages/ASCSetOptPackage.h>
#include <protocol/packages/QCExecutePackage.h>
#include <protocol/packages/ASCPongPackage.h>

#include <LoximServer/Worker.h>

namespace LoximServer{


	void *W_starter(void *arg)
	{
		((Worker*)arg)->start_continue();
		return NULL;
	}

	Worker::Worker(LoximSession *session, ErrorConsole *err_cons)
	{
		this->session = session;
		this->err_cons = err_cons;
		this->cur_package = 0;
		pthread_mutex_init(&mutex, 0);
		pthread_cond_init(&idle_cond, 0);
		pthread_cond_init(&completion_cond, 0);
		shutting_down = 0;
	}

	void Worker::start()
	{
		pthread_create(&thread, NULL, ::LoximServer::W_starter, this);
	}

	void Worker::start_continue()
	{
		//main loop
		int res;
		int error;
		pthread_mutex_lock(&mutex);
		while (true){
			pthread_cond_wait(&idle_cond, &mutex);
			if (shutting_down){
				pthread_cond_signal(&completion_cond);
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}
			pthread_mutex_unlock(&mutex);
			/* worker_mutex is acquired inside */
			res = process_package(cur_package, &error);
			delete cur_package;
			cur_package = NULL;
			pthread_cond_signal(&completion_cond);
			if (shutting_down){
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}
			if (!res){
				session->error = error;
				session->shutdown();
				pthread_mutex_unlock(&mutex);
				pthread_exit(NULL);
			}
		}
	}

	void Worker::cancel_job(bool synchronous, bool mutex_locked)
	{
		session->qEx->stopExecuting();
		if (synchronous){
			if (!mutex_locked)
				pthread_mutex_lock(&mutex);
			if (cur_package)
				pthread_cond_wait(&completion_cond, &mutex);
			if (!mutex_locked)
				pthread_mutex_unlock(&mutex);
		}
	}

	void Worker::stop()
	{
		shutting_down = 1;
		cancel_job(false, false);
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&idle_cond);
		pthread_mutex_unlock(&mutex);
		pthread_join(thread, NULL);
	}

	/*
	 * This can actually be tweaked as not everything needs to between lock
	 * and unlock
	 */
	int Worker::submit(Package *package)
	{
		pthread_mutex_lock(&mutex);
		if (package->getPackageType() == ID_ASCPongPackage){
			delete package;
			pthread_mutex_unlock(&mutex);
			return SUBM_SUCCESS;
		}
			
		if (package->getPackageType() == ID_VSCAbortPackage){
			if (cur_package){
				cancel_job(true, true);
				session->qEx->contExecuting();
			}
			/* 
			 * ignore aborts when they are inappropriate, because
			 * thay may be simply late
			 */
			pthread_mutex_unlock(&mutex);
			return SUBM_SUCCESS;
		}
		//there is a package being worked on and an inappropriate package
		//is received
		if (cur_package){
			/* no need to wait for it */
			cancel_job(false, true);
			pthread_mutex_unlock(&mutex);
			return SUBM_ERROR;
		}
		//no package is being worked on
		if (package->getPackageType() == ID_ASCByePackage){
			err_cons->printf("Client closed connection\n");
			pthread_mutex_unlock(&mutex);
			delete package;
			return SUBM_EXIT;
		} else { 
			//a regular package
			cur_package = package;
			pthread_cond_signal(&idle_cond);
			pthread_mutex_unlock(&mutex);
			return SUBM_SUCCESS;
		}
	}
	
	
	/**
	 * on return, mutex is locked. It shouldn't be locked on entry
	 * returns 0 when the connection should be closed and !0 otherwise
	 */
	int Worker::process_package(Package *package, int *error)
	{
		switch (package->getPackageType()){
			case ID_VSCSendValuesPackage :
				err_cons->printf("Got VSCSendValues - ignoring\n");
				break;
			case ID_QCStatementPackage:{
					QCStatementPackage *stmt_pkg = (QCStatementPackage*)package;
					if (stmt_pkg->getFlags() & STATEMENT_FLAG_EXECUTE){
						err_cons->printf("Got SCStatement - executing\n");
						QueryResult *qres;
						/*QSExecutingPackage confirmation;
						  if (!layer0->writePackage(&confirmation)){
						  return EProtocol;
						  */
						*error = session->execute_statement(stmt_pkg->getStatement()->getBuffor(), &qres);
						pthread_mutex_lock(&mutex);
						if (*error != (ErrQExecutor | EEvalStopped) && (*error != 0)){
							if ((*error = session->send_error(*error))){
								return 0;
							}
						} else if (*error == 0){
                                                        //DataPart *sres = session->serialize_res(qres);
							if ((*error = session->respond(session->serialize_res(qres)))){
                                                                //delete sres;
                                                                //delete qres;
								return 0;
							}
                                                        //delete sres;
                                                        //delete qres;
						} else
							/* execution was aborted */
							return 1;
					} else {
						err_cons->printf("Got SCStatement - ignoring\n");
						pthread_mutex_lock(&mutex);
					}
				}
				break;
			case ID_ASCSetOptPackage:
				err_cons->printf("SCSetOpt - ignoring\n");
				pthread_mutex_lock(&mutex);
				break;
			case ID_QCExecutePackage:
				err_cons->printf("Got SCExecute - ignoring\n");
				pthread_mutex_lock(&mutex);
				break;
			/* 
			 * there is no Abort, Bye and Pong case but this is on
			 * purpose. To avoid race conditions (there is no 
			 * reply to these), these package types are handled in
			 * the protocol thread
			 */
			default: {
					 err_cons->printf("Unexpected package of type %d\n", package->getPackageType());
					 *error = EProtocol;
					 pthread_mutex_lock(&mutex);
					 return 0;
				 }
		}
		return 1;
	}
}
