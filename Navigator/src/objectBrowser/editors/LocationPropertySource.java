/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package objectBrowser.editors;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

import org.eclipse.draw2d.geometry.Point;


public class LocationPropertySource
	implements IPropertySource{

public static String PROP_XPOS = "xPos"; //$NON-NLS-1$
public static String PROP_YPOS = "yPos"; //$NON-NLS-1$
protected static IPropertyDescriptor[] descriptors;

static{
	PropertyDescriptor xProp =
		new TextPropertyDescriptor(PROP_XPOS,
			"x");
	xProp.setValidator(LogicNumberCellEditorValidator.instance());
	PropertyDescriptor yProp = 
		new TextPropertyDescriptor(PROP_YPOS,
			"y");
	yProp.setValidator(LogicNumberCellEditorValidator.instance());
	descriptors = new IPropertyDescriptor[] {xProp, yProp};
}

protected Point point = null;

public LocationPropertySource(Point point){
	this.point = point.getCopy();
}

public Object getEditableValue(){
	return point.getCopy();
}

public IPropertyDescriptor[] getPropertyDescriptors(){
	return descriptors;
}

public Object getPropertyValue(Object propName){
	if(PROP_XPOS.equals(propName)){
		return new String(new Integer(point.x).toString());
	}
	if(PROP_YPOS.equals(propName)){
		return new String(new Integer(point.y).toString());
	}
	return null;
}

public boolean isPropertySet(Object propName){
	return PROP_XPOS.equals(propName) || PROP_YPOS.equals(propName);
}

public void resetPropertyValue(Object propName){}

public void setPropertyValue(Object propName, Object value){
	if(PROP_XPOS.equals(propName)){
		Integer newInt = new Integer((String)value);
		point.x = newInt.intValue();
	}
	if(PROP_YPOS.equals(propName)){
		Integer newInt = new Integer((String)value);
		point.y = newInt.intValue();
	}
}

public String toString(){
	return new String("["+point.x+","+point.y+"]");//$NON-NLS-3$//$NON-NLS-2$//$NON-NLS-1$
}

}
