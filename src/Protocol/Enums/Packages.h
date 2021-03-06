#ifndef PROTOCOL__PACKAGES_H
#define PROTOCOL__PACKAGES_H

/* This file is generated by lw_protogen. DO NOT EDIT.*/

namespace Protocol {
	enum Packages {
		A_SC_BYE_PACKAGE = 3,
		A_SC_ERROR_PACKAGE = 2,
		A_SC_OK_PACKAGE = 1,
		A_SC_PING_PACKAGE = 128,
		A_SC_PONG_PACKAGE = 129,
		A_SC_SETOPT_PACKAGE = 4,
		Q_C_EXECUTE_PACKAGE = 66,
		Q_C_STATEMENT_PACKAGE = 64,
		Q_S_EXECUTING_PACKAGE = 67,
		Q_S_EXECUTION_FINISHED_PACKAGE = 68,
		Q_S_STMTPARSED_PACKAGE = 65,
		V_SC_ABORT_PACKAGE = 35,
		V_SC_FINISHED_PACKAGE = 34,
		V_SC_SENDVALUE_PACKAGE = 33,
		V_SC_SENDVALUES_PACKAGE = 32,
		W_C_HELLO_PACKAGE = 10,
		W_C_LOGIN_PACKAGE = 13,
		W_C_MODE_PACKAGE = 12,
		W_C_PASSWORD_PACKAGE = 15,
		W_S_AUTHORIZED_PACKAGE = 14,
		W_S_HELLO_PACKAGE = 11
	};
}
#endif /* PROTOCOL__PACKAGES_H */ 
