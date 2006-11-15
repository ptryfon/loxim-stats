package objectBrowser.views;


import java.io.IOException;
import java.lang.reflect.InvocationTargetException;

import loxim.driver.exception.SBQLException;
import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.connections.DatabaseConnection;
import objectBrowser.connections.DatabaseConnectionPool;
import objectBrowser.connections.ITreeListener;
import objectBrowser.connections.TreeObject;
import objectBrowser.connections.TreeParent;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.MessageDialogWithToggle;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.DrillDownAdapter;
import org.eclipse.ui.part.ViewPart;
import org.osgi.service.prefs.BackingStoreException;

/**
 * This sample class demonstrates how to plug-in a new workbench view. The view
 * shows data obtained from the model. The sample creates a dummy model on the
 * fly, but a real implementation would connect to the model available either in
 * this or another plug-in (e.g. the workspace). The view is connected to the
 * model using a content provider.
 * <p>
 * The view uses a label provider to define how model objects should be
 * presented in the view. Each view can present the same model objects using
 * different labels and icons, if needed. Alternatively, a single label provider
 * can be shared between views in order to ensure that objects of the same type
 * are presented in the same way everywhere.
 * <p>
 */

public class ObjectDatabaseExplorer extends ViewPart implements
		ISelectionChangedListener {
	private TreeViewer viewer;

	private DrillDownAdapter drillDownAdapter;

	private Action connectAction;

	private Action disconnectAction;

	private Action deleteConnectionAction;

	private Action propertiesAction;

	private Action doubleClickAction;

	private DatabaseConnection selectedConnection;

	private DatabaseConnectionPool connectionPool;

	/*
	 * The content provider class is responsible for providing objects to the
	 * view. It can wrap existing objects in adapters or simply return objects
	 * as-is. These objects may be sensitive to the current input of the view,
	 * or ignore it and always show the same content (like Task List, for
	 * example).
	 */

	class ViewContentProvider implements IStructuredContentProvider,
			ITreeContentProvider, ITreeListener {

		protected TreeViewer viewer;

		public void inputChanged(Viewer v, Object oldInput, Object newInput) {
			try {
				viewer = (TreeViewer) v;

				if (oldInput != null) {
					removeListenerFrom((TreeObject) oldInput);
				}
				if (newInput != null) {
					addListenerTo((TreeObject) newInput);
				}
			} catch (Exception e) {
				throw new RuntimeException(e);
			}
		}

		public void dispose() {
		}

		public void add(TreeObject object) {

			class RefreshThread implements Runnable {
				private TreeObject object;

				public RefreshThread(TreeObject _object) {
					object = _object;
				}

				public void run() {
					viewer.refresh(object.getParent(), false);
					if (object.getParent() == null
							|| object.getParent().getParent() == null)
						viewer.refresh();
				}
			}

			Display.getDefault().asyncExec(new RefreshThread(object));
		}

		public void remove(TreeObject object) {

			class RefreshThread implements Runnable {
				private TreeObject object;

				public RefreshThread(TreeObject _object) {
					object = _object;
				}

				public void run() {
					viewer.refresh(object.getParent(), false);
					if (object.getParent() == null
							|| object.getParent().getParent() == null)
						viewer.refresh();
				}
			}

			Display.getDefault().asyncExec(new RefreshThread(object));

		}

		public void addListenerTo(TreeObject object)
				throws BackingStoreException {
			object.addListener(this);
			if (object instanceof TreeParent) {
				for (TreeObject child : ((TreeParent) object).getChildren()) {
					addListenerTo(child);
				}
			}
		}

		public void removeListenerFrom(TreeObject object)
				throws BackingStoreException {
			object.removeListener(this);
			if (object instanceof TreeParent) {
				for (TreeObject child : ((TreeParent) object).getChildren()) {
					removeListenerFrom(child);
				}
			}
		}

		public Object[] getElements(Object parent) {
			return getChildren(parent);
		}

		public Object getParent(Object child) {
			if (child instanceof TreeObject) {
				return ((TreeObject) child).getParent();
			}
			return null;
		}

		public Object[] getChildren(Object parent) {
			try {
				if (parent instanceof TreeParent) {
					return ((TreeParent) parent).getChildren();
				}
			} catch (Exception e) {
				throw new RuntimeException(e);
			}
			return new Object[0];
		}

		public boolean hasChildren(Object parent) {
			if (parent instanceof TreeParent)
				return ((TreeParent) parent).hasChildren();
			return false;
		}

		public void change(TreeObject object) {
			class RefreshThread implements Runnable {
				private TreeObject object;

				public RefreshThread(TreeObject _object) {
					object = _object;
				}

				public void run() {
					viewer.refresh(object, true);
					updateActions((IStructuredSelection)viewer.getSelection());
				}
			}

			Display.getDefault().asyncExec(new RefreshThread(object));
		}

	}

	class ViewLabelProvider extends LabelProvider {

		public String getText(Object obj) {
			return obj.toString();
		}

		public Image getImage(Object obj) {
			String imageKey = ISharedImages.IMG_OBJ_ELEMENT;
			if (obj instanceof TreeParent)
				imageKey = ISharedImages.IMG_OBJ_FOLDER;
			return PlatformUI.getWorkbench().getSharedImages().getImage(
					imageKey);
		}
	}

	class NameSorter extends ViewerSorter {
	}

	/**
	 * The constructor.
	 */
	public ObjectDatabaseExplorer() {
	}

	/**
	 * This is a callback that will allow us to create the viewer and initialize
	 * it.
	 */
	public void createPartControl(Composite parent) {
		connectionPool = ObjectBrowserPlugin.getPlugin().getConnectionPool();
		viewer = new TreeViewer(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
		drillDownAdapter = new DrillDownAdapter(viewer);
		viewer.setContentProvider(new ViewContentProvider());
		viewer.setLabelProvider(new ViewLabelProvider());
		viewer.setSorter(new NameSorter());
		viewer.setInput(connectionPool);
		viewer.addSelectionChangedListener(this);
		makeActions();
		hookContextMenu();
		hookDoubleClickAction();
		contributeToActionBars();
	}

	private void hookContextMenu() {
		MenuManager menuMgr = new MenuManager("#PopupMenu");
		menuMgr.setRemoveAllWhenShown(true);
		menuMgr.addMenuListener(new IMenuListener() {
			public void menuAboutToShow(IMenuManager manager) {
				ObjectDatabaseExplorer.this.fillContextMenu(manager);
			}
		});
		Menu menu = menuMgr.createContextMenu(viewer.getControl());
		viewer.getControl().setMenu(menu);
		getSite().registerContextMenu(menuMgr, viewer);
	}

	private void contributeToActionBars() {
		IActionBars bars = getViewSite().getActionBars();
		fillLocalPullDown(bars.getMenuManager());
		fillLocalToolBar(bars.getToolBarManager());
	}

	private void fillLocalPullDown(IMenuManager manager) {
		manager.add(connectAction);
		manager.add(disconnectAction);
	}

	private void fillContextMenu(IMenuManager manager) {
		manager.add(connectAction);
		manager.add(disconnectAction);
		manager.add(new Separator());
		manager.add(deleteConnectionAction);
		manager.add(new Separator());
		drillDownAdapter.addNavigationActions(manager);
		// Other plug-ins can contribute there actions here
		manager.add(new Separator(IWorkbenchActionConstants.MB_ADDITIONS));
		manager.add(new Separator());
		manager.add(propertiesAction);
	}

	private void fillLocalToolBar(IToolBarManager manager) {
		manager.add(connectAction);
		manager.add(disconnectAction);
		manager.add(new Separator());
		drillDownAdapter.addNavigationActions(manager);
	}

	private void makeActions() {
		class ConnectAction extends Action {
			public ConnectAction() {
				setText("Connect");
				setToolTipText("Connect");
				setImageDescriptor(PlatformUI.getWorkbench().getSharedImages()
						.getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));

			}

			public void run() {
				IRunnableWithProgress op = new IRunnableWithProgress() {					
					
					public void run(IProgressMonitor monitor) throws InvocationTargetException {
						try {
							monitor.beginTask("Connecting to database", 1);
							monitor.worked(0);
							ISelection selection = viewer.getSelection();
							Object obj = ((IStructuredSelection) selection)
									.getFirstElement();
							if (obj instanceof DatabaseConnection) ((DatabaseConnection)obj).connect();
						} catch (SBQLException e) {
							throw new InvocationTargetException(e);
						} catch (IOException e) {
							throw new InvocationTargetException(e);
							
						} finally {
							monitor.done();
						}
					}
				};
				try {
					new ProgressMonitorDialog(viewer.getControl().getShell()).run(false, false, op);
				} catch (InterruptedException e) {
					MessageDialog.openError(viewer.getControl().getShell(), "Error", "Interrupted");										
				} catch (InvocationTargetException e) {
					Throwable realException = e.getTargetException();
					MessageDialog.openError(viewer.getControl().getShell(), "Error", realException.getMessage());
				}				
			}
		}

		connectAction = new ConnectAction();

		class DisconnectAction extends Action {
			public DisconnectAction() {
				setText("Disconnect");
				setToolTipText("Disconnect");
				setImageDescriptor(PlatformUI.getWorkbench().getSharedImages()
						.getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));

			}

			public void run() {
				IRunnableWithProgress op = new IRunnableWithProgress() {					
					
					public void run(IProgressMonitor monitor) throws InvocationTargetException {
						try {
							monitor.beginTask("Disconnecting from database", 1);
							monitor.worked(0);
							ISelection selection = viewer.getSelection();
							Object obj = ((IStructuredSelection) selection)
									.getFirstElement();
							if (obj instanceof DatabaseConnection) ((DatabaseConnection)obj).disconnect();							
						} catch (SBQLException e) {
							throw new InvocationTargetException(e);
						} finally {
							monitor.done();
						}
					}
				};
				try {
					new ProgressMonitorDialog(viewer.getControl().getShell()).run(false, false, op);
				} catch (InterruptedException e) {
					MessageDialog.openError(viewer.getControl().getShell(), "Error", "Interrupted");										
				} catch (InvocationTargetException e) {
					Throwable realException = e.getTargetException();
					MessageDialog.openError(viewer.getControl().getShell(), "Error", realException.getMessage());
				}				
				
			}
		}

		disconnectAction = new DisconnectAction();

		class DeleteAction extends Action {
			public DeleteAction() {
				setText("Delete connection");
				setToolTipText("Delete connection");
				setImageDescriptor(PlatformUI.getWorkbench().getSharedImages()
						.getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));

			}

			public void run() {
				try {
					ISelection selection = viewer.getSelection();
					Object obj = ((IStructuredSelection) selection)
							.getFirstElement();
					if (obj instanceof DatabaseConnection) {

						if (MessageDialogWithToggle.openConfirm(viewer
								.getControl().getShell(), "Confirm delete",
								"Are you sure you want to delete connection '"
										+ obj + "'"))
							connectionPool
									.removeConnection((DatabaseConnection) obj);
					}
				} catch (Exception e) {
					/* TODO - zrobi� to �adniej */
					throw new RuntimeException(e);
				}
			}
		}

		deleteConnectionAction = new DeleteAction();

		class PropertiesAction extends Action {
			public PropertiesAction() {
				setText("Properties");
				setToolTipText("Properties");
				// setImageDescriptor(PlatformUI.getWorkbench().getSharedImages()
				// .getImageDescriptor(ISharedImages.IMG_TOOL_DELETE));

			}

			public void run() {
				showMessage("Connection properties");
			}
		}

		propertiesAction = new PropertiesAction();

		doubleClickAction = new Action() {
			public void run() {
				ISelection selection = viewer.getSelection();
				Object obj = ((IStructuredSelection) selection)
						.getFirstElement();
				showMessage("Double-click detected on " + obj.toString());
			}
		};
	}

	private void hookDoubleClickAction() {
		viewer.addDoubleClickListener(new IDoubleClickListener() {
			public void doubleClick(DoubleClickEvent event) {
				doubleClickAction.run();
			}
		});
	}

	private void showMessage(String message) {

		MessageDialog.openInformation(viewer.getControl().getShell(),
				"Object Database Explorer", message);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		viewer.getControl().setFocus();
	}
	
	public void updateActions(IStructuredSelection selection) {
		connectAction.setEnabled(false);
		disconnectAction.setEnabled(false);
		deleteConnectionAction.setEnabled(false);
		propertiesAction.setEnabled(false);

		if (selection.isEmpty())
			return;

		/* TODO - nie mozemy pozwolic na wielokrotny wyb�r */
		TreeObject object = (TreeObject) selection.getFirstElement();
		if (object instanceof DatabaseConnection) {
			connectAction.setEnabled(!((DatabaseConnection)object).isConnected());
			disconnectAction.setEnabled(((DatabaseConnection)object).isConnected());
			deleteConnectionAction.setEnabled(true);
			propertiesAction.setEnabled(true);
			selectedConnection = (DatabaseConnection) selectedConnection;
		}
		;		
	}

	public void selectionChanged(SelectionChangedEvent event) {
		IStructuredSelection selection = (IStructuredSelection) event
				.getSelection();
		updateActions(selection);
	}
}