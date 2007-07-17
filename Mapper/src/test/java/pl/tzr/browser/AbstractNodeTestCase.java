package pl.tzr.browser;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import junit.framework.TestCase;
import pl.tzr.browser.session.LoximSession;
import pl.tzr.browser.session.Session;
import pl.tzr.driver.loxim.LoximDatasourceImpl;
import pl.tzr.driver.loxim.SimpleConnection;

public class AbstractNodeTestCase extends TestCase {

    protected LoximDatasourceImpl datasource;

    protected Session session;

    @Override
    protected final void setUp() throws Exception {

        /* Create a datasource */
        datasource = new LoximDatasourceImpl();
        datasource.setHost("127.0.0.1");
        datasource.setPort(6543);
        datasource.setLogin("root");
        datasource.setPassword("");

        /* Create a browsing session */
        session = new LoximSession(datasource);
        
        ((LoximSession)session).getConnection().execute("delete PART");

    }

    @Override
    protected final void tearDown() throws Exception {
        session.rollback();
    }

    private String readQuery(BufferedReader reader) throws IOException {
        
        StringBuffer query = new StringBuffer();
        
        String line;
        
        while ((line=reader.readLine()) != null) {
            
            String trimmedLine = line.trim();
            
            if (trimmedLine.endsWith("/")) {
                if (trimmedLine.length() > 1) {
                    query.append(trimmedLine.substring(0, trimmedLine.length() - 2));
                }
                return query.toString();
            } else {
                
                query.append(trimmedLine);
                query.append(" ");
            }
            
                           
        }
        
        return null;
        
    }

    protected void prepareData(String inputResource) throws Exception {
        
        SimpleConnection currentConnetion = 
            ((LoximSession)session).getConnection();
        
                
        File file = new File(getClass().getResource(inputResource).toURI());                
        
        BufferedReader reader = 
            new BufferedReader(new FileReader(file));
        
        try {
        
            String query;
            while ((query = readQuery(reader)) != null) {
                currentConnetion.execute(query);
                
            }
        
        } finally {
        
            reader.close();
        
        }
        
    }

}
