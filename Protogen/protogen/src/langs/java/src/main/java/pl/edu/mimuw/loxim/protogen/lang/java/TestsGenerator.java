package pl.edu.mimuw.loxim.protogen.lang.java;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ParseErrorException;
import org.apache.velocity.exception.ResourceNotFoundException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketGroupType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.values.FilledPackage;
import pl.edu.mimuw.loxim.protogen.api.values.UniversalValuesSource;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;
import pl.edu.mimuw.loxim.protogen.lang.java.types.JavaTypeHelper;

public class TestsGenerator {

	private OverwriteHelper overwriteHelper;

	public TestsGenerator(File overwrite_dir) {
		overwriteHelper = new OverwriteHelper(overwrite_dir);
	}

	public void generate(File working_test_directory, ProtocolTypeWrapper ptw) throws GeneratorException {
		File f = new File(working_test_directory, "tests");
		f.mkdirs();
		generateTestPackagesFactory(f, ptw);
		copyStaticFiles(f, ptw);

	}

	private void copyStaticFiles(File file, ProtocolTypeWrapper ptw) throws GeneratorException {
		try {
			File f = new File(file, "TestPackages.java");
			VelocityContext vc = new VelocityContext(GlobalVelocityContextFactory.getGlobalVelocityContext(ptw));
			if (!overwriteHelper.useOverwrittingFile(f)) {
				FileWriter fw = new FileWriter(f);
				Template t = Velocity.getTemplate("/templates/TestPackages.java.vm");
				t.merge(vc, fw);
				fw.close();
			}

			f = new File(file, "TestRunnerRec.java");
			if (!overwriteHelper.useOverwrittingFile(f)) {
				FileWriter fw = new FileWriter(f);
				Template t = Velocity.getTemplate("/templates/TestRunnerRec.java.vm");
				t.merge(vc, fw);
				fw.close();
			}

			f = new File(file, "TestRunnerSender.java");
			if (!overwriteHelper.useOverwrittingFile(f)) {
				FileWriter fw = new FileWriter(f);
				Template t = Velocity.getTemplate("/templates/TestRunnerSender.java.vm");
				t.merge(vc, fw);
				fw.close();
			}

		} catch (IOException e1) {
			throw new GeneratorException(e1);
		} catch (ResourceNotFoundException e1) {
			throw new GeneratorException(e1);
		} catch (ParseErrorException e1) {
			throw new GeneratorException(e1);
		} catch (Exception e1) {
			throw new GeneratorException(e1);
		}

	}

	private void generateTestPackagesFactory(File file, ProtocolTypeWrapper ptw) throws GeneratorException {
		try {
			FileWriter fw = new FileWriter(new File(file, "TestPackagesFactory.java"));
			Template t = Velocity.getTemplate("/templates/testPackagesFactory.java.vm");
			VelocityContext vc = new VelocityContext(GlobalVelocityContextFactory.getGlobalVelocityContext(ptw));

			/* Java nie pozwala tworzyæ metod d³u¿szych ni¿ 65Kb" */
			vc.put("constructors", devide_into(generatePackagesConstructors(ptw), 100));

			vc.put("includes_suffixes", generatePackagesIncludesSuffixes(ptw));
			t.merge(vc, fw);
			fw.close();
		} catch (IOException e1) {
			throw new GeneratorException(e1);
		} catch (ResourceNotFoundException e1) {
			throw new GeneratorException(e1);
		} catch (ParseErrorException e1) {
			throw new GeneratorException(e1);
		} catch (Exception e1) {
			throw new GeneratorException(e1);
		}
	}

	private List<String> generatePackagesConstructors(ProtocolTypeWrapper protocol_wrapper) throws GeneratorException {
		List<String> res = new LinkedList<String>();

		for (PackageTypeWrapper pgtw : protocol_wrapper.getMainGroup().getPackageWrappers()) {
			List<FilledPackage> lfp = UniversalValuesSource.getInstance().getExampleFilledPackages(pgtw);
			for (FilledPackage fp : lfp) {
				res.add(createAndInitPackageCode(fp));
			}
		}
		return res;
	}

