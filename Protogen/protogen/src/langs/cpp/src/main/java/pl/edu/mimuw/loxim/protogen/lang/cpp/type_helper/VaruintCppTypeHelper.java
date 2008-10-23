package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class VaruintCppTypeHelper extends NumericCppTypeHelper {
	private boolean nullable;

	@Override
	public void fieldInitialize(FieldType field, ProtocolTypeWrapper protocol) {
		super.fieldInitialize(field, protocol);
		nullable = field.isNullable();
	}

	@Override
	public String getSetter(String name) {
		return super.getSetter(name)
				+ ((nullable) ? ("\n" + new BoolCppTypeHelper()
						.getSetter("bnull_" + name)) : "");
	}

	@Override
	public String getGetter(String name) {
		return super.getGetter(name)
				+ ((nullable) ? ("\n" + new BoolCppTypeHelper()
						.getGetter("bnull_" + name)) : "");
	}

	@Override
	public String getInitCode(String name) {
		return super.getInitCode(name)
				+ ((nullable) ? ("\n" + "bnull_" + name + "=false;") : "");
	}

	@Override
	public String getTypeDeclaration() {
		return "int64_t";
	}

	@Override
	public String getTypeName() {
		return "int64";
	}

	public String readAssignCode(String name) {
		if (nullable) {
			return name + "=" + "reader->readVaruint(bnull_" + name + ");";
		} else {
			return "{bool b; " + name + "=" + "reader->readVaruint(b);}";
		}
	}

	public String readCode() {
		return "NOT SUPPORTED";// reader->readVaruint(null);";
	}

	public String writeCode(String value) {
		if (nullable) {
			return "if (bnull_"
					+ value
					+ ") writer->writeVaruintNull(); else writer->writeVaruint("
					+ value + ");";
		} else {
			return "writer->writeVaruint(" + value + ");";
		}
	}

	public String equalsCode(String name) {
		if (!nullable) {
			return super.equalsCode(name);
		} else {
			String GETTER="get" + CodeHelper.upperFirstChar(name)+"()";
			String BGETTER="get" + CodeHelper.upperFirstChar("bnull_"+name)+"()";
			String base="if ("+BGETTER+"!=p->"+BGETTER+") return false;\n";
			base += "if ((!bnull_"+ name+ ")&&(!"+ compareCode(GETTER, "p->"+GETTER)+")) return false;";
			return base;
		}
	}

}
