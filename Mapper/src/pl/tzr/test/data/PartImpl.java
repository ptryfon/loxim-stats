package pl.tzr.test.data;

import java.util.HashSet;
import java.util.Set;

public class PartImpl implements Part {

    String name;
    
	String kind;
    
	int detailCost;
    
	int detailMass;
	
	Set<Component> component = new HashSet<Component>();
    
    Set<Component> referenceComponent = new HashSet<Component>();
    
    Set<String> otherNames = new HashSet<String>();
	
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

    public Set<Component> getReferenceComponent() {
        return referenceComponent;
    }

    public void setReferenceComponent(Set<Component> component) {
        referenceComponent = component;
    }    
	
    public Set<String> getOtherNames() {
        return otherNames;
    }

    public void setOtherNames(Set<String> otherNames) {
        this.otherNames = otherNames;
    }    
}
