package pl.edu.mimuw.loxim.protogen.lang.java.template.auth;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

public class AuthPassMySQL {

	public static byte[] encodePassword(String password, String salt) throws AuthException {
		try {
			return encodePasswordFromShaP(sha1(password), salt.getBytes());
		} catch (UnsupportedEncodingException e) {
			throw new AuthException(e);
		} catch (NoSuchAlgorithmException e) {
			throw new AuthException(e);
		}
	}

	public static byte[] encodePassword(String password, byte[] salt) throws AuthException {
		try {
			return encodePasswordFromShaP(sha1(password), salt);
		} catch (UnsupportedEncodingException e) {
			throw new AuthException(e);
		} catch (NoSuchAlgorithmException e) {
			throw new AuthException(e);
		}
	}

	public static byte[] encodePassword(byte[] password, byte[] salt) throws AuthException {
		try {
			return encodePasswordFromShaP(sha1(password), salt);
		} catch (NoSuchAlgorithmException e) {
			throw new AuthException(e);
		}
	}

	public static byte[] encodePasswordFromShaP(byte[] sha1_password, byte[] salt) throws AuthException {
		try {
			/* sha1(sha1(password)) */
			byte[] sha1_sha1_password = sha1(sha1_password);

			/* sha1(salt.sha1(sha1(password))) */
			byte[] salt_sha1_sha1_password = sha1(concat(salt, sha1_sha1_password));
			return xorb(sha1_password, salt_sha1_sha1_password);
		} catch (NoSuchAlgorithmException e) {
			throw new AuthException(e);
		}
	}

	private static byte[] concat(byte[] a, byte[] b) {
		byte[] res = new byte[a.length + b.length];
		for (int i = 0; i < a.length; i++)
			res[i] = a[i];
		for (int i = 0; i < b.length; i++)
			res[i + a.length] = b[i];
		return res;
	}

	private static byte[] xorb(byte[] a, byte[] b) {
		if (a.length != b.length)
			throw new IllegalArgumentException();
		byte[] res = new byte[a.length];
		for (int i = 0; i < a.length; i++) {
			res[i] = (byte) ((a[i]) ^ (b[i]));
		}
		return res;
	}

	static byte[] sha1(String password) throws NoSuchAlgorithmException, UnsupportedEncodingException {
		return sha1(password.getBytes("utf-8"));
	}

	static byte[] sha1(byte[] password) throws NoSuchAlgorithmException {
		MessageDigest sha1 = MessageDigest.getInstance("SHA-1");
		return sha1.digest(password);
	}

	public static boolean authorize(byte[] password, byte[] salt, byte[] sha1) {
		try {
			return Arrays.equals(encodePassword(password, salt), encodePasswordFromShaP(sha1, salt));
		} catch (AuthException e) {
			return false;
		}
	}

	public static boolean authorize(String password, byte[] salt, byte[] sha1) {
		try {
			return Arrays.equals(encodePassword(password, salt), encodePasswordFromShaP(sha1, salt));
		} catch (AuthException e) {
			return false;
		}
	}

}
