package pl.edu.mimuw.loxim.protocol.packages;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;

import pl.edu.mimuw.loxim.protogen.lang.java.template.base_packages.ASCPingPackage;
import pl.edu.mimuw.loxim.protogen.lang.java.template.base_packages.ASCPongPackage;

public class PackagesFactory implements pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackagesFactory{

	private static PackagesFactory instance;

	public synchronized static PackagesFactory getInstance() {
		if (instance == null)
			instance = new PackagesFactory();
		return instance;
	}

	public Package createPackage(long /*short*/ package_id) throws ProtocolException {
				if (package_id==ASCPingPackage.ID) return new ASCPingPackage();
		if (package_id==ASCPongPackage.ID) return new ASCPongPackage();
										if (package_id==W_c_helloPackage.ID) return new W_c_helloPackage();
											if (package_id==W_s_helloPackage.ID) return new W_s_helloPackage();
											if (package_id==W_c_modePackage.ID) return new W_c_modePackage();
											if (package_id==W_c_loginPackage.ID) return new W_c_loginPackage();
											if (package_id==W_c_authorizedPackage.ID) return new W_c_authorizedPackage();
											if (package_id==W_c_passwordPackage.ID) return new W_c_passwordPackage();
											if (package_id==Q_c_statementPackage.ID) return new Q_c_statementPackage();
											if (package_id==Q_s_stmtparsedPackage.ID) return new Q_s_stmtparsedPackage();
											if (package_id==Q_c_executePackage.ID) return new Q_c_executePackage();
											if (package_id==Q_s_executingPackage.ID) return new Q_s_executingPackage();
											if (package_id==Q_s_execution_finishedPackage.ID) return new Q_s_execution_finishedPackage();
											if (package_id==A_sc_okPackage.ID) return new A_sc_okPackage();
											if (package_id==A_sc_errorPackage.ID) return new A_sc_errorPackage();
											if (package_id==A_sc_byePackage.ID) return new A_sc_byePackage();
											if (package_id==A_sc_setoptPackage.ID) return new A_sc_setoptPackage();
											if (package_id==V_sc_sendvaluesPackage.ID) return new V_sc_sendvaluesPackage();
											if (package_id==V_sc_sendvaluePackage.ID) return new V_sc_sendvaluePackage();
											if (package_id==V_sc_finishedPackage.ID) return new V_sc_finishedPackage();
											if (package_id==V_sc_abortPackage.ID) return new V_sc_abortPackage();
							throw new ProtocolException("Cannot create instance of package with id:"+package_id+". Unknown package.");
	}
}


