package pl.edu.mimuw.loxim.jdbc;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.List;
import java.util.SimpleTimeZone;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import pl.edu.mimuw.loxim.data.BagImpl;
import pl.edu.mimuw.loxim.data.Binding;
import pl.edu.mimuw.loxim.data.BindingImpl;
import pl.edu.mimuw.loxim.data.ExtReference;
import pl.edu.mimuw.loxim.data.ExtReferenceImpl;
import pl.edu.mimuw.loxim.data.GenericCollection;
import pl.edu.mimuw.loxim.data.Link;
import pl.edu.mimuw.loxim.data.LinkImpl;
import pl.edu.mimuw.loxim.data.LoXiMCollection;
import pl.edu.mimuw.loxim.data.PackageToJavaTypeMapper;
import pl.edu.mimuw.loxim.data.Reference;
import pl.edu.mimuw.loxim.data.ReferenceImpl;
import pl.edu.mimuw.loxim.data.SequenceImpl;
import pl.edu.mimuw.loxim.data.StructImpl;
import pl.edu.mimuw.loxim.data.VoidImpl;
import pl.edu.mimuw.loxim.protocol.packages.A_sc_okPackage;
import pl.edu.mimuw.loxim.protocol.packages.PackageUtil;
import pl.edu.mimuw.loxim.protocol.packages.V_sc_sendvaluePackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BagPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BindingPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BobPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.BoolPackage;
import pl.edu.mimuw.loxim.protocol.packages_data.CollectionPackage;
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
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.pstreams.PackageIO;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;

class ResultReader {
	
	private static final Log log = LogFactory.getLog(ResultReader.class);
	
	private PackageIO pacIO;
	
	public ResultReader(PackageIO pacIO) {
		this.pacIO = pacIO;
	}
	
	public List<Object> readValues() throws ProtocolException {
		List<Object> results = new ArrayList<Object>();
		log.debug("Reading statement result values");
		int i = 0;
		for (Package pac = pacIO.read(); pac.getPackageType() == V_sc_sendvaluePackage.ID; pac = pacIO.read()) {
			log.debug("Reading result value #" + i);
			Object value = readValue(((V_sc_sendvaluePackage) pac).getData());
			log.debug("Result value #" + i + " is " + value.getClass().getName() + " - " + value);
			results.add(value);
			i++;
		}
		log.debug("Read " + i + " result values");
// FIXME		pacIO.write(new A_sc_okPackage());
		return results;
	}
	
