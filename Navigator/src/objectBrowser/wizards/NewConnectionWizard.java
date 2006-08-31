package objectBrowser.wizards;

import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.connections.DatabaseConnection;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.INewWizard;
import org.eclipse.ui.IWorkbench;
import org.eclipse.core.runtime.*;
import org.eclipse.jface.operation.*;
import java.lang.reflect.InvocationTargetException;
import java.util.prefs.BackingStoreException;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.CoreException;
import java.io.*;
import org.eclipse.ui.*;
import org.eclipse.ui.ide.IDE;
import org.osgi.service.prefs.Preferences;

/**
 * This is a sample new wizard. Its role is to create a new file 
 * resource in the provided container. If the container resource
 * (a folder or a project) is selected in the workspace 
 * when the wizard is opened, it will accept it as the target
 * container. The wizard creates one file with the extension
 * "con". If a sample multi-page editor (also available
 * as a template) is registered for the same extension, it will
 * be able to open it.
 */

public class NewConnectionWizard extends Wizard implements INewWizard {
	private DatabaseConnection newConnection;
	private NewConnectionWizardName page;
	private ISelection selection;

	/**
	 * Constructor for objectBrowser.wizards.NewConnectionWizard.
	 */
	public NewConnectionWizard() {
		super();
		setNeedsProgressMonitor(true);
	}
	
	/**
	 * Adding the page to the wizard.
	 */

	public void addPages() {
		page = new objectBrowser.wizards.NewConnectionWizardName(selection);
		addPage(page);
	}

	/**
	 * This method is called when 'Finish' button is pressed in
	 * the wizard. We will create an operation and run it
	 * using wizard as execution context.
	 */
	public boolean performFinish() {
		newConnection = new DatabaseConnection();
		newConnection.setHostname(page.getHost());
		newConnection.setPort(page.getPort().intValue());
		newConnection.setLogin(page.getLogin());
		newConnection.setPassword(page.getPassword());
		newConnection.setConnectionName(page.getConnectionName());
		
		IRunnableWithProgress op = new IRunnableWithProgress() {
			
			
			public void run(IProgressMonitor monitor) throws InvocationTargetException {
				try {
					doFinish(newConnection, monitor);
				} catch (CoreException e) {
					throw new InvocationTargetException(e);
				} catch (org.osgi.service.prefs.BackingStoreException e) {
					throw new InvocationTargetException(e);
					
				} finally {
					monitor.done();
				}
			}
		};
		try {
			getContainer().run(true, false, op);
		} catch (InterruptedException e) {
			return false;
		} catch (InvocationTargetException e) {
			Throwable realException = e.getTargetException();
			MessageDialog.openError(getShell(), "Error", realException.getMessage());
			return false;
		}
		return true;
	}
	
	/**
	 * The worker method. It will find the container, create the
	 * file if missing or just replace its contents, and open
	 * the editor on the newly created file.
	 */

	private void doFinish(
		DatabaseConnection connection,
		IProgressMonitor monitor)
		throws CoreException, org.osgi.service.prefs.BackingStoreException {
		
		monitor.beginTask("Creating database connection", 1);
		
		ObjectBrowserPlugin.getPlugin().getConnectionPool().addConnection(connection);

		monitor.worked(1);
	}
	
	/**
	 * We will initialize file contents with a sample text.
	 */

	private InputStream openContentStream() {
		String contents =
			"This is the initial file contents for *.con file that should be word-sorted in the Preview page of the multi-page editor";
		return new ByteArrayInputStream(contents.getBytes());
	}

	private void throwCoreException(String message) throws CoreException {
		IStatus status =
			new Status(IStatus.ERROR, "ObjectBrowser", IStatus.OK, message, null);
		throw new CoreException(status);
	}

	/**
	 * We will accept the selection in the workbench to see if
	 * we can initialize from it.
	 * @see IWorkbenchWizard#init(IWorkbench, IStructuredSelection)
	 */
	public void init(IWorkbench workbench, IStructuredSelection selection) {
		this.selection = selection;
	}
}