package objectBrowser.connections;

import java.util.LinkedList;
import java.util.List;

import objectBrowser.ObjectBrowserPlugin;

import org.osgi.service.prefs.BackingStoreException;
import org.osgi.service.prefs.Preferences;

public class DatabaseConnectionPool implements TreeParent {
	
	protected List<DatabaseConnection> connections = new LinkedList<DatabaseConnection>();	
	protected List<ITreeListener> listeners = new LinkedList<ITreeListener>();
	
	private void fetchData() throws BackingStoreException {
		connections.clear();
		Preferences prefs = ObjectBrowserPlugin.getPlugin().getInstancePreferences().node("DatabaseConnections");		
		for (String nodeName : prefs.childrenNames()) {
			DatabaseConnection con = new DatabaseConnection();
			Preferences node = prefs.node(nodeName);
			
			try {
				/* TODO - nieza�adne */
				con.setId(new Integer(nodeName.substring(3)).intValue());
				con.setConnectionName(node.get("connectionName", "<unnamed>"));
				con.setHostname(node.get("hostname", "<unknown>"));
				con.setLogin(node.get("login", "<unknown>"));
				con.setPassword(node.get("password", ""));
				con.setPort(node.getInt("port", -1));
				con.setParent(this);		
				connections.add(con);		
			} catch (Exception e) {
				
			}
		}
	}
	
	public DatabaseConnection getConnectionById(int id) {
		for (DatabaseConnection con : connections) {
			if (con.id == id) return con;
		}
		return null;
	}
	
	public DatabaseConnectionPool() throws BackingStoreException {
		fetchData();
	}	
	
	public void addListener(ITreeListener l) {
		listeners.add(l);
	}
	
	public void removeListener(ITreeListener l) {
		listeners.remove(l);
	}
	
	private void fireAdd(TreeObject addedObject) {
		for (ITreeListener l : listeners) l.add(addedObject);
	}
	
	private void fireRemove(TreeObject addedObject) {
		for (ITreeListener l : listeners) l.remove(addedObject);
	}		
	
	public String toString() {
		return "Database connections";
	}
	
	public void addConnection(DatabaseConnection child) throws BackingStoreException {
			int i = 0;			
			while (ObjectBrowserPlugin.getPlugin().getInstancePreferences().nodeExists("DatabaseConnections/con" + i)) i++;
			
			Preferences prefs = ObjectBrowserPlugin.getPlugin().getInstancePreferences().node("DatabaseConnections/con" + i);
			
			prefs.put("hostname", child.getHostname());
			prefs.putInt("port", child.getPort());
			prefs.put("login", child.getLogin());
			prefs.put("password", child.getPassword());
			prefs.put("connectionName", child.getConnectionName());
			child.setId(i);
			
			prefs.flush();
			
			connections.add(child);
			child.setParent(this);
			fireAdd(child);
	}
	public void removeConnection(DatabaseConnection child) throws BackingStoreException {
		ObjectBrowserPlugin.getPlugin().getInstancePreferences().node("DatabaseConnections/con" + child.getId()).removeNode();
		ObjectBrowserPlugin.getPlugin().getInstancePreferences().node("DatabaseConnections").flush();
		connections.remove(child);
		child.setParent(null);
		fireRemove(child);
	}	

	public List<DatabaseConnection> getConnections() {
		return connections;
	}
	
	public TreeObject [] getChildren() throws BackingStoreException {		
		List<DatabaseConnection> connections = getConnections();
		return connections.toArray(new TreeObject[connections.size()]);
	}
	
	public boolean hasChildren() {
		return connections.size() > 0;
	}

	public TreeParent getParent() {		
		return null;
	}

	public void setParent(TreeParent parent) {
		if (parent != null) throw new RuntimeException("DatabaseConnection should be root element in the tree");		
	}

	public Object getAdapter(Class adapter) {
		return null;
	}	
}
