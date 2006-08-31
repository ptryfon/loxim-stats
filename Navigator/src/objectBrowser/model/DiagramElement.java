package objectBrowser.model;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

import objectBrowser.editors.LocationPropertySource;

import org.eclipse.ui.views.properties.IPropertySource;

public abstract class DiagramElement implements IPropertySource, Cloneable, Serializable {
	public static final String PROP_CHILDREN = "Children"; 	 //$NON-NLS-1$
	public static final String PROP_SIZE = "Size";         //$NON-NLS-1$
	public static final String PROP_LOCATION = "Location"; //$NON-NLS-1$	
	transient protected PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	
	public void addPropertyChangeListener(PropertyChangeListener l){
		listeners.addPropertyChangeListener(l);
	}

	protected void firePropertyChange(String prop, Object old, Object newValue){
		listeners.firePropertyChange(prop, old, newValue);
	}

	protected void fireChildAdded(String prop, Object child, Object index) {
		listeners.firePropertyChange(prop, index, child);
	}

	protected void fireChildRemoved(String prop, Object child) {
		listeners.firePropertyChange(prop, child, null);
	}

	protected void fireStructureChange(String prop, Object child){
		listeners.firePropertyChange(prop, null, child);
	}	

	public Object getPropertyValue(Object propName){		
		return null;
	}

	final Object getPropertyValue(String propName){
		return null;
	}

	public boolean isPropertySet(Object propName){
		return isPropertySet((String)propName);
	}

	final boolean isPropertySet(String propName){
		return true;
	}
	
	private void readObject(ObjectInputStream in)throws IOException, ClassNotFoundException {
		in.defaultReadObject();
		listeners = new PropertyChangeSupport(this);
	}

	public void removePropertyChangeListener(PropertyChangeListener l){
		listeners.removePropertyChangeListener(l);
	}
	
	public void setPropertyValue(Object propName, Object val){		
	}	
	
	final void resetPropertyValue(String propName){}	
	
	public void resetPropertyValue(Object propName){
	}
	
	public Object getEditableValue(){
		return this;
	}		
}
