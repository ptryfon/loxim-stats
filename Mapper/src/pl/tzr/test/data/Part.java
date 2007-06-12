package pl.tzr.test.data;

import java.util.Set;

import pl.tzr.transparent.structure.annotation.PersistentSet;

public interface Part {

	int getDetailCost();
	
	void setDetailCost(int detailCost);

	int getDetailMass();

	void setDetailMass(int detailMass);

	String getKind();

	void setKind(String kind);

	String getName();

	void setName(String name);

	@PersistentSet(itemType=Component.class)
	Set<Component> getComponent();

	void setComponent(Set<Component> component);
	
	Component getSingleComponent();

	void setSingleComponent(Component singleComponent);

}