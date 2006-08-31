package objectBrowser.editors.query;


import org.eclipse.ui.editors.text.TextEditor;

public class QueryEditor extends TextEditor {

	private ColorManager colorManager;

	public QueryEditor() {
		super();
		colorManager = new ColorManager();
		//setSourceViewerConfiguration(new XMLConfiguration(colorManager));
		setDocumentProvider(new SBQLDocumentProvider());
	}
	public void dispose() {
		colorManager.dispose();
		super.dispose();
	}

}
