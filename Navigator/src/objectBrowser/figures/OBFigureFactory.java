package objectBrowser.figures;

import objectBrowser.model.CompositeDataObject;
import objectBrowser.model.DataObject;
import objectBrowser.model.DiagramNode;
import objectBrowser.model.ReferenceDataObject;
import objectBrowser.model.Relationship;
import objectBrowser.model.ResultNode;
import objectBrowser.model.SimpleDataObject;

import org.eclipse.draw2d.Ellipse;
import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.PolylineDecoration;
import org.eclipse.draw2d.RotatableDecoration;
import org.eclipse.swt.graphics.Color;

public class OBFigureFactory {
	public Figure createFigure(DiagramNode object) {
		if (object instanceof SimpleDataObject) {
			SimpleDataObjectFigure figure = new SimpleDataObjectFigure();
			figure.updateValue((SimpleDataObject)object);
			return figure;
		} else if (object instanceof CompositeDataObject) {
			CompositeDataObjectFigure figure = new CompositeDataObjectFigure();
			return figure;
		} else if (object instanceof ReferenceDataObject) {
			ReferenceDataObjectFigure figure = new ReferenceDataObjectFigure();
			return figure;			
		} else if (object instanceof ResultNode) {
			Ellipse figure = new ResultNodeFigure();
			return figure;
		} else return new Label("<UNKNOWN OBJECT>");
	}
	
	public static PolylineConnection createRelationConnection(Relationship rel){
		PolylineConnection conn = new PolylineConnection();

		PolylineDecoration arrow = new PolylineDecoration();
		arrow.setTemplate(PolylineDecoration.TRIANGLE_TIP);
		arrow.setScale(5,2.5);
		conn.setTargetDecoration(arrow);
		return conn;
	}
}