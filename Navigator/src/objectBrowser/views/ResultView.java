package objectBrowser.views;


import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.driver.loxim.result.Result;
import objectBrowser.editors.OBEditPartFactory;
import objectBrowser.model.CircleLayout;
import objectBrowser.model.Diagram;
import objectBrowser.model.LoximModelGenerator;
import objectBrowser.tests.DataGenerator1;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.tools.SelectionTool;
import org.eclipse.gef.ui.parts.GraphicalViewerImpl;
import org.eclipse.gef.ui.parts.ScrollingGraphicalViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.ViewPart;


/**
 * This sample class demonstrates how to plug-in a new
 * workbench view. The view shows data obtained from the
 * model. The sample creates a dummy model on the fly,
 * but a real implementation would connect to the model
 * available either in this or another plug-in (e.g. the workspace).
 * The view is connected to the model using a content provider.
 * <p>
 * The view uses a label provider to define how model
 * objects should be presented in the view. Each
 * view can present the same model objects using
 * different labels and icons, if needed. Alternatively,
 * a single label provider can be shared between views
 * in order to ensure that objects of the same type are
 * presented in the same way everywhere.
 * <p>
 */

public class ResultView extends ViewPart {
	
	private GraphicalViewer graphicalViewer;

	private static ResultView instance;
	
	private Result currentResult;

	/**
	 * The constructor.
	 */
	public ResultView() {
		instance = this;
	}
	
	public static ResultView getInstance() {
		return instance;
	}

	/**
	 * This is a callback that will allow us
	 * to create the viewer and initialize it.
	 */
	public void createPartControl(Composite parent) {
		
		graphicalViewer = new ScrollingGraphicalViewer();

		graphicalViewer.setEditDomain(new DefaultEditDomain(null));
		graphicalViewer.createControl(parent);
		graphicalViewer.getControl().setBackground(ColorConstants.listBackground);
		
		graphicalViewer.setEditPartFactory(new OBEditPartFactory());
		//graphicalViewer.setContents(ObjectBrowserPlugin.getPlugin().getLastResult());
		
	}


	/**
	 * Passing the focus request to the viewer's control.
	 */
	public void setFocus() {		
		updateContent();
		graphicalViewer.getControl().setFocus();
		graphicalViewer.getEditDomain().setActiveTool(new SelectionTool());
	}

	public void updateContent() {
		Result newResult = ObjectBrowserPlugin.getPlugin().getLastResult();
		if (currentResult != null && currentResult.equals(newResult)) return; 
		Diagram newModel = new LoximModelGenerator().makeModel(newResult);
		new CircleLayout().makeLayout(newModel);
		graphicalViewer.setContents(newModel);		
	}

	@Override
	public void dispose() {
		instance = null;
		super.dispose();
	}	

}