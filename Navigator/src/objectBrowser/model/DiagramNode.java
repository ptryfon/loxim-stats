package objectBrowser.model;


import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.util.LinkedList;
import java.util.List;

import objectBrowser.editors.DimensionPropertySource;
import objectBrowser.editors.LocationPropertySource;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.NodeEditPart;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.PropertyDescriptor;

public abstract class DiagramNode extends DiagramElement {
	private Point location;
	public List<Relationship> inputs = new LinkedList<Relationship>();
	public List<Relationship> outputs = new LinkedList<Relationship>();
	

	
	public static final String EVENT_INPUTS = "Inputs"; //$NON-NLS-1$
	public static final String EVENT_OUTPUTS = "Outputs"; //$NON-NLS-1$
	
	
	
	protected static IPropertyDescriptor[] descriptors = null;
		

	static{
		descriptors = new IPropertyDescriptor[]{
			new PropertyDescriptor(PROP_SIZE, "Size"),
			new PropertyDescriptor(PROP_LOCATION, "Location")
		};
	}	

	public Point getLocation() {
		return location;
	}

	public void setLocation(Point location) {
		this.location = location;
		firePropertyChange(PROP_LOCATION, null, location);  //$NON-NLS-1$
	}
	

	


	public Object getPropertyValue(Object propName){		
		if (PROP_SIZE.equals(propName))
			//return new DimensionPropertySource(getSize());
			return null;
		else if( PROP_LOCATION.equals(propName))
			return new LocationPropertySource(getLocation());		
		return super.getPropertyValue(propName);
	}


	
	/**
	 * Returns useful property descriptors for the use
	 * in property sheets. this supports location and
	 * size.
	 *
	 * @return  Array of property descriptors.
	 */
	public IPropertyDescriptor[] getPropertyDescriptors() {
		return descriptors;
	}	
	

	public void setPropertyValue(Object propName, Object val){		
		if (PROP_SIZE.equals(propName)) {
			//setSize((Dimension)val);
		} else if (PROP_LOCATION.equals(propName)) {
			setLocation((Point)val);
		} else super.setPropertyValue(propName, val);
	}
	
	final void setPropertyValue(String propName, Object val){}
	
	public void connectInput(Relationship w) {
		inputs.add(w);
		update();
		fireStructureChange(EVENT_INPUTS, w);
	}

	public void connectOutput(Relationship w) {
		outputs.add(w);
		update();
		fireStructureChange(EVENT_OUTPUTS, w);
	}

	public void disconnectInput(Relationship w) {
		inputs.remove(w);
		update();
		fireStructureChange(EVENT_INPUTS,w);
	}

	public void disconnectOutput(Relationship w) {
		outputs.remove(w);
		update();
		fireStructureChange(EVENT_OUTPUTS,w);
	}	
	
	public List<Relationship> getInputs() {
		return inputs;
	}
	
	public List<Relationship> getOutputs() {
		return outputs;
	}

	public void update(){
	}	
}
