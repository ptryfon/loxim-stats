package objectBrowser;

import loxim.pool.ObjectPool;
import loxim.pool.ObjectPoolImpl;
import objectBrowser.connections.DatabaseConnectionPool;

import org.eclipse.core.runtime.preferences.InstanceScope;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;
import org.osgi.service.prefs.Preferences;

/**
 * The main plugin class to be used in the desktop.
 */
public class ObjectBrowserPlugin extends AbstractUIPlugin {
	public static final String PLUGIN_ID = "Navigator2";

	//The shared instance.
	private volatile static ObjectBrowserPlugin plugin;
	
	private DatabaseConnectionPool connectionPool;
	
	private ObjectPool objectPool;
	
	private boolean poolChanged;
	
	public DatabaseConnectionPool getConnectionPool() {
		return connectionPool;
	}

	/**
	 * The constructor.
	 */
	public ObjectBrowserPlugin() {
		plugin = this;		
	}

	/**
	 * This method is called upon plug-in activation
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		connectionPool = new DatabaseConnectionPool();
		objectPool = new ObjectPoolImpl();
	}

	/**
	 * This method is called when the plug-in is stopped
	 */
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
		plugin = null;
	}

	/**
	 * Returns the shared instance.
	 */
	public static ObjectBrowserPlugin getPlugin() {
		return plugin;
	}

	/**
	 * Returns an image descriptor for the image file at the given
	 * plug-in relative path.
	 *
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return AbstractUIPlugin.imageDescriptorFromPlugin("ObjectBrowser", path);
	}

	public Preferences getInstancePreferences() {
		return new InstanceScope().getNode(getBundle().getSymbolicName());
	}

	public ObjectPool getObjectPool() {
		return objectPool;
	}

	public boolean isPoolChanged() {
		return poolChanged;
	}

	public void setPoolChanged(boolean poolChanged) {
		this.poolChanged = poolChanged;
	}

}
