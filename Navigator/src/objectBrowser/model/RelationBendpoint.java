package objectBrowser.model;

import java.io.Serializable;

import org.eclipse.draw2d.Bendpoint;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

public class RelationBendpoint implements Serializable, Bendpoint {

	private float weight = 0.5f;
	private Dimension d1, d2;

	public RelationBendpoint() {}

	public Dimension getFirstRelativeDimension() {
		return d1;
	}

	public Point getLocation() {
		return null;
	}

	public Dimension getSecondRelativeDimension() {
		return d2;
	}

	public float getWeight() {
		return weight;
	}

	public void setRelativeDimensions(Dimension dim1, Dimension dim2) {
		d1 = dim1;
		d2 = dim2;
	}

	public void setWeight(float w) {
		weight = w;
	}

}
