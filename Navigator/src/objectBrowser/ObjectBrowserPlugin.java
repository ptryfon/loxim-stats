package objectBrowser;

import objectBrowser.connections.DatabaseConnectionPool;
import objectBrowser.driver.loxim.result.Result;
import objectBrowser.driver.loxim.result.ResultVoid;

import org.eclipse.ui.plugin.*;
import org.eclipse.core.runtime.preferences.InstanceScope;
import org.eclipse.jface.resource.ImageDescriptor;
import org.osgi.framework.BundleContext;
import org.osgi.service.prefs.Preferences;

/**
 * The main plugin class to be used in the desktop.
 */
public class ObjectBrowserPlugin extends AbstractUIPlugin {

	//The shared instance.
	private volatile static ObjectBrowserPlugin plugin;
	
	private DatabaseConnectionPool connectionPool;
	
	private Result lastResult;
	
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
		lastResult = new ResultVoid();
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

	public Result getLastResult() {
		return lastResult;
	}

	public void setLastResult(Result lastResult) {
		this.lastResult = lastResult;
	}
}
