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
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageGroupTypeWrapper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.PackageTypeWrapper;
import pl.edu.mimuw.loxim.protogen.lang.java.types.JavaTypeHelper;

public class PackagesGenerator{
	
	private OverwriteHelper overwriteHelper;
	public PackagesGenerator(File overwrite_dir)
	{
		overwriteHelper=new OverwriteHelper(overwrite_dir);
	}

	public void generate(File working_directory,
			PackageGroupTypeWrapper pgtw) throws GeneratorException {
		File group_directory = getPackagesDirectory(working_directory, pgtw);

		VelocityContext group_velocity_context = getGroupVelocityContext(pgtw);
		for (PackageTypeWrapper ptw : pgtw.getPackageWrappers()) {
			generatePackage(group_directory, group_velocity_context, ptw);
		}
		generatePackagesFactory(group_directory, group_velocity_context, pgtw);

	}

	private void generatePackagesFactory(File group_directory,
			VelocityContext group_velocity_context, PackageGroupTypeWrapper pgtw) throws GeneratorException {
		try {
			File f=new File(group_directory,pgtw.getFactoryClassName()+".java");
			if(!overwriteHelper.useOverwrittingFile(f))
			{
				FileWriter fw = new FileWriter(f);
				Template t = Velocity.getTemplate("/templates/packagesFactory.java.vm");
				VelocityContext vc = new VelocityContext(group_velocity_context);
				t.merge(vc, fw);
				fw.close();
			}
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

	private void generatePackage(File group_directory,
			VelocityContext group_velocity_context, PackageTypeWrapper ptw) throws GeneratorException {
		try {
			File f=new File(group_directory,ptw.getClassName()+".java");
			if(!overwriteHelper.useOverwrittingFile(f))
			{
				FileWriter fw = new FileWriter(f);
				Template t = Velocity.getTemplate("/templates/package.java.vm");
				VelocityContext vc = new VelocityContext(group_velocity_context);
				vc.put("package",ptw);
				t.merge(vc, fw);
				fw.close();
			}
		} catch (IOException e1) {
			throw new GeneratorException(e1);
		} catch (ResourceNotFoundException e1) {
			throw new GeneratorException(e1);
		} catch (ParseErrorException e1) {
			throw new GeneratorException(e1);
		} catch (Exception e1) {
			throw new GeneratorException(e1);
		}	}

	private VelocityContext getGroupVelocityContext(
			PackageGroupTypeWrapper pgtw) {
		VelocityContext res = new VelocityContext(GlobalVelocityContextFactory
				.getGlobalVelocityContext(pgtw.getProtocol()));
		
		res.put("group", pgtw);
		res.put("java_group_package_name_sufix",JavaTypeHelper.getJavaPackageNameSuffixForGroup(pgtw));
		res.put("id_type", pgtw.getPacket_group_type().getIdType());
		return res;
	}

	private File getPackagesDirectory(File working_directory,
			PackageGroupTypeWrapper pgtw) {
		File f = new File(working_directory, JavaTypeHelper.getJavaPackageNameSuffixForGroup(pgtw));
		f.mkdirs();
		return f;
	}



}