	private Object readValue(Package dataPac) throws ProtocolException {
		log.debug("Reading value from package " + dataPac.getClass().getSimpleName());
		
		switch ((int) dataPac.getPackageType()) {
		
		// simple types
			// single
		case (int) Uint8Package.ID:
			return ((Uint8Package) dataPac).getValue();
		
		case (int) Uint16Package.ID:
			return ((Uint16Package) dataPac).getValue();
		
		case (int) Uint32Package.ID:
			return ((Uint32Package) dataPac).getValue();
		
		case (int) Uint64Package.ID:
			return ((Uint64Package) dataPac).getValue();
		
		case (int) Sint8Package.ID:
			return ((Sint8Package) dataPac).getValue();
		
		case (int) Sint16Package.ID:
			return ((Sint16Package) dataPac).getValue();
		
		case (int) Sint32Package.ID:
			return ((Sint32Package) dataPac).getValue();
		
		case (int) Sint64Package.ID:
			return ((Sint64Package) dataPac).getValue();
		
		case (int) BoolPackage.ID:
			return ((BoolPackage) dataPac).getValue();
		
		case (int) DatePackage.ID:
			DatePackage datePac = (DatePackage) dataPac;
			Calendar calD = Calendar.getInstance();
			calD.clear();
			calD.set(datePac.getYear(), datePac.getMonth(), datePac.getDay());
			return calD;
		
		case (int) TimePackage.ID:
			TimePackage timePac = (TimePackage) dataPac;
			Calendar calT = Calendar.getInstance();
			calT.clear();
			calT.set(Calendar.HOUR_OF_DAY, timePac.getHour());
			calT.set(Calendar.MINUTE, timePac.getMinuts());
			calT.set(Calendar.SECOND, timePac.getSec());
			calT.set(Calendar.MILLISECOND, timePac.getMilis());
			return calT;
		
		case (int) DatetimePackage.ID:
			DatetimePackage datetimePac = (DatetimePackage) dataPac;
			Calendar dtDate = (Calendar) readValue(datetimePac.getDate());
			Calendar dtTime = (Calendar) readValue(datetimePac.getTime());
			dtTime.set(dtDate.get(Calendar.YEAR), dtDate.get(Calendar.MONTH), dtDate.get(Calendar.DATE));
			return dtTime;
		
		case (int) TimetzPackage.ID:
			TimetzPackage timetzPac = (TimetzPackage) dataPac;
			Calendar timeCal = (Calendar) readValue(timetzPac.getTime());
			timeCal.setTimeZone(new SimpleTimeZone(timetzPac.getTz(), "DB-Timezone"));
			return timeCal;

		case (int) DatetimetzPackage.ID:
			DatetimetzPackage datetimetzPac = (DatetimetzPackage) dataPac;
			Calendar dCal = (Calendar) readValue(datetimetzPac.getDate());
			Calendar tCal = (Calendar) readValue(datetimetzPac.getTime());
			tCal.set(dCal.get(Calendar.YEAR), dCal.get(Calendar.MONTH), dCal.get(Calendar.DATE));
			tCal.setTimeZone(new SimpleTimeZone(datetimetzPac.getTz(), "DB-Timezone"));
			return tCal;

		case (int) DoublePackage.ID:
			return ((DoublePackage) dataPac).getValue();

			// multi
		case (int) BobPackage.ID:
			return ((BobPackage) dataPac).getValue();
		
		case (int) VarcharPackage.ID:
			return ((VarcharPackage) dataPac).getValue();
			
		// complex types
			// single
		case (int) VoidPackage.ID:
			return new VoidImpl();
		
		case (int) LinkPackage.ID:
			Link link = new LinkImpl();
			link.setValueId(((LinkPackage) dataPac).getValueId());
			return link;
			
		case (int) BindingPackage.ID:
			Binding binding = new BindingImpl();
			BindingPackage bPac = (BindingPackage) dataPac;
			binding.setBindingName(bPac.getBindingName());
			binding.setValue(readValue(bPac.getValue()));
			return binding;
			
		case (int) RefPackage.ID:
			Reference ref = new ReferenceImpl();
			ref.setValueId(((RefPackage) dataPac).getValueId());
			return ref;
			
		case (int) External_refPackage.ID:
			ExtReference extref = new ExtReferenceImpl();
			External_refPackage erPac = (External_refPackage) dataPac;
			extref.setStamp(erPac.getStamp());
			extref.setValueId(erPac.getValueId());
			return extref;
			
			// multi
		case (int) BagPackage.ID:
			LoXiMCollection bag = new BagImpl();
			fillCollection(bag, (CollectionPackage) dataPac);
			return bag;
			
		case (int) StructPackage.ID:
			LoXiMCollection struct = new StructImpl();
			fillCollection(struct, (CollectionPackage) dataPac);
			return struct;
		
		case (int) SequencePackage.ID:
			LoXiMCollection sequence = new SequenceImpl();
			fillCollection(sequence, (CollectionPackage) dataPac);
			return sequence;

		default:
			throw new ProtocolException("Unhandled value type: " + dataPac.getPackageType());
		}
	}
	
	private void fillCollection(LoXiMCollection collection, CollectionPackage cPac) throws ProtocolException {
		Collection<Object> data = collection.getData();
		for (Package p : cPac.getDataParts()) {
			data.add(readValue(p));
		}
		collection.setGlobalType(PackageToJavaTypeMapper.getJavaClass(cPac.getGlobalType()));
	}
}
