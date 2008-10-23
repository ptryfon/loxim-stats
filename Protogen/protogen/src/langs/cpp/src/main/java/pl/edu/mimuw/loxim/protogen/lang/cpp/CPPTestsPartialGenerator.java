package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.StreamsHelpers;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketGroupType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.values.FilledPackage;
import pl.edu.mimuw.loxim.protogen.api.values.UniversalValuesSource;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class CPPTestsPartialGenerator extends CPPAbstractPackagesGenerator {

	public void generateTests(ProtocolTypeWrapper protocol_wrapper)
			throws GeneratorException {

		generateTestPackagesFactoryCpp(protocol_wrapper);
		generateMakefile(protocol_wrapper);
	}

	private void generateMakefile(ProtocolTypeWrapper protocol_wrapper) throws GeneratorException{
		File f = new File(getTests_directory(), "Makefile");
		try {
			CodeCPPPrintStream out=new CodeCPPPrintStream(f);
			StreamsHelpers.writeWholeInputStream(out, this.getClass().getResourceAsStream("Makefile-tests-prefix"));
			out.blankLine();
			for(String group_name:protocol_wrapper.getPackageGroupWrappers().keySet())
			{
				if(group_name.equals(""))
					out.printCode(" ../packages/packages.a \\");
				else
					out.printCode(" ../packages_"+group_name+"/packages_"+group_name+".a \\");
			}
			StreamsHelpers.writeWholeInputStream(out, this.getClass().getResourceAsStream("Makefile-tests-suffix"));
			out.close();
		} catch (FileNotFoundException e) {
			throw new GeneratorException(e);
		} catch (IOException e) {
			throw new GeneratorException(e);
		} 
	}

	private File getTests_directory() {
		File f=new File(new File(getTarget_directory(),"protocol"), "tests");
		f.mkdirs();
		return f;
	}

	private String createAndInitPackageCode(FilledPackage fp) {
		PackageTypeWrapper ptw = fp.getPackageWrapper();
		StringBuffer res = new StringBuffer();
		res.append("new " + ptw.getClassName() + "(");
		// out.indent();
	
		Iterator<FieldType> iterator = ptw.getPackage().getField().iterator();
		while (iterator.hasNext()) {
			FieldType f = iterator.next();
			Object v = fp.getValue(f.getName());
			if((f.getType()==Type.VARUINT)&&(f.isNullable())&&(v==null))
			{
				res.append("0");
			}
			else{
				res.append(formatValue(v, f));
			}			
			if (iterator.hasNext())
				res.append(",");
			
		}
		for(FieldType f:ptw.getPackage().getField())
		{
			if((f.getType()==Type.VARUINT)&&(f.isNullable()))
			{
				if (fp.getValue(f.getName())==null)
					res.append(",true");
				else
					res.append(",false");
			}
		}
		res.append(")");
		return res.toString();
		// out.deindent();
	}

	private void generateTestPackagesFactoryCpp(
			ProtocolTypeWrapper protocol_wrapper) throws GeneratorException {
		try {
			File f = new File(getTests_directory(), "TestPackagesFactory.cpp");
			if (!useOverwrittingFile(f)) {
				CodeCPPPrintStream out = new CodeCPPPrintStream(f);
				out.printCode("#include <stdio.h>");
				out.printCode("#include <stdlib.h>");

				out.printCode("#include \"TestPackagesFactory.h\"");

				for (PacketGroupType pgt : protocol_wrapper.getProtocol()
						.getPacketGroups().getPacketGroup()) {
					PackageGroupTypeWrapper pgtw=new PackageGroupTypeWrapper(pgt,protocol_wrapper);
					for (PacketType p : pgt.getPackets().getPacket()) {
						PackageTypeWrapper etw = new PackageTypeWrapper(p,pgtw);
						if(pgtw.isMainGroup())
							out.printCode("#include \"../packages/"	+ etw.getClassName() + ".h\"");
						else
							out.printCode("#include \"../packages_"+pgtw.getGroupName()+"/"	+ etw.getClassName() + ".h\"");
					}
				}
				out.blankLine();

				out.printCode("using namespace protocol;");
				out
						.printCode("Package* TestPackagesFactory::createPackage(int nr){");
				out.indent();
				out.printCode("switch(nr){");

				int i = 0;
				for (PackageTypeWrapper pgtw : protocol_wrapper.getMainGroup()
						.getPackageWrappers()) {
					List<FilledPackage> lfp = UniversalValuesSource
							.getInstance().getExampleFilledPackages(pgtw);
					for (FilledPackage fp : lfp) {
						out.printCode("  case " + i + ": return "
								+ createAndInitPackageCode(fp) + ";");
						i++;
					}
				}
				out.printCode("  default: return NULL;");

				out.printCode("}");
				out.deindent();
				out.printCode("}");
				out.close();
			}
		} catch (FileNotFoundException e1) {
			throw new GeneratorException(e1);
		}
	}

	private String formatValue(Object v, FieldType f) {
		if (v == null)
			return "NULL";

		switch (f.getType()) {
		case SINT_8:
		case SINT_16:
		case SINT_32:
			return v.toString();
		case SINT_64:
			return v.toString()+"ll";
		case UINT_8:
		case UINT_16:
		case UINT_32:
			return v.toString();
		case UINT_64:
			return v.toString()+"llu";
		case VARUINT:
			return v.toString()+"llu";
		case BOOL:
		case DOUBLE:
			return v.toString();
		case SSTRING:
		case BYTES:
		case STRING:
			return "new CharArray(\"" + v.toString() + "\")";
		case ENUM:
			return v.toString();
		case ENUM_MAP:
			return enum_map(v, f.getObjectRef());
		case PACKAGE:
		case PACKAGE_MAP:
			return createAndInitPackageCode((FilledPackage) v);
		default:
			return "Unknown type:" + f.getType();
		}
	}

	private String enum_map(Object v, String enumName) {
		return (enumName + "Map").toUpperCase() + "_" + v;
	}

}
