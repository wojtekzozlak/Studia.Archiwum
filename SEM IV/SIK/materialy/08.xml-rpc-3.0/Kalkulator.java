import java.util.Hashtable;

public class Kalkulator {
    public Hashtable sumAndDifference (int x, int y) {
	Hashtable result = new Hashtable();
	result.put("sum", new Integer(x + y));
	result.put("difference", new Integer(x - y));
	return result;
    }
}
