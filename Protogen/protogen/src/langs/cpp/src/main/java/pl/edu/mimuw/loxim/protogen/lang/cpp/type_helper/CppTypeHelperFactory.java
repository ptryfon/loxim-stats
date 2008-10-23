package pl.edu.mimuw.loxim.protogen.lang.cpp.type_helper;

import org.codehaus.plexus.DefaultPlexusContainer;
import org.codehaus.plexus.PlexusContainer;
import org.codehaus.plexus.PlexusContainerException;
import org.codehaus.plexus.component.repository.exception.ComponentLookupException;

import pl.edu.mimuw.loxim.protogen.api.GeneratorException;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.Type;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;

public class CppTypeHelperFactory {
   
	public static CppTypeHelper getCppTypeHelper(String typeName) throws PlexusContainerException, ComponentLookupException
	{
		PlexusContainer container=new DefaultPlexusContainer();
		return (CppTypeHelper) container.lookup(CppTypeHelper.ROLE,typeName);
	}
	
	public static CppTypeHelper getCppTypeHelper(Type typeName) throws PlexusContainerException, ComponentLookupException
	{
		return getCppTypeHelper(typeName.value());
	}
	
	public static CppTypeHelper getCppTypeHelper(FieldType field, ProtocolTypeWrapper descriptor) throws PlexusContainerException, ComponentLookupException,GeneratorException
	{
		PlexusContainer container=new DefaultPlexusContainer();
		if (field.getType()==null)
		{
			throw new GeneratorException("Unknown package type in field: "+field.getName());
		}
		CppTypeHelper type=(CppTypeHelper) container.lookup(CppTypeHelper.ROLE,field.getType().value());
		type.fieldInitialize(field, descriptor);
		return type;
	}

	
}
