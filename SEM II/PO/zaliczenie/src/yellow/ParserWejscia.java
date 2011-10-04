package yellow;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Scanner;
import java.util.EnumSet;
import java.util.regex.MatchResult;
import java.io.File;
import java.io.FileNotFoundException;

public class ParserWejscia {
	public static Wyszukiwarka produkujWyszukiwarke(String nazwa){
		Scanner sc;
		String inLinia;
		ArrayList<Punkt> punkty = new ArrayList<Punkt>();
		ArrayList<OdcinekSzlaku> odcinki = new ArrayList<OdcinekSzlaku>();
		int i = 0;

		
		try{
			sc = new Scanner(new File(nazwa));

			try{
				i = 1;				
				while(sc.hasNextLine()){
					inLinia = sc.nextLine();
					if(inLinia.trim().equals("")) break;
					
					Scanner lSc = new Scanner(inLinia);
					try{
						lSc.findInLine("^\\s*(.+?)\\s*;\\s*(.+?)\\s*;\\s*(.*?)\\s*$");
						MatchResult res = lSc.match();
						punkty.add(new Punkt(res.group(1), Integer.parseInt(res.group(2)),
									(res.group(3) == null ? "" : res.group(3))));
					} catch(IllegalStateException e) {
						throw new ZleDaneWejsciowe("Nie dosc danych, punkt " + i);
					} catch(NumberFormatException e){
						throw new ZleDaneWejsciowe("Zly format liczby, punkt " + i);
					} finally {
						lSc.close();
					}						
					i++;
				}
				Collections.sort(punkty);
					
				i = 1;
				while(sc.hasNextLine()){
					inLinia = sc.nextLine();

					Scanner lSc = new Scanner(inLinia);
					try {
						lSc.findInLine("^\\s*([^;]+?)\\s*;\\s*([^;]+?)\\s*;\\s*([^;]+?)\\s*;\\s*([^;]+?)\\s*;\\s*([^;]+?)\\s*;\\s*([^;]+?)\\s*;\\s*([^;]+?)\\s*;\\s*([^;]*?)\\s*;\\s*([^;]*?)\\s*$");
						MatchResult res = lSc.match();
						Punkt punktStartowy = punkty.get(Collections.binarySearch(punkty, new Punkt(res.group(1))));
						Punkt punktDocelowy = punkty.get(Collections.binarySearch(punkty, new Punkt(res.group(2))));
						int dlugosc = Integer.parseInt(res.group(3));
						if(dlugosc < 0) throw new ZleDaneWejsciowe("Zla dlugosc, odcinek " + i);
						String czasPrzejscia = res.group(4);
						if((new Czas(czasPrzejscia)).compareTo(new Czas("0'")) < 0) throw new ZleDaneWejsciowe("Zly czas, odcinek " + i);
						int przewyzszenie = Integer.parseInt(res.group(5));
						if(przewyzszenie < 0) throw new ZleDaneWejsciowe("Zle przewyzszenie, odcinek " + i);
						short trudnosc = Short.parseShort(res.group(6));
						if(trudnosc > 3 || trudnosc < 1) throw new ZleDaneWejsciowe("Zla trudnosc, odcinek " + i);
						String kolor = res.group(7);
						String[] niebezpieczenstwa = res.group(8).split("\\s*,\\s*");
						String[] zabezpieczenia = res.group(9).split("\\s*,\\s*");
						OdcinekSzlaku odcinek = new OdcinekSzlaku(punktStartowy, punktDocelowy, dlugosc, czasPrzejscia, przewyzszenie, trudnosc, kolor,
								setNiebezpieczenstw(niebezpieczenstwa), setZabezpieczen(zabezpieczenia)) ;
						odcinek.ustawHasz(i);
						odcinki.add(odcinek);
						punktStartowy.dodajOdcinek(odcinek);
					} catch(IllegalStateException e){
						throw new ZleDaneWejsciowe("Nie dosc danych, odcinek " + i);
					} catch(NumberFormatException e){
						throw new ZleDaneWejsciowe("Zly format liczby, odcinek " + i);
					} catch(IndexOutOfBoundsException e){
						throw new ZleDaneWejsciowe("Nieistniejacy punkt, odcinek " + i);
					} catch(ZlyFormatCzasu e){
						throw new ZleDaneWejsciowe("Zly format czasu, odcinek " + i);
					} catch(NieznaneNiebezpieczenstwo e){
						throw new ZleDaneWejsciowe("Nieznane niebezpieczenstwo, odcinek " + i);
					} catch(NieznaneZabezpieczenie e){
						throw new ZleDaneWejsciowe("Nieznane zabezpieczenie, odcinek " + i);
					}finally {
						lSc.close();
					}
					i++;
				}
			} finally {
				sc.close();
			}
			
		} catch(FileNotFoundException e) {
			throw new ZleDaneWejsciowe("Brak pliku wejsciowego");
		}
		
		Plan plan = new Plan(punkty, odcinki);
		
		return new Wyszukiwarka(plan);
	}
	
	private static EnumSet<Zabezpieczenia> setZabezpieczen(String[] zabezpieczenia){
		EnumSet<Zabezpieczenia> set = EnumSet.noneOf(Zabezpieczenia.class);
		if(zabezpieczenia.length != 1 || !zabezpieczenia[0].trim().equals(""))
			for(String z : zabezpieczenia){
				if(z.equals("lancuchy")) set.add(Zabezpieczenia.LANCUCHY);
				else if(z.equals("klamry")) set.add(Zabezpieczenia.KLAMRY);
				else if(z.equals("drabinki")) set.add(Zabezpieczenia.DRABINKI);
				else throw new NieznaneZabezpieczenie();
			}
		return set;
	}
	
	private static EnumSet<Niebezpieczenstwa> setNiebezpieczenstw(String[] niebezpieczenstwa){
		EnumSet<Niebezpieczenstwa> set = EnumSet.noneOf(Niebezpieczenstwa.class);
		if(niebezpieczenstwa.length != 1 || !niebezpieczenstwa[0].trim().equals(""))
			for(String n : niebezpieczenstwa){
				if(n.equals("przepascie")) set.add(Niebezpieczenstwa.PRZEPASCIE);
				else if(n.equals("ekspozycja")) set.add(Niebezpieczenstwa.EKSPOZYCJA);
				else if(n.equals("stromizny")) set.add(Niebezpieczenstwa.STROMIZNY);
				else if(n.equals("niedzwiedzie")) set.add(Niebezpieczenstwa.NIEDZWIEDZIE);
				else throw new NieznaneNiebezpieczenstwo();
			}
		return set;
	}
}

class NieznaneZabezpieczenie extends RuntimeException {}
class NieznaneNiebezpieczenstwo extends RuntimeException {}