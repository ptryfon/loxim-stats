package pl.edu.mimuw.loxim.protocol.packages;

import java.math.BigInteger;
import java.util.EnumSet;
import pl.edu.mimuw.loxim.protogen.lang.java.template.ptools.Package;
import pl.edu.mimuw.loxim.protogen.lang.java.template.exception.ProtocolException;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageInputStreamReader;
import pl.edu.mimuw.loxim.protogen.lang.java.template.streams.PackageOutputStreamWriter;

import pl.edu.mimuw.loxim.protocol.enums.*;

public class Q_c_executePackage extends Package {
	public final static short ID = (short) 66;

	/**
	 **/
	private java.math.BigInteger statement_id=BigInteger.ZERO;
	/**
	 **/
	private EnumSet<Statement_flagsEnum> flags=EnumSet.noneOf(Statement_flagsEnum.class);
	/**
	 **/
	private long paramsCnt=0;
	/**
	 **/
	private Long[] params=new Long[0];

	public Q_c_executePackage() {
	}

	public Q_c_executePackage(java.math.BigInteger a_statement_id,
			EnumSet<Statement_flagsEnum> a_flags, long a_paramsCnt,
			Long[] a_params) {
		statement_id = a_statement_id;
		flags = a_flags;
		paramsCnt = a_paramsCnt;
		params = a_params;

	}

	@Override
	public long getPackageType() {
		return (long) ID;
	}

	@Override
	protected void deserializeW(PackageInputStreamReader reader)
			throws ProtocolException {
		super.deserializeW(reader);

		try {
			statement_id = reader.readUint64();
			java.math.BigInteger tmp_flags = reader.readUint64();
			flags = Statement_flagsEnum.createEnumMapByValue(tmp_flags);
			paramsCnt = reader.readUint32();
			params = new Long[(int)paramsCnt];
			for (int i = 0; i < paramsCnt; i++)
				params[i] = reader.readVaruint();
		} catch (Exception e) {
			throw new ProtocolException(e);
		}
	}

	@Override
	protected void serializeW(PackageOutputStreamWriter writer)
			throws ProtocolException {
		super.serializeW(writer);
		try {
			writer.writeUint64(statement_id);
			writer.writeUint64(Statement_flagsEnum.toEnumMapValue(flags));
			writer.writeUint32(paramsCnt);
			if(params!=null)
				for (int i = 0; i < paramsCnt; i++)
					writer.writeVaruint(params[i]);
		} catch (Exception e) {
			throw new ProtocolException(e);
		}
	}

	@Override
	public boolean equals(Object obj) {
		if (!super.equals(obj))
			return false;
		Q_c_executePackage p = (Q_c_executePackage) obj;
		if (!(((statement_id == p.getStatement_id()) || ((statement_id != null)
				&& (p.getStatement_id() != null) && ((statement_id).equals(p
				.getStatement_id()))))))
			return false;
		if (!(((flags == p.getFlags()) || ((flags != null)
				&& (p.getFlags() != null) && ((flags).equals(p.getFlags()))))))
			return false;
		if (!((paramsCnt == p.getParamsCnt())))
			return false;
		
		if (params == p.getParams())
			return true;
		if (params==null || p.getParams()==null)
			return false;
		if (params.length!=p.getParams().length)
			return false;
		for (int i=0; i<paramsCnt; i++)
		{
			if ((p.getParams()[i]==params[i]))
				continue;
			if ((p.getParams()[i]==null)||(params[i]==null))
				return false;
			if (!(p.getParams()[i].equals(params[i])))
				return false;
		}
	
		return true;
	}

	// ================= GETTERS AND SETTERS ===================

	public java.math.BigInteger getStatement_id() {
		return statement_id;
	};

	public void setStatement_id(java.math.BigInteger a_statement_id) {
		a_statement_id = statement_id;
	};

	public EnumSet<Statement_flagsEnum> getFlags() {
		return flags;
	};

	public void setFlags(EnumSet<Statement_flagsEnum> a_flags) {
		a_flags = flags;
	};

	public long getParamsCnt() {
		return paramsCnt;
	};

	public void setParamsCnt(long a_paramsCnt) {
		a_paramsCnt = paramsCnt;
	};

	public Long[] getParams() {
		return params;
	};

	public void setParams(Long[] a_params) {
		a_params = params;
	};

}
