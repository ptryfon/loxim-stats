package objectBrowser.wizards;

import org.eclipse.jface.dialogs.IDialogPage;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

/**
 * The "New" wizard page allows setting the container for the new file as well
 * as the file name. The page will only accept file name without the extension
 * OR with the extension that matches the expected one (con).
 */

public class NewConnectionWizardName extends WizardPage {
	
	private Text connectionNameText;
	private Text hostText;
	private Text portText;
	private Text loginText;
	private Text passwordText;


	/**
	 * Constructor for SampleNewWizardPage.
	 * 
	 * @param pageName
	 */
	public NewConnectionWizardName(ISelection selection) {
		super("wizardPage");
		setTitle("Database connection");
		setDescription("This wizard creates a connection to the LoXiM database.");
	}

	/**
	 * @see IDialogPage#createControl(Composite)
	 */
	public void createControl(Composite parent) {
		Composite container = new Composite(parent, SWT.NULL);
		GridLayout layout = new GridLayout();
		container.setLayout(layout);
		layout.numColumns = 2;
		layout.verticalSpacing = 9;
		
		{
			Label label = new Label(container, SWT.NULL);
			label.setText("&Connection name:");
			
			connectionNameText = new Text(container, SWT.BORDER | SWT.SINGLE);
			
			GridData gd = new GridData(GridData.FILL_HORIZONTAL);
			connectionNameText.setLayoutData(gd);
			connectionNameText.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
						dialogChanged();
					}
				});		
			
		}
		
		{
			Label label = new Label(container, SWT.NULL);
			label.setText("&Host:");
			
			hostText = new Text(container, SWT.BORDER | SWT.SINGLE);
			
			GridData gd = new GridData(GridData.FILL_HORIZONTAL);
			hostText.setLayoutData(gd);
			hostText.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
						dialogChanged();
					}
				});				
		}
		
		{
			Label label = new Label(container, SWT.NULL);
			label.setText("&Port:");
			
			portText = new Text(container, SWT.BORDER | SWT.SINGLE);
			
			GridData gd = new GridData(GridData.FILL_HORIZONTAL);
			portText.setLayoutData(gd);
			portText.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
						dialogChanged();
					}
				});				
		}
		
		{
			Label label = new Label(container, SWT.NULL);
			label.setText("&Username:");
			
			loginText = new Text(container, SWT.BORDER | SWT.SINGLE);
			
			GridData gd = new GridData(GridData.FILL_HORIZONTAL);
			loginText.setLayoutData(gd);
			loginText.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
						dialogChanged();
					}
				});				
		}
		
		{
			Label label = new Label(container, SWT.NULL);
			label.setText("&Password:");
			
			passwordText = new Text(container, SWT.BORDER | SWT.SINGLE);
			
			GridData gd = new GridData(GridData.FILL_HORIZONTAL);
			passwordText.setEchoChar('*');
			passwordText.setLayoutData(gd);
			passwordText.addModifyListener(new ModifyListener() {
				public void modifyText(ModifyEvent e) {
						dialogChanged();
					}
				});				
		}		
				
		
		initialize();
		dialogChanged();
		setControl(container);
	}

	/**
	 * Tests if the current workbench selection is a suitable container to use.
	 */

	private void initialize() {
		/*
		if (selection != null && selection.isEmpty() == false
				&& selection instanceof IStructuredSelection) {
			IStructuredSelection ssel = (IStructuredSelection) selection;
			if (ssel.size() > 1)
				return;
			Object obj = ssel.getFirstElement();
			if (obj instanceof IResource) {
				IContainer container;
				if (obj instanceof IContainer)
					container = (IContainer) obj;
				else
					container = ((IResource) obj).getParent();
				containerText.setText(container.getFullPath().toString());
			}
		}
		*/
		connectionNameText.setText("New connection");
	}


	/**
	 * Ensures that both text fields are set.
	 */

	private void dialogChanged() {

		
		if (getConnectionName().length() == 0) {
			updateStatus("Connection name must be specified");
			return;
		}
		
		if (getHost().length() == 0) {
			updateStatus("Hostname must be specified");
			return;
		}		

		if ((getPort() <= 0) || (getPort() > 65534)) {
			updateStatus("Incorrect port number");
			return;			
		}		
		
		if (getLogin().length() == 0) {
			updateStatus("Username must be specified");
			return;
		}
		


				
		updateStatus(null);
	}

	private void updateStatus(String message) {
		setErrorMessage(message);
		setPageComplete(message == null);
	}


	public String getConnectionName() {
		return connectionNameText.getText();
	}
	
	public String getHost() {
		return hostText.getText();
	}
	
	public Integer getPort() {
		try {
			return new Integer(portText.getText());
		} catch (NumberFormatException e) {
			return -1;
		}
	}	
	
	public String getLogin() {
		return loginText.getText();
	}
	
	public String getPassword() {
		return passwordText.getText();
	}	
}