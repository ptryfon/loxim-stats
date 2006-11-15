package objectBrowser.launcher;

import objectBrowser.wizards.LoximLaunchConfigurationTab;

import org.eclipse.debug.ui.AbstractLaunchConfigurationTabGroup;
import org.eclipse.debug.ui.ILaunchConfigurationDialog;
import org.eclipse.debug.ui.ILaunchConfigurationTab;

public class LoximTabGroup extends AbstractLaunchConfigurationTabGroup {

	public LoximTabGroup() {
		// TODO Auto-generated constructor stub
	}

	public void createTabs(ILaunchConfigurationDialog dialog, String mode) {
		ILaunchConfigurationTab[] tabs = new ILaunchConfigurationTab[] {
				new LoximLaunchConfigurationTab()
		};
		
		setTabs(tabs);

	}

}
