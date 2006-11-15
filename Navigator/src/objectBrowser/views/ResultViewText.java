package objectBrowser.views;


import objectBrowser.ObjectBrowserPlugin;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.text.Document;
import org.eclipse.jface.text.TextViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IActionBars;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;



public class ResultViewText extends ViewPart {
	private TextViewer viewer;
	private Action action1;
	private Action action2;

	private static ResultViewText instance;
	private Document document;	

	/**
	 * The constructor.
	 */
	public ResultViewText() {
		instance = this;
	}
	
	public static ResultViewText getInstance() {
		return instance;
	}	

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {
		viewer = new TextViewer(parent, SWT.H_SCROLL | SWT.V_SCROLL);
		document = new Document();
		viewer.setEditable(false);
		viewer.setDocument(document);		
		
		makeActions();		
		contributeToActionBars();
	}


	private void contributeToActionBars() {
		IActionBars bars = getViewSite().getActionBars();
		fillLocalToolBar(bars.getToolBarManager());
	}


	
	private void fillLocalToolBar(IToolBarManager manager) {
		manager.add(action1);
		manager.add(action2);
	}

	private void makeActions() {
		action1 = new Action() {
			public void run() {
				showMessage("Action 1 executed");
			}
		};
		action1.setText("Action 1");
		action1.setToolTipText("Action 1 tooltip");
		action1.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().
			getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));
		
		action2 = new Action() {
			public void run() {
				showMessage("Action 2 executed");
			}
		};
		action2.setText("Action 2");
		action2.setToolTipText("Action 2 tooltip");
		action2.setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().
				getImageDescriptor(ISharedImages.IMG_OBJS_INFO_TSK));
	}

	private void showMessage(String message) {
		MessageDialog.openInformation(
			viewer.getControl().getShell(),
			"Result View (text)",
			message);
	}

	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {
		updateContent();
		viewer.getControl().setFocus();	
	}
	
	public void updateContent() {
		String s = ObjectBrowserPlugin.getPlugin().toString();
	}
	
	public Document getDocument() {
		return document;
	}	
}