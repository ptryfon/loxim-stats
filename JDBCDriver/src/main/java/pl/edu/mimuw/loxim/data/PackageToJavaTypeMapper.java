package pl.edu.mimuw.loxim.data;

import java.math.BigInteger;
import java.util.Calendar;

import pl.edu.mimuw.loxim.protocol.packages_data.BagPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BindingPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BobPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BoolPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DatePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DatetimePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DatetimetzPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.DoublePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.External_refPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.LinkPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.RefPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.SequencePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint16Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint32Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint64Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Sint8Package;
import pl.edu.mimuw.loxim.protocol.packages_data.StructPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.TimePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.TimetzPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint16Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint32Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint64Package;
import pl.edu.mimuw.loxim.protocol.packages_data.Uint8Package;
import pl.edu.mimuw.loxim.protocol.packages_data.VarcharPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.VoidPackage;

public final class PackageToJavaTypeMapper {

	public static Class<?> getJavaClass(Long packageDataType) {
		if (packageDataType == null) {
			return null;
		}
		
		switch (packageDataType.intValue()) {
		case (int) Uint8Package.ID:
			return Short.class;
		
		case (int) Uint16Package.ID:
			return Integer.class;
		
		case (int) Uint32Package.ID:
			return Long.class;
		
		case (int) Uint64Package.ID:
			return BigInteger.class;
		
		case (int) Sint8Package.ID:
			return Byte.class;
		
		case (int) Sint16Package.ID:
			return Short.class;
		
		case (int) Sint32Package.ID:
			return Integer.class;
		
		case (int) Sint64Package.ID:
			return Long.class;
		
		case (int) BoolPackage.ID:
			return Boolean.class;
		
		case (int) DatePackage.ID:
			return Calendar.class;
		
		case (int) TimePackage.ID:
			return Calendar.class;
		
		case (int) DatetimePackage.ID:
			return Calendar.class;
		
		case (int) TimetzPackage.ID:
			return Calendar.class;
		
		case (int) DatetimetzPackage.ID:
			return Calendar.class;
		
		case (int) DoublePackage.ID:
			return Double.class;

			// multi
		case (int) BobPackage.ID:
			return String.class;
		
		case (int) VarcharPackage.ID:
			return String.class;
			
		// complex types
			// single
		case (int) VoidPackage.ID:
			return Void.class;
		
		case (int) LinkPackage.ID:
			return Link.class;
			
		case (int) BindingPackage.ID:
			return Binding.class;
			
		case (int) RefPackage.ID:
			return Reference.class;
			
		case (int) External_refPackage.ID:
			return ExtReference.class;
			
			// multi
		case (int) BagPackage.ID:
			return Bag.class;
		
		case (int) StructPackage.ID:
			return Struct.class;
		
		case (int) SequencePackage.ID:
			return Sequence.class;
		
		default:
			return null;
		}
	}
	
}
