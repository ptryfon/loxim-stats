package pl.edu.mimuw.loxim.protogen.lang.java.template.auth;

import static org.junit.Assert.*;

import java.io.UnsupportedEncodingException;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;

import org.junit.Test;

public class AuthPassMySQLTest {

	@Test
	public void testAuth1() throws NoSuchAlgorithmException, UnsupportedEncodingException, AuthException
	{
		byte[] salt="172371893".getBytes();
		String password="Bardzo fajne has這";
		
		byte[] salt2=salt;
		String password2=password;
		assertArrayEquals(AuthPassMySQL.encodePassword(password,salt),AuthPassMySQL.encodePasswordFromShaP(AuthPassMySQL.sha1(password2),  salt2));		
	}
	
	@Test
	public void testAuth2Fail() throws NoSuchAlgorithmException, UnsupportedEncodingException, AuthException
	{
		byte[] salt="1231231231".getBytes();
		String password="aaa";
		
		byte[] salt2=salt;
		String password2=password;
		assertArrayEquals(AuthPassMySQL.encodePassword(password,salt),AuthPassMySQL.encodePasswordFromShaP(AuthPassMySQL.sha1(password2),  salt2));		
	}
	
	@Test
	public void testAuth3Fail() throws NoSuchAlgorithmException, UnsupportedEncodingException, AuthException
	{
		byte[] salt="172371893".getBytes();
		String password="Bardzo fajne has這";
		
		byte[] salt2="172371893 ".getBytes();
		String password2=password;
		assertFalse(Arrays.equals(AuthPassMySQL.encodePassword(password,salt),AuthPassMySQL.encodePasswordFromShaP(AuthPassMySQL.sha1(password2),  salt2)));		
	}
	
	@Test
	public void testAuth4Fail() throws NoSuchAlgorithmException, UnsupportedEncodingException, AuthException
	{
		byte[] salt="172371893".getBytes();
		String password="Bardzo fajne has這";
		
		byte[] salt2=salt;
		String password2=" Bardzo fajne has這";
		assertFalse(Arrays.equals(AuthPassMySQL.encodePassword(password,salt),AuthPassMySQL.encodePasswordFromShaP(AuthPassMySQL.sha1(password2),  salt2)));		
	}
	
	@Test
	public void testAuthorize() throws NoSuchAlgorithmException, UnsupportedEncodingException
	{
		assertTrue(AuthPassMySQL.authorize("abc", "12345".getBytes(), AuthPassMySQL.sha1("abc")));
	}

	@Test
	public void testAuthorize2() throws NoSuchAlgorithmException, UnsupportedEncodingException
	{
		assertFalse(AuthPassMySQL.authorize("abc", "12345".getBytes(), AuthPassMySQL.sha1("ab")));
	}
	

	

}
