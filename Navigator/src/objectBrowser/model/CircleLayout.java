package objectBrowser.model;

import org.eclipse.draw2d.geometry.Point;

public class CircleLayout {
	public void makeLayout(Diagram diagram) {
		ResultNode resultNode = null;
		for (DiagramNode child : diagram.getObjects()) {
			if (child instanceof ResultNode) resultNode = (ResultNode)child;
		}
		
		resultNode.setLocation(new Point(400, 300));
		
		double angle = 0;

		int count = diagram.getObjects().size() - 1;
		
		for (DiagramNode child : diagram.getObjects()) {
			if (!child.equals(resultNode)) {  
				angle = angle + 2 * Math.PI / count;
				double newX = 400 + Math.sin(angle) * 280;
				double newY = 300 + Math.cos(angle) * 280;
				child.setLocation(new Point(newX, newY));
			}
		}
		
		
	}
}
