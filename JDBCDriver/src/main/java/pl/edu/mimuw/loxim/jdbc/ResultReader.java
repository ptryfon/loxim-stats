package pl.edu.mimuw.loxim.jdbc;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Collection;
import java.util.List;
import java.util.SimpleTimeZone;

import pl.edu.mimuw.loxim.data.Binding;
import pl.edu.mimuw.loxim.data.BindingImpl;
import pl.edu.mimuw.loxim.data.ExtReference;
import pl.edu.mimuw.loxim.data.ExtReferenceImpl;
import pl.edu.mimuw.loxim.data.GenericCollection;
import pl.edu.mimuw.loxim.data.Link;
import pl.edu.mimuw.loxim.data.LinkImpl;
import pl.edu.mimuw.loxim.data.LoXiMCollection;
import pl.edu.mimuw.loxim.data.Reference;
import pl.edu.mimuw.loxim.data.ReferenceImpl;
import pl.edu.mimuw.loxim.data.VoidImpl;
import pl.edu.mimuw.loxim.protocol.enums.Send_value_flagsEnum;
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
	
	private static class DataBuffer {
		private List<Package> buffer = new ArrayList<Package>();
		private long type = -1;
		private static final long[] tbcPackages = {BobPackage.ID, VarcharPackage.ID, BagPackage.ID, StructPackage.ID, SequencePackage.ID};
		
		static {
			Arrays.sort(tbcPackages);
		}
		
		public void append(Package chunk) throws ProtocolException {
			if (type == -1) {
				type = chunk.getPackageType();
			} else if (type != chunk.getPackageType()) {
				throw new ProtocolException("Error while reading TO_BE_CONTINUED data. Expecting type " + type + " but was " + chunk.getPackageType());
			}
			if (!buffer.isEmpty() && Arrays.binarySearch(tbcPackages, chunk.getPackageType()) < 0) {
				throw new ProtocolException("Error while reading TO_BE_CONTINUED data. Type " + chunk.getPackageType() + " cannot be split");
			}
			buffer.add(chunk);
		}
		
		public Package consolidate() {
			Package consolidated = buffer.get(0);
			
			switch ((int) type) {
			case (int) BobPackage.ID:
				StringBuilder bobBuf = new StringBuilder();
				for (BobPackage chunk : (List<BobPackage>) (List) buffer) {
					bobBuf.append(chunk.getValue());
				}
				BobPackage bob = (BobPackage) consolidated;
				bob.setValue(bobBuf.toString());
				break;

			case (int) VarcharPackage.ID:
				StringBuilder vcBuf = new StringBuilder();
				for (VarcharPackage chunk : (List<VarcharPackage>) (List) buffer) {
					vcBuf.append(chunk.getValue());
				}
				VarcharPackage vc = (VarcharPackage) consolidated;
				vc.setValue(vcBuf.toString());
				break;
				

			case (int) BagPackage.ID:
			case (int) StructPackage.ID:
			case (int) SequencePackage.ID:
				CollectionPackage cPac = (CollectionPackage) consolidated;
				Long globalType = cPac.getGlobalType();
				long count = 0;
				List<Package> dataParts = new ArrayList<Package>();
	
				for (CollectionPackage chunk : (List<CollectionPackage>) (List) buffer) {
					if (globalType != null) {
						if (chunk.getGlobalType() == null) {
							globalType = null;
						} else {
							globalType += chunk.getGlobalType();
						}
					}
					
					count += chunk.getCount();
					
					dataParts.addAll(Arrays.asList(chunk.getDataParts())); // XXX long data parts?
				}
				
				cPac.setDataParts(dataParts.toArray(new Package[0]));
				cPac.setGlobalType(globalType);
				cPac.setCount(count);
				return cPac;
				
			default:
				break;
			}
			
			buffer.clear();
			type = -1;
			return consolidated;
		}
		
		public boolean isEmpty() {
			return buffer.isEmpty();
		}
	}
	
	private PackageIO pacIO;
	
	public ResultReader(PackageIO pacIO) {
		this.pacIO = pacIO;
	}
	
	public List<Object> readValues() throws ProtocolException {
		List<Object> results = new ArrayList<Object>();
		DataBuffer buf = new DataBuffer();

		for (Package pac = PackageUtil.readPackage(pacIO, Package.class); pac.getPackageType() == V_sc_sendvaluePackage.ID; pac = PackageUtil.readPackage(pacIO, Package.class)) {
			V_sc_sendvaluePackage vPac = (V_sc_sendvaluePackage) pac;
			buf.append(vPac.getData());
			
			if (!vPac.getFlags().contains(Send_value_flagsEnum.svf_to_be_continued)) {
				results.add(readValue(buf.consolidate()));
			}
		}
		
		return results;
	}
	
	private Object readValue(Package dataPac) throws ProtocolException {
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
			return ((Uint8Package) dataPac).getValue();
		
		case (int) Sint16Package.ID:
			return ((Uint8Package) dataPac).getValue();
		
		case (int) Sint32Package.ID:
			return ((Uint8Package) dataPac).getValue();
		
		case (int) Sint64Package.ID:
			return ((Uint8Package) dataPac).getValue();
		
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
			// TODO set value
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
		case (int) StructPackage.ID:
		case (int) SequencePackage.ID:
			LoXiMCollection collection = new GenericCollection();
			CollectionPackage cPac = (CollectionPackage) dataPac;
			Collection<Object> data = collection.getData();
			for (Package p : cPac.getDataParts()) {
				data.add(readValue(p));
			}
			// TODO type mapping collection.setGlobalType(globalType);
			return collection;
		default:
			throw new ProtocolException("Unhandled value type: " + dataPac.getPackageType());
		}
	}
	
}
