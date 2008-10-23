package pl.edu.mimuw.loxim.protogen;

import org.codehaus.plexus.PlexusTestCase;
import org.junit.Test;

import pl.edu.mimuw.loxim.protogen.api.BasicLanguageGenerator;
import pl.edu.mimuw.loxim.protogen.api.LanguageGenerator;

//@Ignore
public class PlexusTest extends PlexusTestCase{

	@Test
	public void testJavaContainer() throws Exception {
		LanguageGenerator lg=(LanguageGenerator) getContainer().lookup(LanguageGenerator.ROLE,"java");
		checkLanguageGenerator(lg);
		
	}
	
	private void checkLanguageGenerator(LanguageGenerator lg) {
		assertNotNull(lg);
		assertNotNull(((BasicLanguageGenerator)lg).getPartialLanguageGenerator());	
	}

	@Test
	public void testCppContainer() throws Exception {
		LanguageGenerator lg=(LanguageGenerator) getContainer().lookup(LanguageGenerator.ROLE,"cpp");
		checkLanguageGenerator(lg);
	}
}
