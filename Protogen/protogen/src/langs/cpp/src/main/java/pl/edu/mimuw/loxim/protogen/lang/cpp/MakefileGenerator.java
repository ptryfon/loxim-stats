package pl.edu.mimuw.loxim.protogen.lang.cpp;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.StreamsHelpers;

public class MakefileGenerator {
	
	public void generate(File f,Integer depth,String libname,List<String> headers,List<String> sources,List<String> subdirectiories) throws GeneratorException
	{
		try {
			CodeCPPPrintStream os=new CodeCPPPrintStream(f);
			StreamsHelpers.writeWholeInputStream(os,this.getClass().getResourceAsStream("Makefile-prefix"));
//			include ../../make.defs
//			SOURCES =       *.cpp
//			HEADERS =       *.h
//			STATIC_LIB =    layers.a

			os.printCode("include ./"+times(depth,"../")+"make.defs");
			os.printCode("SOURCES = \\");
			writeFileList(os,sources);
			os.blankLine();
			
			os.printCode("HEADERS = \\");
			writeFileList(os,headers);
			os.blankLine();
			
			os.printCode("SUBDIRS = \\");
			writeFileList(os,subdirectiories);
			os.blankLine();
			
			os.printCode("STATIC_LIB = "+libname+".a");		
			StreamsHelpers.writeWholeInputStream(os,this.getClass().getResourceAsStream("Makefile-suffix"));
			os.close();
		} catch (FileNotFoundException e) {
			throw new GeneratorException(e);
		} catch (IOException e) {
			throw new GeneratorException(e);
		}
	}
	
	private String times(Integer depth, String string) {
		StringBuffer res=new StringBuffer();
		for(int i=0; i<depth; i++)
			res.append(string);
		return res.toString();
	}

	private void writeFileList(CodeCPPPrintStream os,List<String> sources) {
		os.indent();
		Iterator<String> s=sources.iterator();
		while(s.hasNext())
		{
			String item=s.next();
			os.printCode(item+(s.hasNext()?" \\":""));
		}
		os.deindent();
	}

	public void generate(File f,Integer depth,String libname,List<String> names,List<String> subdirectiories)throws GeneratorException
	{
		List<String> sources=new LinkedList<String>();
		List<String> headers=new LinkedList<String>();
		for(String name:names)
		{
			sources.add(name+".cpp");
			headers.add(name+".h");
		}
		generate(f,depth,libname, headers,sources,subdirectiories);
	}

}
