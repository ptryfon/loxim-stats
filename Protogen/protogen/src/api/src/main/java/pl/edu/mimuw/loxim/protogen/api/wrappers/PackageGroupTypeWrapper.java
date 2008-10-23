package pl.edu.mimuw.loxim.protogen.api.wrappers;

import java.security.InvalidParameterException;
import java.util.LinkedList;
import java.util.List;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketGroupType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketType;

public class PackageGroupTypeWrapper {
	
	private PacketGroupType packet_group_type;
	private ProtocolTypeWrapper protocol;
	private List<PackageTypeWrapper> package_wrappers;
	
	public PackageGroupTypeWrapper(PacketGroupType a_packet_group_type, ProtocolTypeWrapper a_protocol) {
		protocol=a_protocol;
		packet_group_type=a_packet_group_type;
	}
	
	
	public ProtocolTypeWrapper getProtocol() {
		return protocol;
	}
	
	public void setProtocol(ProtocolTypeWrapper protocol) {
		this.protocol = protocol;
	}
	
	public PacketGroupType getPacket_group_type() {
		return packet_group_type;
	}
	
	public void setPacket_group_type(PacketGroupType packet_group_type) {
		this.packet_group_type = packet_group_type;
	}


	public String getGroupName() {
		if(packet_group_type.getName()==null )
		{
			return "";
		}
		else 
		{
			return packet_group_type.getName();
		}
	}


	public boolean isMainGroup() {
		return getGroupName().equals("");
	}


	public PackageTypeWrapper findPackageByName(String packageName) {
		if(packageName==null)
			throw new InvalidParameterException("findPackageByName(null) is not supported");
		for (PacketType pt:packet_group_type.getPackets().getPacket())
		{
			if (packageName.equals(pt.getName()))
			{
				return new PackageTypeWrapper(pt,this);
			}
		}
		return null;
	}


	public String getSuperclassName() {
		return (packet_group_type.getExtends()==null || packet_group_type.getExtends().equals(""))?"Package":CodeHelper.upperFirstChar(packet_group_type.getExtends()+"Package");
	}


	public List<PackageTypeWrapper> getExamplePackages() {
		List<PackageTypeWrapper> ptw=new LinkedList<PackageTypeWrapper>();
		int size=packet_group_type.getPackets().getPacket().size();
		if(size>0)
			ptw.add(new PackageTypeWrapper(packet_group_type.getPackets().getPacket().get(0),this));
		if(size>1)
			ptw.add(new PackageTypeWrapper(packet_group_type.getPackets().getPacket().get(size-1),this));
		if(size>2)
			ptw.add(new PackageTypeWrapper(packet_group_type.getPackets().getPacket().get(size/2),this));
		return ptw;
	}


	public List<PackageTypeWrapper> getPackageWrappers() {
		if (package_wrappers==null)
		{
			package_wrappers=new LinkedList<PackageTypeWrapper>();
			for(PacketType pt:getPacket_group_type().getPackets().getPacket())
			{
				package_wrappers.add(new PackageTypeWrapper(pt,this));
			}
		}
		return package_wrappers;
	}


	public String getFactoryClassName() {
		return CodeHelper.upperFirstChar(getGroupName()+"PackagesFactory");
	}
	

}
