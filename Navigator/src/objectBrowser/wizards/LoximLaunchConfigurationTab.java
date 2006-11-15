package objectBrowser.wizards;


import java.util.List;

import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.connections.DatabaseConnection;
import objectBrowser.connections.DatabaseConnectionPool;
import objectBrowser.launcher.QueryLaunchConfiguration;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchConfigurationWorkingCopy;
import org.eclipse.debug.ui.AbstractLaunchConfigurationTab;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ComboViewer;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;

public class LoximLaunchConfigurationTab extends AbstractLaunchConfigurationTab {
	Label fQueryLabel;
	Text fQueryText;
	Button fQueryButton;
	
	Label fConnectionLabel;
	ComboViewer fConnectionViewer;
	
	public IWorkspaceRoot getWorkspaceRoot() {
		return ResourcesPlugin.getWorkspace().getRoot();
	}
	
    protected IFile getContext() {
        IWorkbenchWindow activeWorkbenchWindow= PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        if (activeWorkbenchWindow == null) {
                return null;
        }
        IWorkbenchPage page = activeWorkbenchWindow.getActivePage();
        if (page != null) {
                ISelection selection = page.getSelection();
                if (selection instanceof IStructuredSelection) {
                        IStructuredSelection ss = (IStructuredSelection)selection;
                        if (!ss.isEmpty()) {
                                Object obj = ss.getFirstElement();
                                
                                if (obj instanceof IFile) {
                                        return (IFile)obj;
                                }
                                /*
                                if (obj instanceof IResource) {
                                		((IResource)obj).getFullPath()
                                        IJavaElement je = JavaCore.create((IResource)obj);
                                        if (je == null) {
                                                IProject pro = ((IResource)obj).getProject();
                                                je = JavaCore.create(pro);
                                        }
                                        if (je != null) {
                                                return je;
                                        }
                                }
                                */
                        }
                }
                IEditorPart part = page.getActiveEditor();
                if (part != null) {
                        IEditorInput input = part.getEditorInput();
                        return (IFile)input.getAdapter(IFile.class);                        
                }
        }
        return null;
}

	
	public void createControl(Composite parent) {
		Composite comp = new Composite(parent, SWT.NONE);
		setControl(comp);
		GridLayout topLayout = new GridLayout();
		topLayout.numColumns = 3;
		comp.setLayout(topLayout);
		
		createSelectQuery(comp);
		
		createSpacer(comp);
		
		createSelectConnection(comp);
		
		Dialog.applyDialogFont(comp);

		validatePage();
	}
	
	public void createSpacer(Composite parent) {
		
	}
	
	public void createSelectQuery(Composite comp) {
	     fQueryLabel = new Label(comp, SWT.NONE);
         fQueryLabel.setText("Query file:");
         GridData gd = new GridData();
         gd.horizontalIndent = 25;
         fQueryLabel.setLayoutData(gd);

         fQueryText = new Text(comp, SWT.SINGLE | SWT.BORDER);
         fQueryText.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
         fQueryText.addModifyListener(new ModifyListener() {
        	 public void modifyText(ModifyEvent evt) {
        		 validatePage();
        		 updateLaunchConfigurationDialog();
        	 }
         });

         fQueryButton = new Button(comp, SWT.PUSH);
         fQueryButton.setText("Browse...");
         fQueryButton.addSelectionListener(new SelectionAdapter() {
                 public void widgetSelected(SelectionEvent evt) {
                         handleQueryButtonSelected();
                 }
         });
         setButtonGridData(fQueryButton);

	}
	
	public void setButtonGridData(Button button) {
		GridData gd = new GridData();
		button.setLayoutData(gd);
		/* TODO cos z hintem */
	}
	
	public void handleQueryButtonSelected() {
		
	}
	
	public void createSelectConnection(Composite comp) {
		fConnectionLabel = new Label(comp, SWT.NONE);
        fConnectionLabel.setText("Connection:");
        GridData gd = new GridData();
        gd.horizontalIndent = 25;
        fConnectionLabel.setLayoutData(gd);
        
        fConnectionViewer = new ComboViewer(comp, SWT.DROP_DOWN | SWT.READ_ONLY);
        fConnectionViewer.getCombo().setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        
        DatabaseConnectionPool pool = ObjectBrowserPlugin.getPlugin().getConnectionPool();
        
        List<DatabaseConnection> items = pool.getConnections();
        
        fConnectionViewer.setContentProvider(new ArrayContentProvider());
        fConnectionViewer.setLabelProvider(new LabelProvider() {
        	public String getText(Object element) {
        		return ((DatabaseConnection)element).getConnectionName();
        	}
        });
        
        fConnectionViewer.setInput(items);
        
        fConnectionViewer.addSelectionChangedListener(new ISelectionChangedListener() {
        	public void selectionChanged(SelectionChangedEvent event) {
        		validatePage();
        		updateLaunchConfigurationDialog();
        	}
        });

	}

	public String getName() {
		return "Main";
	}
	

	public void initializeFrom(ILaunchConfiguration configuration) {
		updateQueryFromConfig(configuration);
		updateConnectionFromConfig(configuration);
	}
	
	public void updateQueryFromConfig(ILaunchConfiguration configuration) {
		try {
			fQueryText.setText(configuration.getAttribute(QueryLaunchConfiguration.QUERY_ATTR, ""));
		} catch (CoreException e) {
			
		}
	}
	
	public void updateConnectionFromConfig(ILaunchConfiguration configuration) {
		try {
			int id = configuration.getAttribute(QueryLaunchConfiguration.CONN_ATTR, 1);
			DatabaseConnectionPool pool = ObjectBrowserPlugin.getPlugin().getConnectionPool();
			DatabaseConnection con = pool.getConnectionById(id);
			if (con == null) {
				fConnectionViewer.setSelection(new StructuredSelection());
			} else {
				fConnectionViewer.setSelection(new StructuredSelection(con));
			}
		} catch (CoreException e) {
			
		}
		
		/* TODO zapisywanie polaczenia */
	}

	public void performApply(ILaunchConfigurationWorkingCopy config) {
		config.setAttribute(QueryLaunchConfiguration.QUERY_ATTR, fQueryText.getText());
		IStructuredSelection selection = (IStructuredSelection)fConnectionViewer.getSelection();
		if (!selection.isEmpty()) {
			DatabaseConnection con = (DatabaseConnection)selection.getFirstElement();
			config.setAttribute(QueryLaunchConfiguration.CONN_ATTR, con.getId());
		}
	}

	public void setDefaults(ILaunchConfigurationWorkingCopy config) {
		config.setAttribute(QueryLaunchConfiguration.QUERY_ATTR, "");
		config.setAttribute(QueryLaunchConfiguration.CONN_ATTR, -1);
		IFile context = getContext();
		if (context != null) {
			if (context.getFullPath() != null) {
				config.setAttribute(QueryLaunchConfiguration.QUERY_ATTR, context.getFullPath().toString());
			}
		} 
	}

	public void validatePage() {
		setErrorMessage(null);
		setMessage(null);
		
		try {
			if (!getWorkspaceRoot().getFile(new Path(fQueryText.getText())).exists()) {
				setErrorMessage("Query source file not exists");
				return;
			}
		} catch (IllegalArgumentException e) {
			setErrorMessage("Invalid query source path");
			return;
		}
		// TODO
		
		if (fConnectionViewer.getSelection().isEmpty()) {
			setErrorMessage("You have to select the connection.");
			return;
		}
		
	
	}
}
