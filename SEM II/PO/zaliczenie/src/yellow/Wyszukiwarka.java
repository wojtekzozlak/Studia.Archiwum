package yellow;

import java.util.ArrayList;
import java.util.TreeSet;
import java.util.Collections;

public class Wyszukiwarka {
	private Plan plan;
	
	public String znajdzIWypiszOdNajkrotszej(Kryteria k){
		kryteriumAssert(k);
		ArrayList<Trasa> trasy = znajdz(k);
		Collections.sort(trasy, new TrasaDlugoscComparator());
		return wypisz(trasy);
	}
	
	public String znajdzIWypiszOdNajszybszej(Kryteria k){
		kryteriumAssert(k);
		ArrayList<Trasa> trasy = znajdz(k);
		Collections.sort(trasy, new TrasaCzasComparator());
		return wypisz(trasy);
	}
	
	public String znajdzIWypiszOdNajlatwiejszej(Kryteria k){
		kryteriumAssert(k);
		ArrayList<Trasa> trasy = znajdz(k);
		Collections.sort(trasy, new TrasaTrudnoscComparator());
		return wypisz(trasy);
	}
	
	public void kryteriumAssert(Kryteria k){
		if(plan.dajPunkt(k.dajPoczatkowy()) == null || plan.dajPunkt(k.dajKoncowy()) == null)
			throw new BrakPunktu();
		if(k.dajPrzez() != null) for(String p: k.dajPrzez()){
			if(plan.dajPunkt(p) == null) throw new BrakPunktu();
		}
	}
	
	public Wyszukiwarka(Plan plan){
		this.plan = plan;
	}
	
	public ArrayList<Trasa> znajdz(Kryteria k){
		TreeSet<OdcinekSzlaku> odwiedzone = new TreeSet<OdcinekSzlaku>();
		Trasa trasa = new Trasa();
		ArrayList<Trasa> zgromadzone = new ArrayList<Trasa>();
		Punkt punktStartowy = this.plan.dajPunkt(k.dajPoczatkowy());
		
		backtrack(punktStartowy, trasa, odwiedzone, zgromadzone, k);
				
		return zgromadzone;
	}
	
	private String wypisz(ArrayList<Trasa> zgromadzone){
		if(zgromadzone.size() == 0) return "Brak tras spelniajacych podane kryteria\n";
		StringBuffer str = new StringBuffer("");
		int nr = 1;
		for(Trasa t : zgromadzone){
			str.append("Trasa " + nr + ": ");
			str.append("(");
			str.append(t.dajCzas().toString());
			str.append(", ");
			str.append(t.dajDlugosc());
			str.append(" m)\n");
			str.append(t.toString());
			if(nr != zgromadzone.size()) str.append("\n");
			nr++;
		}
		return str.toString();
	}
	
	private void backtrack(Punkt tuJestem, Trasa tedyPrzyszedlem, TreeSet<OdcinekSzlaku> odwiedzone,
			ArrayList<Trasa> zgromadzone, Kryteria k){
		if(!k.czyZaDlugo(tedyPrzyszedlem.dajCzas())){
			if(k.czyPunktKoncowy(tuJestem) && k.czyDobraTrasa(tedyPrzyszedlem))
				zgromadzone.add(new Trasa(tedyPrzyszedlem));
			for(OdcinekSzlaku sz: tuJestem.dajOdcinki()){
				if(!odwiedzone.contains(sz) && k.czyDobrySzlak(sz)){
					odwiedzone.add(sz);
					tedyPrzyszedlem.dodajSzlak(sz);
					backtrack(sz.dajDocelowy(), tedyPrzyszedlem, odwiedzone, zgromadzone, k);
				}
			}
		}
		if(tedyPrzyszedlem.dajOdcinki().size() > 0) odwiedzone.remove(tedyPrzyszedlem.usunOstatni());
	}
	
	
}
