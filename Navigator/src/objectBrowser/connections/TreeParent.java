package objectBrowser.connections;

import org.osgi.service.prefs.BackingStoreException;


public interface TreeParent extends TreeObject {
	public TreeObject [] getChildren() throws BackingStoreException ;
	public boolean hasChildren();
}
