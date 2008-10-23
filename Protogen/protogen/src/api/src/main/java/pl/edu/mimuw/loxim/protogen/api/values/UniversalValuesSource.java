package pl.edu.mimuw.loxim.protogen.api.values;

import java.math.BigInteger;
import java.util.LinkedList;
import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;

public class UniversalValuesSource {
	private static final String LONG_TEXT = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";
	private static final String MIN_SINT_64 = (new BigInteger("-2147483648")).multiply((new BigInteger("4294967296"))).add(new BigInteger("1")).toString();
	private static final String MAX_SINT_64 = (new BigInteger("2147483648")).multiply((new BigInteger("4294967296"))).subtract(new BigInteger("1")).toString();
	private static final String MAX_UINT_64 = (new BigInteger("4294967296")).multiply((new BigInteger("4294967296"))).subtract(new BigInteger("1")).toString();

	private static final String MAX_VARUINT = MAX_SINT_64;

	private static UniversalValuesSource instance;

	public synchronized static UniversalValuesSource getInstance() {
		if (instance == null)
			instance = new UniversalValuesSource();
		return instance;
	}

	public List<String> getSStringValues() {
		List<String> res = new LinkedList<String>();
		res.add(null);
		res.add(LONG_TEXT.substring(0, 249));
		return res;
	}

	public List<String> getStringValues() {
		List<String> res = getSStringValues();
		res.add(LONG_TEXT);
		return res;
	}

	public List<?> getValuesForType(Type typeValue, boolean nullable) {
		List<String> res = new LinkedList<String>();

		switch (typeValue) {
		case UINT_8:
			res.add("0");
			res.add("73");
			res.add("255");
			break;
		case UINT_16:
			res.add("0");
			res.add("1073");
			res.add(Long.toString(256 * 256 - 1));
			break;
		case UINT_32:
			res.add("0");
			res.add("123456");
			res.add(Long.toString(256l * 256l * 256l * 256l - 1));
			break;
		case UINT_64:
			res.add("0");
			res.add("732343556787931");
			res.add(MAX_UINT_64);
			break;
		case SINT_8:
			res.add("-128");
			res.add("0");
			res.add("73");
			res.add("127");
			break;
		case SINT_16:
			res.add(Long.toString(-128 * 256));
			res.add("0");
			res.add("-7300");
			res.add(Long.toString(128 * 256 - 1));
			break;
		case SINT_32:
			res.add(Long.toString(-128l * 256l * 256l * 256l));
			res.add("0");
			res.add("-7300123");
			res.add(Long.toString(128l * 256l * 256l * 256l - 1));
			break;
		case SINT_64:
			res.add(MIN_SINT_64);
			res.add("0");
			res.add("-7300");
			res.add(MAX_SINT_64);
			break;
		case VARUINT:
			if (nullable)
				res.add(null);
			res.add("0");
			res.add("249");
			res.add("250");
			res.add("678");
			res.add("70800");
			res.add("70800001234");
			res.add(MAX_VARUINT);
			break;
		case BOOL:
			res.add("true");
			res.add("false");
			break;
		case DOUBLE:
			res.add("0.0");
			res.add("-1234.56789");
			res.add("98765.4321");
			break;
		case SSTRING:
			return getSStringValues();
		case STRING:
			return getStringValues();
		case BYTES:
			return getSStringValues();
		}
		return res;
	}

	public List<?> getValuesForType(PackageTypeWrapper _package, String fieldName) throws GeneratorException {
		FieldType ft = _package.getFieldByName(fieldName);
		switch (ft.getType()) {
		case UINT_8:
		case UINT_16:
		case UINT_32:
		case UINT_64:
		case SINT_8:
		case SINT_16:
		case SINT_32:
		case SINT_64:
		case STRING:
		case BYTES:
		case SSTRING:
		case BOOL:
		case VARUINT:
		case DOUBLE:
			return getValuesForType(ft.getType(), ft.isNullable());
		case ENUM:
			return _package.getProtocol().findEnumWrapper(ft.getObjectRef()).getExampleValues();
		case ENUM_MAP:
			return _package.getProtocol().findEnumWrapper(ft.getObjectRef()).getEnumMapValuesList();
		case PACKAGE: {
			PackageTypeWrapper p = _package.getProtocol().findPackageWrapper(ft.getObjectRef(), ft.getObjectRefId());
			return getExampleFilledPackages(p);
		}
		case PACKAGE_MAP: {
			PackageGroupTypeWrapper p = _package.getProtocol().findGroupByName(ft.getObjectRef());
			List<PackageTypeWrapper> examplePackages = p.getExamplePackages();
			List<Object> res = new LinkedList<Object>();
			for (PackageTypeWrapper ptw : examplePackages) {
				res.addAll(getExampleFilledPackages(ptw));
			}
			return res;
		}
		default:
			throw new IllegalStateException("Unknown type: " + ft.getType());
		}
	}

	public List<FilledPackage> getExampleFilledPackages(PackageTypeWrapper p) throws GeneratorException {
		List<FilledPackage> res = new LinkedList<FilledPackage>();
		res.add(new FilledPackage(p));
		for (FieldType ft : p.getPackage().getField()) {
			System.out.println("Generating values for package: " + p.getClassName() + " fields: " + ft.getName());

			res = FilledPackage.generateVariants(res, p, ft.getName());
			// Ograniczamy pokolenie do nie wiêcej ni¿ 100 elementów poprzez usuniêcie 75% elementów
			if (res.size() > 50) {
				int i = 0;
				while (res.size() > i + 5) {
					res.remove(i);
					res.remove(i);
					res.remove(i);
					i++;
				}
			}
		}
		for (FilledPackage r : res) {
			r.correctConsistency();
		}
		return res;
	}
}
