package objectBrowser.perspectives;

import org.eclipse.ui.IFolderLayout;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IPerspectiveFactory;

public class ObjectBrowsingPerspectiveFactory implements IPerspectiveFactory {

	public void createInitialLayout(IPageLayout layout) {
		String editor = layout.getEditorArea();
        layout.addView("objectBrowser.views.ObjectDatabaseExplorer", IPageLayout.LEFT, 0.3f, editor);
        layout.addView(IPageLayout.ID_RES_NAV, IPageLayout.TOP, 0.3f, "objectBrowser.views.ObjectDatabaseExplorer");
        
        IFolderLayout bottomRight = layout.createFolder("bottomRight", IPageLayout.BOTTOM, 0.5f, editor);
        
        bottomRight.addView("objectBrowser.views.ResultViewText");
        bottomRight.addView("objectBrowser.views.ResultView");
        
        layout.setEditorAreaVisible(true);
        layout.setFixed(false);
        
	}

}
