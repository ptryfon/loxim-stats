package objectBrowser.editors;

import objectBrowser.ObjectBrowserPlugin;
import objectBrowser.tests.DataGenerator1;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.gef.DefaultEditDomain;
import org.eclipse.gef.palette.PaletteDrawer;
import org.eclipse.gef.palette.PaletteGroup;
import org.eclipse.gef.palette.PaletteRoot;
import org.eclipse.gef.palette.SelectionToolEntry;
import org.eclipse.gef.palette.ToolEntry;
import org.eclipse.gef.ui.palette.FlyoutPaletteComposite.FlyoutPreferences;
import org.eclipse.gef.ui.parts.GraphicalEditor;
import org.eclipse.gef.ui.parts.GraphicalEditorWithFlyoutPalette;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.ui.texteditor.IMarkerUpdater;

public class ObjectiveDataEditor extends GraphicalEditorWithFlyoutPalette {
	
	/** Preference ID used to persist the palette location. */
	private static final String PALETTE_DOCK_LOCATION = "ConnectionEditorPaletteFactory.Location";
	/** Preference ID used to persist the palette size. */
	private static final String PALETTE_SIZE = "ConnectionEditorPaletteFactory.Size";
	/** Preference ID used to persist the flyout palette's state. */
	private static final String PALETTE_STATE = "ConnectionEditorPaletteFactory.State";
	private static final int DEFAULT_PALETTE_SIZE = 125;
	
	private PaletteRoot paletteRoot;

	private IPreferenceStore getPreferenceStore() {
		return ObjectBrowserPlugin.getPlugin().getPreferenceStore();
	}

	@Override
	protected FlyoutPreferences getPalettePreferences() {
		getPreferenceStore().setDefault(PALETTE_DOCK_LOCATION, -1);
		getPreferenceStore().setDefault(PALETTE_STATE, -1);
		getPreferenceStore().setDefault(PALETTE_SIZE, DEFAULT_PALETTE_SIZE);

		return new FlyoutPreferences() {

			public int getDockLocation() {
				return getPreferenceStore().getInt(PALETTE_DOCK_LOCATION);
			}

			public int getPaletteState() {
				return getPreferenceStore().getInt(PALETTE_STATE);
			}

			public int getPaletteWidth() {
				return getPreferenceStore().getInt(PALETTE_SIZE);
			}

			public void setDockLocation(int location) {
				getPreferenceStore().setValue(PALETTE_DOCK_LOCATION, location);
			}

			public void setPaletteState(int state) {
				getPreferenceStore().setValue(PALETTE_STATE, state);
			}

			public void setPaletteWidth(int width) {
				getPreferenceStore().setValue(PALETTE_SIZE, width);
			}
		};
	}

	@Override
	protected PaletteRoot getPaletteRoot() {
		if (paletteRoot == null) createPaletteRoot();
		return paletteRoot;
	}
	
	protected void createPaletteRoot() {
		paletteRoot = new PaletteRoot();
		paletteRoot.setDescription("Mail palette");
		
		PaletteGroup mainGroup = new PaletteGroup("Main group");		
		
		ToolEntry selectionTool = new SelectionToolEntry();
		selectionTool.setVisible(true);
		mainGroup.add(selectionTool);
		
		paletteRoot.add(mainGroup);		
		paletteRoot.setDefaultEntry(selectionTool);
	}

	public ObjectiveDataEditor() {
		setEditDomain(new DefaultEditDomain(this));
	}

	protected void configureGraphicalViewer() {
		super.configureGraphicalViewer(); // Sets the viewer's background to
											// System "white"
		getGraphicalViewer().setEditPartFactory(new OBEditPartFactory());
	}

	protected void initializeGraphicalViewer() {
		getGraphicalViewer().setContents(new DataGenerator1().makeSample());		
	}

	public void doSave(IProgressMonitor monitor) {
		/* TODO */
	}

	public void doSaveAs() {
		/* TODO */
	}

	public void gotoMarker(IMarkerUpdater marker) {

	}



	public boolean isSaveAsAllowed() {
		return false;
	}

}
