import java.net.InetAddress;
import org.apache.xmlrpc.common.TypeConverterFactoryImpl;
//import org.apache.xmlrpc.demo.webserver.proxy.impls.AdderImpl;
import org.apache.xmlrpc.server.PropertyHandlerMapping;
import org.apache.xmlrpc.server.XmlRpcServer;
import org.apache.xmlrpc.webserver.XmlRpcServlet;
import org.apache.xmlrpc.server.XmlRpcServerConfigImpl;
import org.apache.xmlrpc.webserver.ServletWebServer;
import org.apache.xmlrpc.webserver.WebServer;
	       
public class JavaServer {
    private static final int port = 5763;
   
    public static void main(String[] args) throws Exception {
    WebServer webServer = new WebServer(port);
    XmlRpcServer xmlRpcServer = webServer.getXmlRpcServer();
    PropertyHandlerMapping phm = new PropertyHandlerMapping();

   /* Load handler definitions from a property file.
    *           
    * The property file might look like:
    *                       
    *   Calculator=org.apache.xmlrpc.demo.Calculator
    *           
    *   org.apache.xmlrpc.demo.proxy.Adder=org.apache.xmlrpc.demo.proxy.AdderImpl
    *        
    */
    phm.load(Thread.currentThread().getContextClassLoader(),"moje.properties");
    xmlRpcServer.setHandlerMapping(phm);
    XmlRpcServerConfigImpl serverConfig =
	    (XmlRpcServerConfigImpl) xmlRpcServer.getConfig();
    serverConfig.setEnabledForExtensions(true);
    serverConfig.setContentLengthOptional(false);
    webServer.start();
   }
}
