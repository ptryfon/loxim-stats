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
package objectBrowser.model.commands;

import objectBrowser.model.RelationBendpoint;

import org.eclipse.draw2d.Bendpoint;


public class MoveBendpointCommand
	extends BendpointCommand 
{

private Bendpoint oldBendpoint;

public void execute() {
	RelationBendpoint bp = new RelationBendpoint();
	bp.setRelativeDimensions(getFirstRelativeDimension(), 
					getSecondRelativeDimension());
	setOldBendpoint((Bendpoint)getRelationship().getBendpoints().get(getIndex()));
	getRelationship().setBendpoint(getIndex(), bp);
	super.execute();
}

protected Bendpoint getOldBendpoint() {
	return oldBendpoint;
}

public void setOldBendpoint(Bendpoint bp) {
	oldBendpoint = bp;
}

public void undo() {
	super.undo();
	getRelationship().setBendpoint(getIndex(), getOldBendpoint());
}

}


