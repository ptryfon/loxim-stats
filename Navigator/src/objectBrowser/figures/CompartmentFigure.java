package objectBrowser.figures;


import org.eclipse.draw2d.AbstractBorder;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ToolbarLayout;
import org.eclipse.draw2d.geometry.Insets;

public class CompartmentFigure extends Figure {
	  public CompartmentFigure() {
		    ToolbarLayout layout = new ToolbarLayout();
		    layout.setMinorAlignment(ToolbarLayout.ALIGN_TOPLEFT);
		    layout.setStretchMinorAxis(false);
		    layout.setSpacing(2);
		    setLayoutManager(layout);
		    setBorder(new CompartmentFigureBorder());
		  }
		    
		  public class CompartmentFigureBorder extends AbstractBorder {
		    public org.eclipse.draw2d.geometry.Insets getInsets(IFigure figure) {
		      return new Insets(1,2,2,2);
		    }
		    public void paint(IFigure figure, Graphics graphics, Insets insets) {
		      graphics.drawLine(getPaintRectangle(figure, insets).getTopLeft(),
		                        tempRect.getTopRight());
		    }
		  }
}
