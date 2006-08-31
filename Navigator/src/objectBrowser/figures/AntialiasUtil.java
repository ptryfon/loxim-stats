package objectBrowser.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.swt.SWT;

public class AntialiasUtil {
	
	private AntialiasUtil() {
	super();
	}

	public static void setAntialias(Graphics graphics) {
		graphics.setAntialias(SWT.ON);
	}
	
}
