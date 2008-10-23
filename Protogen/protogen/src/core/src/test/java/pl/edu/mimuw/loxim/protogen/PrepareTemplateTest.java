package pl.edu.mimuw.loxim.protogen;

import java.io.File;
import java.net.URISyntaxException;

import org.junit.Test;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;


public class PrepareTemplateTest {
	
	@Test
	public void testUnzipCpp() throws GeneratorException, URISyntaxException
	{
		PrepareTemplate p=new PrepareTemplate();
		p.unzipTempate(this.getClass().getResourceAsStream("/template-cpp.zip"), new File("./target/surefire/template"));
	}
	
	@Test
	public void testUnzipJava() throws GeneratorException, URISyntaxException
	{
//		PrepareTemplate p=new PrepareTemplate();
//		p.unzipTempate(new File(this.getClass().getResource("/template-java.zip").toURI()), new File("./target/surefire/template"));
	}

	
}
