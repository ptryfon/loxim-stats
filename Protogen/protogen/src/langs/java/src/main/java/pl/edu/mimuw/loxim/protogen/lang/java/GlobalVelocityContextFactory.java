package pl.edu.mimuw.loxim.protogen.lang.java;

import org.apache.velocity.VelocityContext;
import org.apache.velocity.tools.generic.IteratorTool;

import pl.edu.mimuw.loxim.protogen.api.CodeHelper;
import pl.edu.mimuw.loxim.protogen.api.wrappers.ProtocolTypeWrapper;
import pl.edu.mimuw.loxim.protogen.lang.java.hlp.StringUtil;
import pl.edu.mimuw.loxim.protogen.lang.java.types.JavaTypeHelper;

public class GlobalVelocityContextFactory {
	private static ProtocolTypeWrapper last_ptw;
	private static VelocityContext last_vc;
	
	
	/*Do not modify returned context*/
	public static VelocityContext getGlobalVelocityContext(ProtocolTypeWrapper ptw)
	{
		if(last_ptw!=ptw)
		{
			last_vc=new VelocityContext();
			
			/*tools*/
			last_vc.put("loop",new IteratorTool());
			last_vc.put("stringUtil",new StringUtil());
			last_vc.put("codeHelper",new CodeHelper());
			last_vc.put("jth", new JavaTypeHelper(ptw));
			/*data*/
			last_vc.put("packageName",ptw.getLangMetadataMap("java").get("packageName"));

			last_ptw=ptw;
		}
		return last_vc;
	}
}
