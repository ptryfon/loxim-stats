package pl.edu.mimuw.loxim.protogen.api;
import java.io.File;

import pl.edu.mimuw.loxim.protogen.api.jaxb.ProtocolType;

public interface LanguageGenerator {
	/* Plexus role identifier */
	static String ROLE = LanguageGenerator.class.getName();

	public void generate(ProtocolType descriptor,File target_directory,File overwrite_dir) throws GeneratorException;
}
