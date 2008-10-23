package pl.edu.mimuw.loxim.protogen.api.wrappers;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import org.w3c.dom.Element;

import pl.edu.mimuw.loxim.protogen.api.jaxb.EnumType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.LanguageType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.PacketGroupType;
import pl.edu.mimuw.loxim.protogen.api.jaxb.ProtocolType;

public class ProtocolTypeWrapper {

	private ProtocolType protocol;
	private Map<String, PackageGroupTypeWrapper> packageGroupWrappers;

	public ProtocolType getProtocol() {
		return protocol;
	}

	public ProtocolTypeWrapper(ProtocolType protocol) {
		super();
		this.protocol = protocol;
	}

	public void setProtocol(ProtocolType protocol) {
		this.protocol = protocol;
	}

	public EnumTypeWrapper findEnumWrapper(String enumName) {
		EnumTypeWrapper res = null;
		for (EnumType et : protocol.getEnums().getEnum()) {
			if (enumName.equals(et.getName()))
				res = new EnumTypeWrapper(et, this);
		}
		return res;
	}

	public PackageTypeWrapper findPackageWrapper(String groupName,
			String packageName) {
		PackageGroupTypeWrapper group = findGroupByName(groupName);
		return group.findPackageByName(packageName);
	}

	public PackageGroupTypeWrapper findGroupByName(String groupName) {
		if ((groupName == null) || (groupName.equals("")))
			return getPackageGroupWrappers().get("");
		else
			return getPackageGroupWrappers().get(groupName);
	}

	public PackageGroupTypeWrapper getMainGroup() {
		return findGroupByName(null);
	}

	public Map<String, PackageGroupTypeWrapper> getPackageGroupWrappers() {
		if (packageGroupWrappers == null) {
			packageGroupWrappers = new LinkedHashMap<String, PackageGroupTypeWrapper>();
			for (PacketGroupType pgt : protocol.getPacketGroups()
					.getPacketGroup()) {
				if (pgt.getName() == null)
					packageGroupWrappers.put("", new PackageGroupTypeWrapper(
							pgt, this));
				else
					packageGroupWrappers.put(pgt.getName(),
							new PackageGroupTypeWrapper(pgt, this));
			}
		}
		return packageGroupWrappers;
	}

	public LanguageType findLangMetadata(String lang) {
		if ((getProtocol().getMetadata()==null)||(getProtocol().getMetadata().getLanguages()==null)||(getProtocol().getMetadata().getLanguages().getLang()==null))
			return null;
		for (LanguageType lt : getProtocol().getMetadata().getLanguages()
				.getLang()) {
			if (lang.equals(lt.getId())) {
				return lt;
			}
		}
		return null;
	}

	public Map<String, String> getLangMetadataMap(String lang) {
		Map<String, String> langMetadataMap=new HashMap<String, String>();
		LanguageType languageType=findLangMetadata(lang);
		if(languageType!=null)
		for (Element e : languageType.getAny()) {
			langMetadataMap.put(e.getTagName(),e.getTextContent());
		}
		return langMetadataMap;
	}
}
