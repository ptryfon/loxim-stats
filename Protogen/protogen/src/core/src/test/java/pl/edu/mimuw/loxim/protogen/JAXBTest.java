package pl.edu.mimuw.loxim.protogen;


import java.io.ByteArrayOutputStream;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.namespace.QName;

import junit.framework.Assert;

import org.junit.Test;

import pl.edu.mimuw.loxim.protogen.api.jaxb.ProtocolType;

public class JAXBTest 
{

   @SuppressWarnings("unchecked")
   @Test
    public void testUnmarshal() throws JAXBException
    {
        JAXBContext jaxbContext=JAXBContext.newInstance(ProtocolType.class.getPackage().getName());
        Unmarshaller u=jaxbContext.createUnmarshaller();
        JAXBElement<ProtocolType> el=(JAXBElement<ProtocolType>) u.unmarshal(getClass().getResourceAsStream("/loxim2.xml"));
        ProtocolType pt=el.getValue();
        
        Assert.assertEquals(pt.getMetadata().getMajorVersion().intValue(),2);
    }
   
   @SuppressWarnings("unchecked")
@Test
   public void testMarshal() throws JAXBException
   {
       JAXBContext jaxbContext=JAXBContext.newInstance(ProtocolType.class.getPackage().getName());
       Unmarshaller u=jaxbContext.createUnmarshaller();
       JAXBElement<ProtocolType> el=(JAXBElement<ProtocolType>) u.unmarshal(getClass().getResourceAsStream("/loxim2.xml"));
       ProtocolType pt=el.getValue();
       
       Marshaller m=jaxbContext.createMarshaller();
       m.marshal(new JAXBElement<ProtocolType>(new QName("http://loxim.mimuw.edu.pl/protoGen", "protocol"), ProtocolType.class, null, pt), new ByteArrayOutputStream());
   }
}
