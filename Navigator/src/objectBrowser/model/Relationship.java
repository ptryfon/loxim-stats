package objectBrowser.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.eclipse.draw2d.Bendpoint;
import org.eclipse.ui.views.properties.IPropertyDescriptor;

public class Relationship extends DiagramElement {
	
	protected List<Bendpoint> bendpoints = new ArrayList<Bendpoint>();
	
	protected DiagramNode source;
	protected DiagramNode target;
	
	public static final String PROP_CONNECTION_ROUTER = "ConnectionRouter";

	public IPropertyDescriptor[] getPropertyDescriptors() {
		return new IPropertyDescriptor[0];
	}

	public DiagramNode getSource() {
		return source;
	}

	public void setSource(DiagramNode source) {
		this.source = source;
	}

	public DiagramNode getTarget() {
		return target;
	}

	public void setTarget(DiagramNode target) {
		this.target = target;
	}		

	
	public void attachSource(){
		if (getSource() == null || getSource().getOutputs().contains(this))
			return;
		getSource().connectOutput(this);
	}

	public void attachTarget(){
		if (getTarget() == null || getTarget().getInputs().contains(this))
			return;
		getTarget().connectInput(this);
	}

	public void detachSource(){
		if (getSource() == null)
			return;
		getSource().disconnectOutput(this);
	}

	public void detachTarget(){
		if (getTarget() == null)
			return;
		getTarget().disconnectInput(this);
	}
	
	public void insertBendpoint(int index, Bendpoint point) {
		getBendpoints().add(index, point);
		firePropertyChange("bendpoint", null, null);//$NON-NLS-1$
	}

	public void removeBendpoint(int index) {
		getBendpoints().remove(index);
		firePropertyChange("bendpoint", null, null);//$NON-NLS-1$
	}

	public void setBendpoint(int index, Bendpoint point) {
		getBendpoints().set(index, point);
		firePropertyChange("bendpoint", null, null);//$NON-NLS-1$
	}

	public void setBendpoints(Vector points) {
		bendpoints = points;
		firePropertyChange("bendpoint", null, null);//$NON-NLS-1$
	}
	
	public List getBendpoints() {
		return bendpoints;
	}	
}
