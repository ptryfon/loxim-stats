package szbd.test;

import java.net.Socket;
import java.util.List;

import com.sun.jmx.mbeanserver.MetaData;

import szbd.client.Connection;
import szbd.client.TcpConnection;
import szbd.client.TcpConnectionFactory;
import szbd.client.result.Result;
import szbd.metadata.MetadataFactory;
import szbd.metadata.TypeDefinition;

public class Test1 {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			Connection con = TcpConnectionFactory.getConnection("127.0.0.1", 6543);			
			Result r = con.execute("begin;");
			System.out.println(r.toString());
			MetadataFactory mf = new MetadataFactory(con);
			List<TypeDefinition> ld = mf.fetchTypes();
			r = con.execute("end;");
			System.out.println(r.toString());
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}

}
