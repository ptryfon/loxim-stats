package objectBrowser.driver.loxim;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class StatementPackage implements Package {

	public void deserialize(InputStream stream, int size) throws IOException,
			SBQLException {
		// TODO Auto-generated method stub

	}

	public short getType() {
		return Package.TYPE_STATEMENT;
	}

	public int serialize(OutputStream stream) throws IOException {
		// TODO Auto-generated method stub
		return 0;
	}

	public long getStatementNumber() {
		/* TODO */
		return -1;
	}
}
