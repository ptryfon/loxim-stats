package pl.tzr.test.data;

import java.util.HashSet;
import java.util.Set;

public class PartImpl implements Part {

	String name;
	String kind;
	int detailCost;
	int detailMass;
	
	Set<Component> component = new HashSet<Component>();
	
	Component singleComponent;
	
	public int getDetailCost() {
		return detailCost;
	}
	
	public void setDetailCost(int detailCost) {
		this.detailCost = detailCost;
	}
	
	public int getDetailMass() {
		return detailMass;
	}
	
	public void setDetailMass(int detailMass) {
		this.detailMass = detailMass;
	}
	
	public String getKind() {
		return kind;
	}
	
	public void setKind(String kind) {
		this.kind = kind;
	}
	
	public String getName() {
		return name;
	}
	
	public void setName(String name) {
		this.name = name;
	}
	
	

	public Set<Component> getComponent() {
		return component;
	}

	public void setComponent(Set<Component> component) {
		this.component = component;
	}

	public Component getSingleComponent() {
		return singleComponent;
	}

	public void setSingleComponent(Component singleComponent) {
		this.singleComponent = singleComponent;
	}

	
}
