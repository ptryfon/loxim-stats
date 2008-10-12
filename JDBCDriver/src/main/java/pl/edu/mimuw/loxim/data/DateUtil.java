package pl.edu.mimuw.loxim.data;

import java.util.Calendar;
import java.util.GregorianCalendar;

public final class DateUtil {

    private static Calendar tempCal        = new GregorianCalendar();
    private static Calendar tempCalDefault = new GregorianCalendar();
	
	private DateUtil() {

	}

	public static void resetToDate(Calendar cal) {
		cal.set(Calendar.HOUR_OF_DAY, 0);
		cal.set(Calendar.MINUTE, 0);
		cal.set(Calendar.SECOND, 0);
		cal.set(Calendar.MILLISECOND, 0);
	}

	public static void resetToTime(Calendar cal) {
		cal.set(Calendar.YEAR, 1970);
		cal.set(Calendar.MONTH, 0);
		cal.set(Calendar.DATE, 1);
		cal.set(Calendar.MILLISECOND, 0);
	}

	public static long getTimeInMillis(java.util.Date dt, Calendar source, Calendar target) {

		if (source == null) {
			source = tempCalDefault;
		}

		if (target == null) {
			target = tempCalDefault;
		}

		synchronized (tempCal) {
			tempCal.setTimeZone(source.getTimeZone());
			tempCal.setTime(dt);
			tempCal.setTimeZone(target.getTimeZone());

			return tempCal.getTimeInMillis();
		}
	}
}
