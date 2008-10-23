package pl.edu.mimuw.loxim.protogen.api;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.StringTokenizer;


public class CodePrintStream extends PrintStream {
	private int level=0;

	public CodePrintStream(File file, String csn) throws FileNotFoundException,
			UnsupportedEncodingException {
		super(file, csn);
	}

	public CodePrintStream(File file) throws FileNotFoundException {
		super(file);
	}

	public CodePrintStream(OutputStream out, boolean autoFlush, String encoding)
			throws UnsupportedEncodingException {
		super(out, autoFlush, encoding);
	}

	public CodePrintStream(OutputStream out, boolean autoFlush) {
		super(out, autoFlush);
	}

	public CodePrintStream(OutputStream out) {
		super(out);
	}

	public CodePrintStream(String fileName, String csn)
			throws FileNotFoundException, UnsupportedEncodingException {
		super(fileName, csn);
	}

	public CodePrintStream(String fileName) throws FileNotFoundException {
		super(fileName);
	}
	
	public void indent()
	{
		level++;
	}
	
	public void deindent()
	{
		level--;
	}
	
	public void printLines(String lines,String prefix)
	{
		StringTokenizer tokenizer=new StringTokenizer(lines,"\n",false);
		while(tokenizer.hasMoreTokens())
		{
			print(prefix);
			print(tokenizer.nextToken());
			print("\n");
		}
	}
	
	public void blankLine()
	{
		print("\n");
	}
	
	public void printCode(CharSequence code)
	{
		printLines(code.toString(), genIndent());
	}
	
	protected String genIndent()
	{
		StringBuffer res=new StringBuffer();
		for(int i=0; i<level; i++)
			res.append("\t");
		return res.toString();
	}
	
}
