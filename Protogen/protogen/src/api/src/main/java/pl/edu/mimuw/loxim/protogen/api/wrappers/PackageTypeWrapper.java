package pl.edu.mimuw.loxim.protogen.api.wrappers;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.jaxb.FieldType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketType;

public class PackageTypeWrapper {
	
	private PacketType _package;
	private PackageGroupTypeWrapper packet_group_type;
	
	public PackageTypeWrapper(PacketType pt,PackageGroupTypeWrapper a_packet_group_type) {
		_package=pt;
		packet_group_type=a_packet_group_type;
	}
	
	public PacketType getPackage() {
		return _package;
	}
	
	public void setPackage(PacketType packet) {
		this._package = packet;
	}
	
	public ProtocolTypeWrapper getProtocol() {
		return packet_group_type.getProtocol();
	}
	
	
	public String getClassName()
	{
		return getClassNameForPacket(_package.getName());
	}
	
	public String getExtendClassName()
	{
		if(_package.getExtendsPacket()==null || _package.getExtendsPacket().equals(""))
			return packet_group_type.getSuperclassName();
		else
			return getClassNameForPacket(_package.getExtendsPacket());
	}
	
	private static String getClassNameForPacket(String name)
	{
		return name==null?null:CodeHelper.upperFirstChar(name)+"Package";		
	}
	
	public PackageGroupTypeWrapper getPacket_group_type() {
		return packet_group_type;
	}
	
	public void setPacket_group_type(PackageGroupTypeWrapper packet_group_type) {
		this.packet_group_type = packet_group_type;
	}

	public boolean isAbstract() {
		return ((_package.getIdValue()==null) || (_package.getIdValue().equals("")));
	}

	public FieldType getFieldByName(String fieldName) {
		for(FieldType ft:_package.getField())
		{
			if(ft.getName().equals(fieldName))
				return ft;
		}
		return null;
	}
	
	
	
}
