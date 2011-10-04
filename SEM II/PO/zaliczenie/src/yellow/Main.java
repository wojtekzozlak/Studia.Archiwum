package yellow;

public class Main {
	public static Wyszukiwarka wczytajDane(String nazwa) throws ZleDaneWejsciowe{
		return ParserWejscia.produkujWyszukiwarke(nazwa);
	}
	
	public static void main(String[] args){
		String nazwaPliku = "yt16.txt";
		
		Wyszukiwarka w = wczytajDane(nazwaPliku);
		
		String[] przez = {"Kalatowki", "Kondracka Przelecz"};
		Zabezpieczenia[] z = {Zabezpieczenia.LANCUCHY};
		Niebezpieczenstwa[] n = {Niebezpieczenstwa.NIEDZWIEDZIE, Niebezpieczenstwa.STROMIZNY, Niebezpieczenstwa.EKSPOZYCJA};
		
		//Kryteria k = new Kryteria("Kuznice", "Kalatowki", null, 4, new Double(2), null, null, null);
		Kryteria k = new Kryteria("Kuznice", "Giewont", null, 0.2, null, null, null, null);
	
		System.out.println(w.znajdzIWypiszOdNajkrotszej(k));
		
	}
}