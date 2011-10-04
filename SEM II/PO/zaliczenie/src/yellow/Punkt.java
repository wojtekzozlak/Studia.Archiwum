package yellow;

import java.util.ArrayList;

public class Punkt implements Comparable<Punkt> {
	private String nazwa;
	private int wysokosc;
	private String opis;
	private ArrayList<OdcinekSzlaku> odcinki = new ArrayList<OdcinekSzlaku>();
	
	public int dajWysokosc(){
		return wysokosc;
	}
	
	public String dajNazwe(){
		return nazwa;
	}
	
	public String dajOpis(){
		return opis;
	}
	
	public ArrayList<OdcinekSzlaku> dajOdcinki(){
		return odcinki;
	}

	public Punkt(String nazwa){
		this.nazwa = nazwa;
	}
	
	public Punkt(String nazwa, int wysokosc, String opis){
		this.nazwa = nazwa;
		this.wysokosc = wysokosc;
		this.opis = opis;
	}
	
	public int compareTo(Punkt b){
		return dajNazwe().compareTo(b.dajNazwe());
	}
	
	public void dodajOdcinek(OdcinekSzlaku s){
		odcinki.add(s);
	}
	
	public String toString(){
		StringBuffer str = new StringBuffer("");
		str.append(nazwa);
		str.append(" (");
		str.append(wysokosc);
		str.append(" m)");
		if(!opis.equals("")) str.append(" [").append(opis).append("]");
		return str.toString();
	}
	
}