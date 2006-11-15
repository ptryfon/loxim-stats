package objectBrowser.model;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

import org.eclipse.draw2d.geometry.Point;

public class CircleLayout {
	
	private Diagram diagram;
	private Set<DiagramNode> allocated;
	private Set<DiagramNode> calculated;
	
	public CircleLayout(Diagram diagram) {
		this.diagram = diagram;
	}
	
	private int calcSize(DiagramNode node) {
		if (calculated.contains(node)) return 0;
		calculated.add(node);		
		
		
		int result = 1;
		for (Relationship relationship: node.getOutputs()) {			
			DiagramNode childNode = relationship.getTarget();
			result = Math.max(result, calcSize(childNode) + 1);
		}
		
		if (node instanceof CompositeDataObject) {
			for (DataObject childNode : ((CompositeDataObject)node).getChildren()) {
				result = Math.max(result, calcSize(childNode) + 1);
			}
		}		
		return result;
	}
	
	private List<DiagramNode> getRelated(DiagramNode node) {
		List<DiagramNode> result = new LinkedList<DiagramNode>();
		for (Relationship relationship: node.getOutputs()) {
			result.add(relationship.getTarget());
		}
		
		if (node instanceof CompositeDataObject) {
			for (DataObject item : ((CompositeDataObject)node).getChildren()) {
				result.addAll(getRelated(item));
			}
		}
		return result;
	}
	
	private void resizeLeft() {
		Point topLeft = null;
		for (DiagramNode node : diagram.getObjects()) {
			Point p = node.getLocation();
			if (topLeft == null) {
				topLeft = new Point(p);
			} else {
				topLeft = new Point(Math.min(p.x, topLeft.x), Math.min(p.y, topLeft.y));
			}
		}
		
		for (DiagramNode node : diagram.getObjects()) {
			node.setLocation(new Point(node.getLocation().x - topLeft.x + 30, node.getLocation().y - topLeft.y + 30));
		}		
	}
	
	
	private void makeLayout(DiagramNode node, double x, double y, double angle, double range, double size) {
		if (allocated.contains(node)) return;
		allocated.add(node);
		node.setLocation(new Point(x, y));
		
		double newAngle = angle - (range / 2);
		int number = 0;
		
		List<DiagramNode> related = this.getRelated(node);
		int count = related.size();
		
		for (DiagramNode childNode: related) {
			double currentAngle = newAngle + range * number / count;
			double newX = x + Math.sin(currentAngle) * size;
			double newY = y + Math.cos(currentAngle) * size;
			makeLayout(childNode, newX, newY, currentAngle, angle / count, 200);
			number++;
		}
		
	}
	public void makeLayout() {
		int x = 50;
		int y = 50;
		
		calculated = new HashSet<DiagramNode>();
		allocated = new HashSet<DiagramNode>();
		
		for (ResultNode node : diagram.getResultNodes()) {
			int nodeSize = calcSize(node); 
			y = y + 200 * nodeSize;
			x = 200 * nodeSize;
			makeLayout(node, x, y, 0, 2 * Math.PI, 300);
			y = y + 200 * nodeSize;
		}
			
		resizeLeft();
	}
}
