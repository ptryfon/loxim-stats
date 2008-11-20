#include <protocol/auth/AuthPassMySQL.h>
#include <protocol/auth/sha1.h>

#include <stdlib.h>
#include <string.h>

using namespace protocol;

char* xorb(char* l1, char* l2, int size)
{
	char *res=(char*) malloc(size);
	
	for(int i=0; i<size; i++)
	{
		res[i]= (l1[i])^(l2[i]);
	};
	
	return res;
};

char* salt_sha1(char* salt, int salt_len, char* part2, int part2_len)
{
	char* buf=(char*)malloc(salt_len+part2_len);
	memcpy(buf,salt,salt_len);
	memcpy(buf+salt_len,part2,part2_len);
	char* res=sha1(buf,salt_len+part2_len);
	free(buf);
	return res;
}


CharArray* protocol::encodePassword(CharArray *password,char* salt)
{
	char* sha1_password=sha1(password); //sha1(password)
	
	CharArray *res=encodePasswordFromShaP(sha1_password,salt);
	free(sha1_password);
	
	return res;
}

CharArray* protocol::encodePasswordFromShaP(char *sha1_password,char* salt)
{
	char* sha1_sha1_password=sha1(sha1_password); //sha1(sha1(password))
	
	// sha1(salt.sha1(sha1(password)))
	char* salt_sha1_sha1_password=salt_sha1(salt,20,sha1_sha1_password,20); 
	char* res=xorb(sha1_password,salt_sha1_sha1_password,20);
	
	free(salt_sha1_sha1_password);
	free(sha1_sha1_password);
	return new CharArray(res,20);
}

