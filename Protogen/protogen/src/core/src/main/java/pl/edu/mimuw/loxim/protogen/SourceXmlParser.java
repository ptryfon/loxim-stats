package pl.edu.mimuw.loxim.protogen;

import java.io.File;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;

import pl.edu.mimuw.loxim.protogen.api.jaxb.ProtocolType;

public class SourceXmlParser {

	@SuppressWarnings("unchecked")
	public ProtocolType parse(File xml) throws JAXBException {
        JAXBContext jaxbContext=JAXBContext.newInstance(ProtocolType.class.getPackage().getName());
        Unmarshaller u=jaxbContext.createUnmarshaller();
        JAXBElement<ProtocolType> el=(JAXBElement<ProtocolType>) u.unmarshal(xml);
        return el.getValue();
	}

}
