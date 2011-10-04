package yellow;

import java.util.EnumSet;

public class OdcinekSzlaku implements Comparable<OdcinekSzlaku>{
	private Punkt punktStartowy;
	private Punkt punktDocelowy;
	private int dlugosc;
	private Czas czasPrzejscia;
	private int przewyzszenie;
	private short trudnosc;
	private String kolor;
	private EnumSet<Niebezpieczenstwa> niebezpieczenstwa;
	private EnumSet<Zabezpieczenia> zabezpieczenia;
	private int hasz;
	
	public String dajKolor(){
		return kolor;
	}
	
	public int dajDlugosc(){
		return dlugosc;
	}
	
	public Czas dajCzasPrzejscia(){
		return czasPrzejscia;
	}
	
	public short dajTrudnosc(){
		return trudnosc;
	}
	
	public Punkt dajStartowy(){
		return punktStartowy;
	}
	
	public Punkt dajDocelowy(){
		return punktDocelowy;
	}
	
	public int dajPrzewyzszenie(){
		return przewyzszenie;
	}
	
	public EnumSet<Niebezpieczenstwa> dajNiebezpieczenstwa(){
		return niebezpieczenstwa;
	}
	
	public EnumSet<Zabezpieczenia> dajZabezpieczenia(){
		return zabezpieczenia;
	}
	
	public OdcinekSzlaku(Punkt punktStartowy, Punkt punktDocelowy, int dlugosc, String czasPrzejscia, int przewyzszenie,
			short trudnosc, String kolor, EnumSet<Niebezpieczenstwa> niebezpieczenstwa, EnumSet<Zabezpieczenia> zabezpieczenia ){
		this.punktStartowy = punktStartowy;
		this.punktDocelowy = punktDocelowy;
		this.dlugosc = dlugosc;
		this.czasPrzejscia = new Czas(czasPrzejscia);
		this.przewyzszenie = przewyzszenie;
		this.trudnosc = trudnosc;
		this.kolor = kolor;
		this.niebezpieczenstwa = niebezpieczenstwa;
		this.zabezpieczenia = zabezpieczenia;
	}
	
	public String toString(){
		StringBuffer str = new StringBuffer("");
		str.append("szlak ");
		str.append(kolor);
		str.append(", ");
		str.append(czasPrzejscia.toString());
		str.append(", ");
		str.append(dlugosc);
		str.append(" m, ");
		str.append(przewyzszenie);
		str.append(" m w gore, trudnosc ");
		str.append(trudnosc);
		if(!niebezpieczenstwa.isEmpty() || !zabezpieczenia.isEmpty()){
			str.append(" (");
			if(!zabezpieczenia.isEmpty()){
				String s = zabezpieczenia.toString().toLowerCase();
				str.append(s.substring(1, s.length()-1));
			}
			if(!niebezpieczenstwa.isEmpty() && !zabezpieczenia.isEmpty()) str.append(", ");
			if(!niebezpieczenstwa.isEmpty()){
				String s = niebezpieczenstwa.toString().toLowerCase();
				str.append(s.substring(1, s.length()-1));
			}
			str.append(")");
		}
		return str.toString();
	}
	
	public int compareTo(OdcinekSzlaku b){
		if(this.hasz > b.hasz) return 1;
		else if(this.hasz < b.hasz) return -1;
		else return 0;
	}
	
	public int dajHasz(){
		return hasz;
	}
	
	public void ustawHasz(int hasz){
		this.hasz = hasz;
	}
}