package pl.edu.mimuw.loxim.protogen;

import java.io.File;
import java.io.InputStream;
import java.net.URISyntaxException;

import javax.xml.bind.JAXBException;

import org.codehaus.plexus.DefaultPlexusContainer;
import org.codehaus.plexus.PlexusContainer;
import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.LanguageGenerator;

public class ProtoGen {
	public static void main(String[] args) throws PlexusContainerException,
			ComponentLookupException, GeneratorException, JAXBException, URISyntaxException {
		PlexusContainer pc = new DefaultPlexusContainer();
		if ((args.length == 3)||(args.length == 4)) {
			File xml = new File(args[0]);
			File dst = new File(args[1]);
			String lang = args[2];
			File over = (args.length==4) ? new File(args[3]):null;

			System.out.println("Source xml      = " + xml.getAbsolutePath());
			System.out.println("Destination dir = " + dst.getAbsolutePath());
			System.out.println("Target language = " + lang);
			System.out.println("Overwritten dir = " + (over!=null?over.getAbsolutePath():"[null]"));
			System.out.println("==============================================");
			dst.mkdirs();			
			
			InputStream is=ProtoGen.class.getResourceAsStream("/template-"+lang+".zip");
			if(is!=null)
			{
				System.out.print("+Unziping template.....");
				PrepareTemplate template=new PrepareTemplate();
				template.unzipTempate(is, dst);
				System.out.println("... [done]");
			}
			
			System.out.println("+Generating packages...");
			LanguageGenerator lg = (LanguageGenerator) pc.lookup(
					LanguageGenerator.ROLE, lang);
			SourceXmlParser parse = new SourceXmlParser();
			lg.generate(parse.parse(xml), dst,over);
			System.out.println("                       ... [done]");
		}else{
			System.err.println("Expected 3 parameters: source_xml target_directory lang");
		}
	}
}
