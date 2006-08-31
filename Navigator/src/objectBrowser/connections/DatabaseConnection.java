package objectBrowser.connections;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import objectBrowser.driver.loxim.Connection;
import objectBrowser.driver.loxim.SBQLException;
import objectBrowser.driver.loxim.TcpConnectionFactory;
import objectBrowser.driver.loxim.result.Result;
import objectBrowser.driver.loxim.result.ResultBool;

public class DatabaseConnection implements TreeObject {
	protected int id;
	protected String connectionName;
	protected String hostname;
	protected int port;
	protected String login;
	protected String password;
	protected boolean connected = false;
	
	protected DatabaseConnectionPool parent;
	protected Connection con;
	
	protected List<ITreeListener> listeners = new LinkedList<ITreeListener>();
	
	
	public void addListener(ITreeListener l) {
		listeners.add(l);
	}
	
	public void removeListener(ITreeListener l) {
		listeners.remove(l);
	}
	
	private void fireChange() {
		for (ITreeListener l : listeners) l.change(this);
	}		
	
	public TreeParent getParent() {
		return parent;
	}
	
	public void setParent(TreeParent parent) {
		this.parent = (DatabaseConnectionPool)parent;
	}
	
	public String getHostname() {
		return hostname;
	}
	public void setHostname(String hostname) {
		this.hostname = hostname;
	}
	public String getLogin() {
		return login;
	}
	public void setLogin(String login) {
		this.login = login;
	}
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
	public int getPort() {
		return port;
	}
	public void setPort(int port) {
		this.port = port;
	}
	
	public String toString() {
		if (connected) return hostname + ":" + port + " (*)";
		return hostname + ":" + port;
	}
	
	public Object getAdapter(Class c) {
		return null;
	}

	public String getConnectionName() {
		return connectionName;
	}

	public void setConnectionName(String connectionName) {
		this.connectionName = connectionName;
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public boolean isConnected() {
		return connected;
	}
	
	public boolean doLogin() throws SBQLException {
		con.execute("begin");
		Result res;
		if ("".equals(password)) {
			res = con.execute("validate " + login + " not_a_password");
		} else {
			res = con.execute("validate " + login + " " + password);
		}
		con.execute("end");
		if (!(res instanceof ResultBool)) throw new SBQLException("Unknown result", null);
		return ((ResultBool)res).getValue();
	}
	
	public void doBegin() throws SBQLException {
		con.execute("begin");
	}
	
	public void doCommit() throws SBQLException {
		con.execute("end");		
	}
	
	public void doRollback() throws SBQLException {
		con.execute("rollback");		
	}	
	
	public Result doExecute(String query) throws SBQLException {
		return con.execute(query);
	}
	
	public void connect() throws IOException, SBQLException {
		con = TcpConnectionFactory.getConnection(hostname, port);
		if (!doLogin()) throw new SBQLException("Incorrect login", null);
		connected = true;
        fireChange();
	}
	
	public void disconnect() throws SBQLException {
		con.close();
		connected = false;
		fireChange();
	}
}
