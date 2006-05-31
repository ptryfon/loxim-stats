#include "Misc.h"
#include <iostream>

using namespace std;

namespace Store
{
	void Misc::MemoryCheck()
	{
		cout << "Checking memory..\n";
		for(int s=1; s<1000; s++) {
			char* b = new char[s];
			for(int i=0; i<s; i++) {
				b[i] = 0;
				if( b[i] == 0) continue;
			}
			delete[] b;
			s++; s++;
		}
		cout << "Memory OK\n";
	}

	void Misc::MemoryCheck(void* by, unsigned s)
	{
		cout << "Checking memory..\n";
		char* b = (char*)by;
		cout << "Checking memory ptr..\n";
		for(int i=0; i<s; i++) {
			if( b[i] == 0) continue;
		}
		cout << "Memory OK\n";
	}
	
	


	Serialized::Serialized()
	{
		bytes = NULL;
		size = 0;
		realsize = 0;
	}

	Serialized::Serialized(int psize)
	{
		bytes = new unsigned char[psize];
		realsize = size = psize;
	}
	
	Serialized::Serialized(const Serialized& s)
	{
		bytes = new unsigned char[realsize = size = s.size];
		for(int i=0; i<size; i++) bytes[i] = s.bytes[i];
	}
	
	Serialized& Serialized::operator=(const Serialized& s)
	{s.info();
		if(this != &s) {
			if(bytes) delete[] bytes;
			bytes = new unsigned char[realsize = size = s.size];
			for(int i=0; i<size; i++) bytes[i] = s.bytes[i];
		}
		return *this;
	}
	
	Serialized::~Serialized()
	{
		if(bytes) delete[] bytes;
	}

	Serialized& Serialized::operator+=(const Serialized& s)
	{
		if(s.size > 0) {
			if(realsize-size < s.size) {
				unsigned char *newbin = new unsigned char[realsize=(size+s.size)*2];
				for(int i=0; i<size; i++) newbin[i] = bytes[i];
				for(int i=0; i<s.size; i++) newbin[size+i] = s.bytes[i];
				if(bytes) delete[] bytes;
				bytes = newbin;
			} else {
				for(int i=0; i<s.size; i++) bytes[size+i] = s.bytes[i];				
			}
			size += s.size;
		}
		return *this;
	}

	Serialized& Serialized::operator+=(const int& s)
	{
		return ((*this) += p_baseTypeSerialize(s));
	}

	Serialized& Serialized::operator+=(const double& s)
	{
		return ((*this) += p_baseTypeSerialize(s));
	}

	Serialized& Serialized::operator+=(const string& s)
	{
		int slen = s.length();
		Serialized z(sizeof(int)+slen);
		*(reinterpret_cast<int*>(z.bytes)) = slen;
		for(int i=0; i<slen; i++) z.bytes[i+sizeof(int)] = s[i];
		return ((*this) += z);
	}

	Serialized& Serialized::operator+=(const LogicalID& s)
	{
		return ((*this) += s.serialize());
	}

	Serialized& Serialized::operator+=(const DataValue& s)
	{
		return ((*this) += s.serialize());
	}

// dlaczego ten template nie dziala O_O?
	template <typename T> Serialized& Serialized::operator+=(const T& s)
	{
		return ((*this) += s.serialize());
	}

	template <typename T> Serialized Serialized::p_baseTypeSerialize(const T& s)
	{
		Serialized z(sizeof(T));
		*(reinterpret_cast<T*>(z.bytes)) = s;
		return z;
	}

	void Serialized::info() const
	{
		cout << "SRLZobj: size: " <<size<<"/"<<realsize<<endl;
	}

}
