package yellow;

import java.util.ArrayList;
import java.util.Comparator;

public class Trasa {
	private Czas czas = new Czas();
	private int dlugosc = 0;
	private ArrayList<OdcinekSzlaku> trasa = new ArrayList<OdcinekSzlaku>();
	private int[] trudnosci = new int[3]; 
	
	public Trasa(){}
	
	public Trasa(Trasa trasa){
		this.czas = new Czas(trasa.czas);
		this.dlugosc = trasa.dlugosc;
		this.trasa = new ArrayList<OdcinekSzlaku>(trasa.trasa);
		this.trudnosci = trasa.trudnosci.clone();
	}
	
	public void dodajSzlak(OdcinekSzlaku s){
		trasa.add(s);
		czas.dodaj(s.dajCzasPrzejscia());
		dlugosc += s.dajDlugosc();
		trudnosci[3-s.dajTrudnosc()]++;
	}
	
	public OdcinekSzlaku usunOstatni(){
		int i = trasa.size() - 1;
		czas.odejmij(trasa.get(i).dajCzasPrzejscia());
		dlugosc -= trasa.get(i).dajDlugosc();
		trudnosci[3-trasa.get(i).dajTrudnosc()]--;
		return trasa.remove(i);
	}
	
	public Czas dajCzas(){
		return czas;
	}
	
	public int dajDlugosc(){
		return dlugosc;
	}
	
	public int[] dajTrudnosc(){
		return trudnosci;
	}
	
	public ArrayList<OdcinekSzlaku> dajOdcinki(){
		return trasa;
	}
	
	public String toString(){
		StringBuffer str = new StringBuffer("");
		for(OdcinekSzlaku sz: trasa){
			str.append(sz.dajStartowy().toString()).append(" ->\n");
			str.append(sz.toString()).append("\n");
		}
		str.append(trasa.get(trasa.size()-1).dajDocelowy().toString()).append("\n");
		return str.toString();
	}
}

class TrasaDlugoscComparator implements Comparator<Trasa>{
	public int compare(Trasa a, Trasa b){
		if(a.dajDlugosc() > b.dajDlugosc()) return 1;
		else if(a.dajDlugosc() < b.dajDlugosc()) return -1;
		else return a.toString().compareTo(b.toString());
	}
}

class TrasaCzasComparator implements Comparator<Trasa>{
	public int compare(Trasa a, Trasa b){
		int cmp = a.dajCzas().compareTo(b.dajCzas());
		if(cmp != 0) return cmp;
		else return a.toString().compareTo(b.toString());
	}
}

class TrasaTrudnoscComparator implements Comparator<Trasa>{
	public int compare(Trasa a, Trasa b){
		int[] trudA = a.dajTrudnosc();
		int[] trudB = b.dajTrudnosc();
		if(trudA[0] > trudB[0]) return 1;
		else if(trudA[0] < trudB[0]) return -1;
		else if(trudA[1] > trudB[1]) return 1;
		else if(trudA[1] < trudB[1]) return -1;
		else if(trudA[2] > trudB[2]) return 1;
		else if(trudA[2] < trudB[2]) return -1;
		else return a.toString().compareTo(b.toString());
	}
}