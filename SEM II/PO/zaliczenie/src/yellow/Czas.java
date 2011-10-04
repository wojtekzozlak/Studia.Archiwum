package yellow;

import java.util.Scanner;
import java.util.regex.MatchResult;

public class Czas implements Comparable<Czas>{
	private int czas = 0;
	
	public Czas(){};
	
	public Czas(int czas){
		this.czas = czas;
	}
	
	public Czas(double czas){
		this.czas = (int)(czas*60);
	}
	
	public Czas(Czas czas){
		this.czas = czas.czas;
	}
	
	public Czas(String str){
		Scanner sc = new Scanner(str);
		try{
			sc.findInLine("^((\\d*?)h)?((\\d*?)')?$");
			MatchResult x = sc.match();
			if(x.group(2) != null) czas += Integer.parseInt(x.group(2))*60;
			if(x.group(4) != null) czas += Integer.parseInt(x.group(4));
		}catch(IllegalStateException e){
			throw new ZlyFormatCzasu();
		}finally{
			sc.close();
		}
	}
	
	public void dodaj(Czas c){
		this.czas += c.czas;
	}
	
	public void odejmij(Czas c){
		this.czas -= c.czas;
	}
	
	public int compareTo(Czas c){
		if(this.czas < c.czas) return -1;
		else if(this.czas > c.czas) return 1;
		else return 0;
	}
	
	public String toString(){
		if(czas == 0) return "0'";
		int godziny = czas/60;
		int minuty = czas - (czas/60)*60;
		StringBuffer str = new StringBuffer();
		if(godziny > 0) str.append(godziny + "h");
		if(minuty >= 10) str.append(minuty + "'");
		else if(minuty > 0) str.append("0" + minuty + "'");
		return str.toString();  
	}
}