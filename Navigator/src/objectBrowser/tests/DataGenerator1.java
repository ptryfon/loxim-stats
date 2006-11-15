package objectBrowser.tests;

import objectBrowser.model.Diagram;

public class DataGenerator1 {
	public Diagram makeSample() {
		Diagram pool = new Diagram();
		/*
		CompositeDataObject o1 = new CompositeDataObject("czlowiek");
		o1.getChildren().add(new SimpleDataObject("imie", "Jan"));
		o1.getChildren().add(new SimpleDataObject("nazwisko", "Kowalski"));		
		pool.getObjects().add(o1);
		
		CompositeDataObject but1 = new CompositeDataObject("but");
		but1.getChildren().add(new SimpleDataObject("rodzaj", "lewy"));
		but1.getChildren().add(new SimpleDataObject("kolor", "niebieski"));
		
		o1.getChildren().add(but1);

		CompositeDataObject but2 = new CompositeDataObject("but");
		but2.getChildren().add(new SimpleDataObject("rodzaj", "prawy"));
		but2.getChildren().add(new SimpleDataObject("kolor", "zielony"));
		
		o1.getChildren().add(but2);
		
		
		CompositeDataObject o2 = new CompositeDataObject("czlowiek");
		o2.getChildren().add(new SimpleDataObject("imie", "Boles�aw"));
		o2.getChildren().add(new SimpleDataObject("nazwisko", "Chrobry"));		
		pool.getObjects().add(o2);
		
		ResultNode rn = new ResultNode();
		
		pool.getObjects().add(rn);
		
		Relationship r1 = new Relationship();
		
		rn.connectInput(r1);
		o1.connectOutput(r1);
		
		Relationship r2 = new Relationship();
		
		rn.connectInput(r2);
		o2.connectOutput(r2);
		*/
		return pool;
	}
}
