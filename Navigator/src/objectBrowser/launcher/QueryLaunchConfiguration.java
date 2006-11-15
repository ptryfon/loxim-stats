package objectBrowser.launcher;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import loxim.driver.exception.SBQLException;
import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.connections.DatabaseConnection;
import objectBrowser.connections.DatabaseConnectionPool;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.debug.core.ILaunch;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.model.ILaunchConfigurationDelegate;

public class QueryLaunchConfiguration implements ILaunchConfigurationDelegate {
	public static final String QUERY_ATTR = ObjectBrowserPlugin.PLUGIN_ID + ".QUERY";
	public static final String CONN_ATTR = ObjectBrowserPlugin.PLUGIN_ID + ".CONNECTION";
	
	private IWorkspaceRoot getWorkspaceRoot() {
		return ResourcesPlugin.getWorkspace().getRoot();
	}
	
	public void launch(ILaunchConfiguration configuration, String mode,
			ILaunch launch, IProgressMonitor monitor) throws CoreException {
		
		int id = configuration.getAttribute(QueryLaunchConfiguration.CONN_ATTR, -1);
		DatabaseConnectionPool pool = ObjectBrowserPlugin.getPlugin().getConnectionPool();
		DatabaseConnection con = pool.getConnectionById(id);
		if (con == null) throw new RuntimeException("Connection not found");
		
		IFile file = getWorkspaceRoot().getFile(new Path(configuration.getAttribute(QueryLaunchConfiguration.QUERY_ATTR, "")));
		
		if (!con.isConnected())
			throw new CoreException(new Status(IStatus.ERROR, ObjectBrowserPlugin.PLUGIN_ID, IStatus.OK, "Not connected to database", null));
		
		InputStream stream = file.getContents();

		BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
		
		try {
		
			StringBuffer buffer = new StringBuffer();
			while (reader.ready()) {
				buffer.append(reader.readLine() + "\n");
			}
			
			stream.close();
			
			con.doExecute(buffer.toString());
			
			//ResultViewText rvt = ResultViewText.getInstance();
			//if (rvt != null) rvt.updateContent();
			
			//ResultView rv = ResultView.getInstance();
			//if (rv != null) rv.updateContent();		
		
		} catch (IOException e) {
			throw new CoreException(new Status(IStatus.ERROR, ObjectBrowserPlugin.PLUGIN_ID, IStatus.OK, "IOException", e));
		} catch (SBQLException e) {
			throw new CoreException(new Status(IStatus.ERROR, ObjectBrowserPlugin.PLUGIN_ID, IStatus.OK, "SBQLException", e));
		}
	}

}
