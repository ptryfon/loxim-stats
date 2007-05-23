package pl.tzr.test;

import java.util.Set;

import pl.tzr.driver.loxim.SimpleConnectionImpl;
import pl.tzr.driver.loxim.TcpConnectionFactory;
import pl.tzr.test.data.Component;
import pl.tzr.test.data.Part;
import pl.tzr.transparent.SimpleTransparentSessionFactoryImpl;
import pl.tzr.transparent.TransparentSession;
import pl.tzr.transparent.TransparentSessionFactory;

public class TransparentTest {

	private static void testTransparentProxying() throws Exception {

		/* Tworzymy polaczenie z baza danych */
		SimpleConnectionImpl connection = new SimpleConnectionImpl(TcpConnectionFactory
				.getConnection("127.0.0.1", 6543), "root", "");
		
		TransparentSessionFactory transparentSessionFactory = new
			SimpleTransparentSessionFactoryImpl(connection, new Class[] { Part.class,
					Component.class });

		try {

			TransparentSession transparentSession = 
				transparentSessionFactory.getCurrentSession();

			Set<Object> results = transparentSession
					.find("Part", Part.class);

			/* Odczytujemy wszystkie elementy typu Part */

			for (Object item : results) {

				Part part = (Part) item;

				System.out.println("Part name=" + part.getName());

				System.out.println("Components of the part:");

				if (part.getComponent() != null) {

					for (Component comp : part.getComponent()) {
						System.out.println("- component leadsTo "
								+ comp.getLeadsTo().getName());
						System.out.println(" -component amount ="
								+ comp.getAmount());

						/* Dokonujemy modyfikacji bazy */
						comp.setAmount(comp.getAmount() + 1);

					}

				}

				System.out.println();

			}

			connection.commitTransation();

		} catch (Exception e) {

			connection.rollbackTransaction();
			connection.close();

			throw e;

		}

	}


	public static void main(String[] args) throws Exception {

		testTransparentProxying();

	}

}