	private Map<Integer, List<String>> devide_into(List<String> src, Integer items_in_grp) {
		Map<Integer, List<String>> res = new LinkedHashMap<Integer, List<String>>(src.size() / 100);
		Iterator<String> item = src.iterator();
		int j = 0;
		while (item.hasNext()) {
			List<String> newSublist = new LinkedList<String>();
			int i = 0;
			while (item.hasNext() && (i < items_in_grp)) {
				newSublist.add(item.next());
				i++;
			}
			res.put(j, newSublist);
			j++;
		}
		return res;
	}

	private List<String> generatePackagesIncludesSuffixes(ProtocolTypeWrapper protocol_wrapper) throws GeneratorException {
		List<String> res = new LinkedList<String>();

		for (PacketGroupType pgt : protocol_wrapper.getProtocol().getPacketGroups().getPacketGroup()) {
			PackageGroupTypeWrapper pgtw = new PackageGroupTypeWrapper(pgt, protocol_wrapper);

			if (pgtw.isMainGroup())
				res.add("packages.*");
			else
				res.add("packages_" + pgtw.getGroupName() + ".*");
		}

		return res;
	}

	private String createAndInitPackageCode(FilledPackage fp) throws GeneratorException {
		PackageTypeWrapper ptw = fp.getPackageWrapper();
		StringBuffer res = new StringBuffer();
		res.append("new " + ptw.getClassName() + "(");

		Iterator<FieldType> iterator = ptw.getPackage().getField().iterator();
		while (iterator.hasNext()) {
			FieldType f = iterator.next();
			Object v = fp.getValue(f.getName());
			res.append(formatValue(v, f, ptw.getProtocol()));
			if (iterator.hasNext())
				res.append(",");

		}
		res.append(")");
		return res.toString();
	}

	private String formatValue(Object v, FieldType f, ProtocolTypeWrapper ptw) throws GeneratorException {
		if (v == null)
			return "null";

		switch (f.getType()) {
		case SINT_8:
		case SINT_16:
		case SINT_32:
		case UINT_8:
		case UINT_16:
		case SSTRING:
		case BOOL:
		case DOUBLE:
		case STRING:
		case BYTES:
		case UINT_64:
			return new JavaTypeHelper(ptw).getTypeValue(f.getType(), f.getObjectRef(), f.getObjectRefId(), v.toString());
		case SINT_64:
		case UINT_32:
			return new JavaTypeHelper(ptw).getTypeValue(f.getType(), f.getObjectRef(), f.getObjectRefId(), v.toString()) + "l";
		case VARUINT:
			return new JavaTypeHelper(ptw).getTypeValue(Type.SINT_64, f.getObjectRef(), f.getObjectRefId(), v.toString()) + "l";

		case ENUM:
			return ptw.findEnumWrapper(f.getObjectRef()).getClassName() + "." + v.toString();
		case ENUM_MAP:
			return enum_map(v, f.getObjectRef(), ptw);
		case PACKAGE:
		case PACKAGE_MAP:
			return createAndInitPackageCode((FilledPackage) v);
		default:
			return "UNSUPPORTED: " + f.getType();
			// return "Unknown type:" + f.getType();
		}
	}

	private String enum_map(Object v, String objectRef, ProtocolTypeWrapper ptw) {
		String className = ptw.findEnumWrapper(objectRef).getClassName();
		if (v.equals(EnumTypeWrapper.ALL_CONSTANT)) {
			return "EnumSet.allOf(" + className + ".class)";
		}
		if (v.equals(EnumTypeWrapper.NONE_CONSTANT)) {
			return "EnumSet.noneOf(" + className + ".class)";
		}
		if (v.equals(EnumTypeWrapper.ODD_CONSTANT)) {
			return className + ".getODD()";
		}
		return "NOT SUPPORTED";
	}

}
