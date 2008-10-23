package pl.edu.mimuw.loxim.protogen.lang.cpp;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;

import org.codehaus.plexus.PlexusTestCase;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;
import org.junit.Test;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.LanguageGenerator;
import pl.edu.mimuw.loxim.protogen.api.jaxb.ProtocolType;


public class CppPartialLanguageGeneratorTest extends PlexusTestCase{
	
	@SuppressWarnings("unchecked")
	@Test
	public void testRunGenerator() throws JAXBException, ComponentLookupException, GeneratorException
	{
	    JAXBContext jaxbContext=JAXBContext.newInstance(ProtocolType.class.getPackage().getName());
	    Unmarshaller u=jaxbContext.createUnmarshaller();
	    JAXBElement<ProtocolType> el=(JAXBElement<ProtocolType>) u.unmarshal(getClass().getResourceAsStream("/loxim2.xml"));
	    ProtocolType pt=el.getValue();
	    
	    LanguageGenerator lg=(LanguageGenerator) getContainer().lookup(LanguageGenerator.ROLE,"cpp");
	    lg.generate(pt,new java.io.File("./target/surefire/result"),null);
	}
}
