package yellow;

import java.util.ArrayList;
import java.util.Collections;

public class Plan {
	private ArrayList<Punkt> punkty;
	private ArrayList<OdcinekSzlaku> odcinki;
	
	public Punkt dajPunkt(String nazwa){
		try {
		  int indeks = Collections.binarySearch(punkty, new Punkt(nazwa));
		  return punkty.get(indeks); 
		} catch(IndexOutOfBoundsException e){
			return null;
		}
	}
	
	public ArrayList<Punkt> dajPunkty(){
		return punkty;
	}
	
	public ArrayList<OdcinekSzlaku> dajOdcinki(){
		return odcinki;
	}
	
	public Plan(ArrayList<Punkt> punkty, ArrayList<OdcinekSzlaku> odcinki){
		this.punkty = punkty;
		this.odcinki = odcinki;
	}
	
	public void wypisz(){
		for(int i=0; i<punkty.size(); i++) System.out.print(punkty.get(i).toString());
		System.out.println();
		for(int i=0; i<odcinki.size(); i++) System.out.print(odcinki.get(i).toString());
	}
}