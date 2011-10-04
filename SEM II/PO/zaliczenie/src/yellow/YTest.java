package yellow;

import static org.junit.Assert.*;

import org.junit.Test; 
public class YTest {
	
	@Test (expected = Exception.class)
	public void YTest1() throws Exception{
		// brak punktu z odcinka
		Wyszukiwarka w = Main.wczytajDane("yt1.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest2() throws Exception{
		// brak linii oddzielaj¹cej out
		Wyszukiwarka w = Main.wczytajDane("yt2.txt");
	}

	@Test (expected = Exception.class)
	public void YTest3() throws Exception{
		// pusta nazwa punktu
		Wyszukiwarka w = Main.wczytajDane("yt3.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest4() throws Exception{
		// zla wysokosc w punkcie
		Wyszukiwarka w = Main.wczytajDane("yt4.txt");
	}

	@Test (expected = Exception.class)
	public void YTest5() throws Exception{
		// zla wysokosc w odcinku
		Wyszukiwarka w = Main.wczytajDane("yt5.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest6() throws Exception{
		// zla forma godziny
		Wyszukiwarka w = Main.wczytajDane("yt6.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest7() throws Exception{
		// ujemne przewyzszenie
		Wyszukiwarka w = Main.wczytajDane("yt7.txt");
	}

	@Test (expected = Exception.class)
	public void YTest8() throws Exception{
		// trudnosc ponad skala
		Wyszukiwarka w = Main.wczytajDane("yt8.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest9() throws Exception{
		// trudnosc pod skala
		Wyszukiwarka w = Main.wczytajDane("yt9.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest10() throws Exception{
		// nieznane niebezpieczenstwo
		Wyszukiwarka w = Main.wczytajDane("yt10.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest11() throws Exception{
		// nieznane zabezpieczenie
		Wyszukiwarka w = Main.wczytajDane("yt11.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest12() throws Exception{
		// ujemna dlugosc odcinka
		Wyszukiwarka w = Main.wczytajDane("yt12.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest13() throws Exception{
		// ujemne minuty
		Wyszukiwarka w = Main.wczytajDane("yt13.txt");
	}
	
	@Test (expected = Exception.class)
	public void YTest14() throws Exception{
		// ujemne godziny
		Wyszukiwarka w = Main.wczytajDane("yt14.txt");
	}
	
	@Test
	public void YTest15() throws Exception{
		// opis punktu z ";"
		Wyszukiwarka w = Main.wczytajDane("yt15.txt");
		Kryteria k = new Kryteria("Kuznice", "Kalatowki", null, 0.2, null, null, null, null);
		
		assertEquals("Trasa 1: (10', 1650 m)\n" +
"Kuznice (1000 m) [opis zawierajacy ;] ->\n" +
"szlak niebieski, 10', 1650 m, 200 m w gore, trudnosc 1\n" +
"Kalatowki (1200 m) [schronisko]\n", w.znajdzIWypiszOdNajkrotszej(k));
	}
	
	@Test
	public void YTest16() throws Exception{
		// trimowanie wejscia
		Wyszukiwarka w = Main.wczytajDane("yt16.txt");
		Kryteria k = new Kryteria("Kuznice", "Kalatowki", null, 0.2, null, null, null, null);
		
		assertEquals("Trasa 1: (10', 1650 m)\n" +
"Kuznice (1000 m) [opis] ->\n" +
"szlak niebieski, 10', 1650 m, 200 m w gore, trudnosc 1\n" +
"Kalatowki (1200 m) [schronisko]\n", w.znajdzIWypiszOdNajkrotszej(k));
	}
} 