package pl.edu.mimuw.loxim.jdbc;


import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Collection;
import java.util.List;

import org.jmock.Expectations;
import org.jmock.Mockery;
import org.jmock.integration.junit4.JMock;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

import pl.edu.mimuw.loxim.data.Binding;
import pl.edu.mimuw.loxim.data.BindingImpl;
import pl.edu.mimuw.loxim.data.Struct;
import pl.edu.mimuw.loxim.data.StructImpl;

import static org.junit.Assert.*;

@RunWith(JMock.class)
public class LoXiMResultSetImplTest {

	private final Mockery mock = new Mockery();
	private List<Object> list;
	private LoXiMResultSet result;

	@Before
	public void setUp() throws SQLException {
		list = new ArrayList<Object>();
		final LoXiMStatement statement = mock.mock(LoXiMStatement.class);
		mock.checking(new Expectations() {{
			oneOf(statement).getFetchSize();
			will(returnValue(0));
		}});
		result = new LoXiMResultSetImpl(statement, list);
	}
	
	@Test
	public void testSimpleObjects() throws SQLException {
		list.add(1);
		list.add("asdf");
		list.add(1L);
		list.add(1);
		Calendar cal = Calendar.getInstance();
		cal.set(2008, 5, 13, 12, 0);
		list.add(cal);
		assertTrue(result.isBeforeFirst());
		assertTrue(result.next());
		assertEquals(1, result.getInt(1));
		testException(0);
		testException(2);
		assertTrue(result.next());
		assertEquals("asdf", result.getString(1));
		testException(0);
		testException(2);
		assertTrue(result.next());
		assertEquals(1, result.getLong(1));
		testException(0);
		testException(2);
		assertTrue(result.next());
		assertEquals(true, result.getBoolean(1));
		testException(0);
		testException(2);
		assertTrue(result.next());
		assertTrue(result.isLast());
		assertEquals(cal.getTime(), result.getDate(1));
		testException(0);
		testException(2);
		assertFalse(result.next());
		assertTrue(result.isAfterLast());
	}
	
	@Test
	public void testBindings() throws SQLException {
		Binding b = new BindingImpl();
		b.setBindingName("a");
		b.setValue(1);
		list.add(b);
		Struct s = new StructImpl();
		Collection<Object> d = s.getData();
		d.add(2);
		Binding b1 = new BindingImpl();
		b1.setBindingName("kilof");
		b1.setValue("bbb");
		d.add(b1);
		list.add(s);
		s = new StructImpl();
		d = s.getData();
		b1 = new BindingImpl();
		b1.setBindingName("firstName");
		b1.setValue("Adam");
		d.add(b1);
		b1 = new BindingImpl();
		b1.setBindingName("firstName");
		b1.setValue("Jan");
		d.add(b1);
		b1 = new BindingImpl();
		b1.setBindingName("firstName");
		b1.setValue("Ryszard");
		d.add(b1);
		list.add(s);
		assertTrue(result.isBeforeFirst());
		assertTrue(result.next());
		assertEquals(1, result.getInt(1));
		assertEquals(1, result.getInt("a"));
		assertNull(result.getObject("lala"));
		assertEquals(0, result.getInt("lala"));
		testException(0);
		testException(2);
		assertTrue(result.next());
		assertEquals(2, result.getInt(1));
		assertEquals("bbb", result.getString(2));
		assertEquals("bbb", result.getString("kilof"));
		assertEquals("bbb", result.getObject("kilof"));
		assertEquals(Arrays.asList("bbb"), result.getObjects("kilof"));
		testException(0);
		testException(3);
		assertTrue(result.next());
		assertEquals("Adam", result.getString(1));
		assertEquals("Jan", result.getString(2));
		assertEquals("Ryszard", result.getString(3));
		assertEquals(Arrays.asList("Adam", "Jan", "Ryszard"), result.getObject("firstName"));
		assertEquals(Arrays.asList("Adam", "Jan", "Ryszard"), result.getObjects("firstName"));
		assertNull(result.getObject("lastName"));
		assertTrue(result.getObjects("lastName").isEmpty());
		try {
			System.out.println(result.getString("firstName"));
			fail();
		} catch (SQLException e) {
			// should be thrown
		}
		testException(0);
		testException(4);
		assertFalse(result.next());
		assertTrue(result.isAfterLast());
	}
	
	private void testException(int column) {
		try {
			result.getObject(column);
			fail();
		} catch (SQLException e) {
			// should be thrown
		}
	}
}
