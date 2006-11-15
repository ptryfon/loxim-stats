package objectBrowser.connections;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import loxim.driver.SimpleConnection;
import loxim.driver.TcpConnectionFactory;
import loxim.driver.exception.SBQLException;
import loxim.driver.exception.SBQLProtocolException;
import loxim.pool.ConnectionService;
import loxim.pool.LoximObjectFactory;
import objectBrowser.ObjectBrowserPlugin;


public class DatabaseConnection implements TreeObject {
	protected int id;
	protected String connectionName;
	protected String hostname;
	protected int port;
	protected String login;
	protected String password;
	protected boolean connected = false;
	
	protected DatabaseConnectionPool parent;
	protected SimpleConnection con;
	
	protected ConnectionService service;	
	
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
		if (connected) return connectionName + "(connected)";
		return connectionName;
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
			
	public void doExecute(String query) throws SBQLException {
		con.beginTransaction();
		try {
			service.executeQuery(query);
			service.expandAll();
			ObjectBrowserPlugin.getPlugin().setPoolChanged(true);
		} catch (SBQLException e) {
			con.rollbackTransaction();
			throw e;
		}
		con.commitTransation();
	}
	
	public void connect() throws IOException, SBQLException {
		con = new SimpleConnection(TcpConnectionFactory.getConnection(hostname, port));
		if (!con.login(login, password)) throw new SBQLProtocolException("Incorrect login");
		service = new ConnectionService(ObjectBrowserPlugin.getPlugin().getObjectPool(), con, new LoximObjectFactory());
		ObjectBrowserPlugin.getPlugin().setPoolChanged(true);
		connected = true;
        fireChange();
	}
	
	public void disconnect() throws SBQLException {
		con.close();
		connected = false;
		fireChange();
	}
}
