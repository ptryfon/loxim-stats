package pl.edu.mimuw.loxim.protogen.lang.java;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ParseErrorException;
import org.apache.velocity.exception.ResourceNotFoundException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.lang.java.types.JavaTypeHelper;

public class EnumsGenerator {

	private OverwriteHelper overwriteHelper;

	public EnumsGenerator(File overwrite_dir) {
		overwriteHelper = new OverwriteHelper(overwrite_dir);
	}

	public void generate(File working_directory, EnumTypeWrapper e) throws GeneratorException {
		File f = new File(getEnumDirectory(working_directory), e.getClassName() + ".java");
		if (!overwriteHelper.useOverwrittingFile(f)) {
			try {
				FileWriter fw = new FileWriter(f);
				Template t = Velocity.getTemplate("/templates/enum.java.vm");
				VelocityContext vc = new VelocityContext(GlobalVelocityContextFactory.getGlobalVelocityContext(e.getProtocol()));
				vc.put("enum", e);
				vc.put("id_type", new JavaTypeHelper(e.getProtocol()).getTypeName(e.getEnum_type().getAsType()));
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
	}

	private File getEnumDirectory(File packages_directory) {
		File f = new File(packages_directory, "enums");
		f.mkdirs();
		return f;
	}

}
