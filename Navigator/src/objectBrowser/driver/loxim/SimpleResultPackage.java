package objectBrowser.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class SimpleResultPackage implements Package {

	public short getType() {		
		return Package.TYPE_SIMPLERESULT;
	}

	public int serialize(OutputStream stream) throws IOException {
		// TODO Auto-generated method stub
		return 0;
	}

	public void deserialize(InputStream stream, int size) throws IOException {
		// TODO Auto-generated method stub

	}

}
