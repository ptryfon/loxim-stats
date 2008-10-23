package pl.edu.mimuw.loxim.protogen.api;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Map;

public class StreamsHelpers {

	static public void writeWholeInputStream(OutputStream os,
			InputStream is) throws IOException {
		int res=0;
		byte[] buffor=new byte[40960];
		while((res=is.read(buffor))>=0)
		{
			os.write(buffor,0,res);
		}
	}
	
	static public void storeInputStreamToFile(InputStream is, File tmpFile) throws IOException {
		FileOutputStream fos=new FileOutputStream(tmpFile);
		StreamsHelpers.writeWholeInputStream(fos, is);
		fos.close();
	}

	public static void writeWholeInputStream(OutputStream out,
			InputStream resourceAsStream, Map<String, String> translations) throws IOException {
		ByteArrayOutputStream baos=new ByteArrayOutputStream();
		writeWholeInputStream(baos, resourceAsStream);
		String s=new String(baos.toByteArray(),"iso8859-2");
		for(Map.Entry<String, String> e:translations.entrySet())
		{
			s=s.replace(e.getKey(), e.getValue());
		}
		writeWholeInputStream(out, new ByteArrayInputStream(s.getBytes("iso8859-2")));
	}


}
