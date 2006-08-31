package objectBrowser.editors.query;

import java.lang.reflect.InvocationTargetException;

import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.connections.DatabaseConnection;
import objectBrowser.driver.loxim.SBQLException;
import objectBrowser.driver.loxim.result.Result;
import objectBrowser.views.ResultView;
import objectBrowser.views.ResultViewText;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.texteditor.BasicTextEditorActionContributor;
import org.eclipse.ui.texteditor.ITextEditor;

public class QueryEditorActionContributor extends
		BasicTextEditorActionContributor {
	public static final String ACTION_EXEC = "Exec";

	public static final String M_DATABASE = "database";

	class ExecAction extends Action {
		QueryEditorActionContributor parent;

		DatabaseConnection con;
		IDocument document;
		Result res;

		public ExecAction(QueryEditorActionContributor parent) {
			super();
			setText("Execute query...");
			setToolTipText("Execute query...");

			this.parent = parent;
		}

		public void run() {
			ITextEditor editor = (ITextEditor) parent.getActiveEditorPart();
			document = editor.getDocumentProvider().getDocument(
					editor.getEditorInput());

			con = ObjectBrowserPlugin.getPlugin().getConnectionPool()
					.getConnections().get(0);
			
			if (con != null) {
				IRunnableWithProgress op = new IRunnableWithProgress() {					

					public void run(IProgressMonitor monitor)
							throws InvocationTargetException {
						try {
							monitor.beginTask("Executing query", 3);
							monitor.worked(0);

							con.doBegin();
							monitor.worked(1);
							res = con.doExecute(document.get());
							monitor.worked(2);
							con.doCommit();
							
							ObjectBrowserPlugin.getPlugin().setLastResult(res);
							
							ResultViewText rvt = ResultViewText.getInstance();
							if (rvt != null) rvt.updateContent();
							
							ResultView rv = ResultView.getInstance();
							if (rv != null) rv.updateContent();					

						} catch (SBQLException e) {
							throw new InvocationTargetException(e);
						} finally {
							monitor.done();
						}
					}
				};
				try {
					new ProgressMonitorDialog(editor.getSite().getShell()).run(
							false, false, op);
				} catch (InterruptedException e) {
					MessageDialog.openError(editor.getSite().getShell(),
							"Error", "Interrupted");
				} catch (InvocationTargetException e) {
					Throwable realException = e.getTargetException();
					MessageDialog.openError(editor.getSite().getShell(),
							"Error", realException.getMessage());
				}

			} else
				MessageDialog.openInformation(editor.getSite().getShell(),
						"Information", "No connection available");
		}
	}

	@Override
	public void contributeToMenu(IMenuManager menu) {
		super.contributeToMenu(menu);

		ExecAction execAction = new ExecAction(this);

		IMenuManager additionalMenu = menu; // (GroupMarker)menu.findUsingPath(IWorkbenchActionConstants.MB_ADDITIONS);
		if (additionalMenu != null) {
			MenuManager databaseMenu = new MenuManager("Database", M_DATABASE);
			databaseMenu.add(execAction);
			menu.add(databaseMenu);
		}
	}

}
