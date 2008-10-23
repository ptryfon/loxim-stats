package pl.edu.mimuw.loxim.protocol.packages_data;

import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;


public class DataPackagesFactory implements pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackagesFactory{

	private static DataPackagesFactory instance;

	public synchronized static DataPackagesFactory getInstance() {
		if (instance == null)
			instance = new DataPackagesFactory();
		return instance;
	}

	public Package createPackage(long /*long*/ package_id) throws ProtocolException {
										if (package_id==Uint8Package.ID) return new Uint8Package();
											if (package_id==Sint8Package.ID) return new Sint8Package();
											if (package_id==Uint16Package.ID) return new Uint16Package();
											if (package_id==Sint16Package.ID) return new Sint16Package();
											if (package_id==Uint32Package.ID) return new Uint32Package();
											if (package_id==Sint32Package.ID) return new Sint32Package();
											if (package_id==Uint64Package.ID) return new Uint64Package();
											if (package_id==Sint64Package.ID) return new Sint64Package();
											if (package_id==BoolPackage.ID) return new BoolPackage();
											if (package_id==DatePackage.ID) return new DatePackage();
											if (package_id==TimePackage.ID) return new TimePackage();
											if (package_id==DatetimePackage.ID) return new DatetimePackage();
											if (package_id==TimetzPackage.ID) return new TimetzPackage();
											if (package_id==DatetimetzPackage.ID) return new DatetimetzPackage();
											if (package_id==BobPackage.ID) return new BobPackage();
											if (package_id==VarcharPackage.ID) return new VarcharPackage();
											if (package_id==DoublePackage.ID) return new DoublePackage();
											if (package_id==VoidPackage.ID) return new VoidPackage();
											if (package_id==LinkPackage.ID) return new LinkPackage();
											if (package_id==BindingPackage.ID) return new BindingPackage();
																if (package_id==StructPackage.ID) return new StructPackage();
											if (package_id==BagPackage.ID) return new BagPackage();
											if (package_id==SequencePackage.ID) return new SequencePackage();
											if (package_id==RefPackage.ID) return new RefPackage();
											if (package_id==External_refPackage.ID) return new External_refPackage();
							throw new ProtocolException("Cannot create instance of package with id:"+package_id+". Unknown package.");
	}
}


