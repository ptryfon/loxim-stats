package pl.edu.mimuw.loxim.protogen.lang.java;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.StreamsHelpers;

public class OverwriteHelper {
	
	public File overwrite_src_directory;
	
	public File getOverwrite_src_directory() {
		return overwrite_src_directory;
	}

	public void setOverwrite_src_directory(File overwrite_src_directory) {
		this.overwrite_src_directory = overwrite_src_directory;
	}
	
	public OverwriteHelper(File f) {
		overwrite_src_directory=f;
	}
	
	protected boolean useOverwrittingFile(File f) throws GeneratorException {
		File src_file = new File(overwrite_src_directory, f.getName());
		if (src_file.exists()) {
			try {
				System.out.println(" - using overwritten file: "
						+ src_file.getAbsolutePath());
				FileOutputStream fos;

				fos = new FileOutputStream(f);
				FileInputStream fis = new FileInputStream(src_file);
				StreamsHelpers.writeWholeInputStream(fos, fis);
				fos.close();
				fis.close();
				return true;
			} catch (FileNotFoundException e) {
				throw new GeneratorException(e);
			} catch (IOException e) {
				throw new GeneratorException(e);
			}

		} else {
			return false;
		}
	}
}
