package pl.edu.mimuw.loxim.protogen.lang.java;

import java.io.File;
import java.io.FileWriter;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ParseErrorException;
import org.apache.velocity.exception.ResourceNotFoundException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.PartialLanguageGenerator;
import pl.edu.mimuw.loxim.protogen.api.wrappers.EnumTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class JavaPartialLanguageGenerator implements PartialLanguageGenerator{
	private ProtocolTypeWrapper descriptor;
	private File target_directory;
	private File overwrite_src_directory;
	private File working_directory,working_test_directory;
	
	private EnumsGenerator enumsGenerator;
	private PackagesGenerator packagesGenerator;
	private TestsGenerator testsGenerator;
	private boolean initialized=false;
	
	
	public void init() throws GeneratorException
	{
		try {
			Velocity.setProperty("resource.loader","class");
			Velocity.setProperty("class.resource.loader.class","org.apache.velocity.runtime.resource.loader.ClasspathResourceLoader");
			Velocity.init();
			//if(getOverwrite_src_directory()==null)
			//	throw new IllegalStateException("Call setOverwrite_src_directory first - before init");
			enumsGenerator=new EnumsGenerator(getOverwrite_src_directory());
			packagesGenerator=new PackagesGenerator(getOverwrite_src_directory());
			testsGenerator=new TestsGenerator(getOverwrite_src_directory());
			initialized=true;
		} catch (Exception e) {
			throw new GeneratorException(e);
		}
	}
	
	public File getOverwrite_src_directory() {
		return overwrite_src_directory;
	}
	
	public void setOverwrite_src_directory(File overwrite_src_directory) {
		if(initialized)
			throw new IllegalStateException("Call setOverwrite_src_directory first - before init");
		this.overwrite_src_directory = overwrite_src_directory;
	}
	
	public File getWorking_directory()
	{
		if(working_directory==null){
			working_directory=new File(getSafeTarget_directory().getAbsolutePath()+"/src/main/java/"+getPackageDir());
			working_directory.mkdirs();
		}
		return working_directory;
	}
	
	private File getSafeTarget_directory() {
		if (target_directory!=null)
			  return target_directory;
		else  return new File(".");
	}

	public File getWorking_tests_directory()
	{
		if(working_test_directory==null){
			working_test_directory=new File(getSafeTarget_directory().getAbsolutePath()+"/src/test/java/"+getPackageDir());
			working_test_directory.mkdirs();
		}
		return working_test_directory;
	}

	
	private String getPackageDir() {
		String packageName=descriptor.getLangMetadataMap("java").get("packageName");
		if(packageName!=null)
			return packageName.replace(".", "/");
			else return "/protocol"; 
	}

	public File getTarget_directory()
	{
		return target_directory;
	}
	public void setTarget_directory(File directory) {
		target_directory=directory;
	}
	
	public ProtocolTypeWrapper getDescriptor() {
		return descriptor;
	}
	
	public void setDescriptor(ProtocolTypeWrapper descriptor) {
		this.descriptor=descriptor;
		
	}

	public void generateEnum(EnumTypeWrapper enum_type) throws GeneratorException {
		enumsGenerator.generate(getWorking_directory(), enum_type);		
	}
	
	public void generateGlobals() throws GeneratorException {
		generatePom(descriptor);
	}

	private void generatePom(ProtocolTypeWrapper protocol_wrapper) throws GeneratorException{
		try {
			Template t=Velocity.getTemplate("/templates/pom.xml.vm");
			VelocityContext vc=new VelocityContext();
			vc.put("lang", protocol_wrapper.getLangMetadataMap("java"));
			FileWriter w=new FileWriter(new File(target_directory,"pom.xml"));
			t.merge(vc, w);
			w.close();
		} catch (ResourceNotFoundException e) {
			throw new GeneratorException(e);
		} catch (ParseErrorException e) {
			throw new GeneratorException(e);
		} catch (Exception e) {
			throw new GeneratorException(e);
		} 
	}

	public void generatePackageGroup(PackageGroupTypeWrapper package_group)
			throws GeneratorException {
		packagesGenerator.generate(working_directory, package_group);
	}

	public void generateTests() throws GeneratorException {
		testsGenerator.generate(getWorking_tests_directory(),descriptor);		
	}
}
