package pl.edu.mimuw.loxim.protogen;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import org.codehaus.plexus.DefaultPlexusContainer;
import org.codehaus.plexus.archiver.UnArchiver;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.StreamsHelpers;

public class PrepareTemplate {

	public void unzipTempate(InputStream is, File dst)
			throws GeneratorException {
		File tmpFile;
		try {
			tmpFile = tmpFile();
		} catch (IOException e) {
			throw new GeneratorException(e);
		}
		try {
			StreamsHelpers.storeInputStreamToFile(is, tmpFile);
			unzipTempate(tmpFile, dst);
		} catch (IOException e) {
			throw new GeneratorException(e);
		} finally {
			tmpFile.delete();
		}
	}

	private File tmpFile() throws IOException {
		return File.createTempFile("protogen", null);
	}


	public void unzipTempate(File src, File dst) throws GeneratorException {
		try {
			UnArchiver a = (UnArchiver) new DefaultPlexusContainer().lookup(
					UnArchiver.ROLE, "zip");
			a.setSourceFile(src);
			dst.mkdirs();
			a.setDestDirectory(dst);
			a.setOverwrite(true);
			a.extract();
		} catch (Exception e) {
			throw new GeneratorException(e);
		}
	}

}
