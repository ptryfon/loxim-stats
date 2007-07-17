package pl.tzr.transparent.data;

import java.util.Set;

import pl.tzr.transparent.structure.annotation.ComponentSet;
import pl.tzr.transparent.structure.annotation.Node;
import pl.tzr.transparent.structure.annotation.OnRemoveAction;
import pl.tzr.transparent.structure.annotation.Persistent;
import pl.tzr.transparent.structure.annotation.PrimitiveSet;
import pl.tzr.transparent.structure.annotation.ReferenceSet;

@Node(nodeName="PART")
public interface Part {

	int getDetailCost();
	
	void setDetailCost(int detailCost);

	int getDetailMass();

	void setDetailMass(int detailMass);

	String getKind();

	void setKind(String kind);

	String getName();

	void setName(String name);
    
    @Persistent(nodeName="otherName")
    @PrimitiveSet(itemType=String.class)
    Set<String> getOtherNames();
    
    void setOtherNames(Set<String> otherNames);

    @Persistent(nodeName="component2")
	@ComponentSet(itemType=Component.class)
	Set<Component> getComponent();

	void setComponent(Set<Component> component);
	
    @Persistent(nodeName="single")
    @pl.tzr.transparent.structure.annotation.Component(onRemove=OnRemoveAction.DELETE)
	Component getSingleComponent();

	void setSingleComponent(Component singleComponent);
    
    
    @Persistent(nodeName="component3")
    @ReferenceSet(itemType=Component.class)
    Set<Component> getReferenceComponent();
    
    public void setReferenceComponent(Set<Component> component);

}