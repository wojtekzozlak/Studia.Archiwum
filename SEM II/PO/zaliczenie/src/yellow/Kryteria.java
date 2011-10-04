package yellow;

import java.util.EnumSet;

public class Kryteria {
	private String punktPoczatkowy;
	private String punktKoncowy;
	private String[] przez;
	private Czas czasDo;
	private Czas czasOd;
	private Integer nieTrudniejNiz;
	private EnumSet<Zabezpieczenia> dopuszczalneZabezpieczenia = null;
	private EnumSet<Niebezpieczenstwa> dopuszczalneNiebezpieczenstwa = null;
	
	public Kryteria(String punktPoczatkowy, String punktKoncowy, String[] przez, double czasDo, Double czasOd,
			Integer nieTrudniejNiz, Zabezpieczenia[] dopuszczalneZabezpieczenia, Niebezpieczenstwa[] dopuszczalneNiebezpieczenstwa){
		if(punktPoczatkowy == null) throw new NieWszystkieWymagane();
		if(punktKoncowy == null) throw new NieWszystkieWymagane();
		this.punktPoczatkowy = punktPoczatkowy;
		this.punktKoncowy = punktKoncowy;
		if(przez != null) this.przez = przez;
		if(czasDo < 0) throw new BlednyCzas();
		this.czasDo = new Czas(czasDo);
		if(czasOd != null)
			if(czasOd > czasDo || czasOd < 0) throw new BlednyCzas(); 
			else this.czasOd = new Czas(czasOd);
		if(nieTrudniejNiz != null)
			if(nieTrudniejNiz > 3 || nieTrudniejNiz < 1) throw new ZlaTrudnosc();
			else this.nieTrudniejNiz = nieTrudniejNiz;
		if(dopuszczalneZabezpieczenia != null) ustawZabezpieczenia(dopuszczalneZabezpieczenia);
		if(dopuszczalneNiebezpieczenstwa != null) ustawNiebezpieczenstwa(dopuszczalneNiebezpieczenstwa);
	}
	
	private void ustawZabezpieczenia(Zabezpieczenia[] zabezpieczenia){
		dopuszczalneZabezpieczenia = EnumSet.noneOf(Zabezpieczenia.class);
		for(Zabezpieczenia z : zabezpieczenia)
			dopuszczalneZabezpieczenia.add(z);
	}
	
	private void ustawNiebezpieczenstwa(Niebezpieczenstwa[] niebezpieczenstwa){
		dopuszczalneNiebezpieczenstwa = EnumSet.noneOf(Niebezpieczenstwa.class);
		for(Niebezpieczenstwa n : niebezpieczenstwa)
			dopuszczalneNiebezpieczenstwa.add(n);
	}
	
	public boolean czyDobrySzlak(OdcinekSzlaku szlak){
		if(nieTrudniejNiz != null && szlak.dajTrudnosc() > nieTrudniejNiz) return false;
		if(dopuszczalneNiebezpieczenstwa != null)
			for(Niebezpieczenstwa n : szlak.dajNiebezpieczenstwa())
				if(!dopuszczalneNiebezpieczenstwa.contains(n)) return false;
		if(dopuszczalneZabezpieczenia != null)
			for(Zabezpieczenia z : szlak.dajZabezpieczenia())
				if(!dopuszczalneZabezpieczenia.contains(z)) return false;
		return true;
	}
	
	public boolean czyDobraTrasa(Trasa trasa){
		if(trasa.dajCzas().compareTo(czasDo) > 0) return false;
		if(czasOd != null && trasa.dajCzas().compareTo(czasOd) < 0) return false;
		boolean maPunkty = true;
		if(przez != null){ 
			int i = 0;
			int j = 0;
			
			for( ; i<trasa.dajOdcinki().size(); i++){
				if(trasa.dajOdcinki().get(i).dajStartowy().dajNazwe().equals(przez[j]) ||
						trasa.dajOdcinki().get(i).dajDocelowy().dajNazwe().equals(przez[j])){
					j++;
					if(j >= przez.length) break;
				}
			}
			if(i >= trasa.dajOdcinki().size()) maPunkty = false;
		}
		if(!maPunkty) return false;
		return true;
	}

	public String dajPoczatkowy(){
		return punktPoczatkowy;
	}
	
	public String dajKoncowy(){
		return punktKoncowy;
	}
	
	public String[] dajPrzez(){
		return przez;
	}
	
	public boolean czyPunktKoncowy(Punkt p){
		return p.dajNazwe().equals(punktKoncowy);
	}
	
	public boolean czyZaDlugo(Czas c){
		if(c.compareTo(czasDo) > 0){
			return true;
		}
		return false;
	}
}
