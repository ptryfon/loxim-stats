package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

import pl.edu.mimuw.loxim.protogen.api.CodePrintStream;

public class CodeCPPPrintStream extends CodePrintStream{

	public CodeCPPPrintStream(File file, String csn)
			throws FileNotFoundException, UnsupportedEncodingException {
		super(file, csn);
	}

	public CodeCPPPrintStream(File file) throws FileNotFoundException {
		super(file);
	}

	public CodeCPPPrintStream(OutputStream out, boolean autoFlush,
			String encoding) throws UnsupportedEncodingException {
		super(out, autoFlush, encoding);
	}

	public CodeCPPPrintStream(OutputStream out, boolean autoFlush) {
		super(out, autoFlush);
	}

	public CodeCPPPrintStream(OutputStream out) {
		super(out);
	}

	public CodeCPPPrintStream(String fileName, String csn)
			throws FileNotFoundException, UnsupportedEncodingException {
		super(fileName, csn);
	}

	public CodeCPPPrintStream(String fileName) throws FileNotFoundException {
		super(fileName);
	}

}
