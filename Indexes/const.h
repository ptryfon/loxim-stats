#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <algorithm>

namespace Indexes {

	//#define INDEX_DEBUG_MODE
	
	/** maksymalna liczba scache'owanych wezlow */
	extern const unsigned int MAX_CACHED_NODES;
	extern const int NODE_SIZE;
	extern const unsigned int NEW_NODE;
	
	extern const int MAX_STRING_LEN;
	
	extern const int MAX_INDEX_STRUCT_SIZE;
	
	extern const int MAX_NODE_CAPACITY;
	
	//extern const int MIN_NODE_SIZE_ALLOWED;

	/**  maksymalny adres wezla. zeby nie przekroczyc zakresu */
	extern const unsigned int MAX_NODE_ADDR;
		
	typedef unsigned int lid_t;				/** LogicalID */
	typedef int ts_t;						/** timestamp */
	typedef int tid_t;						/** transaction id */
	
	const tid_t ROLLED_BACK = -2;
	const tid_t BEYOND_HORIZON = -3;
		
	// typy danych dla drzew
	//zamias int wystarczyloby ushort, u int, u short
	typedef int nodeEntry_off_t;	/** odleglosc do nastepnego NodeEntry, rozmiar NodeEntry. ze znakiem na potrzeby testow */
	typedef unsigned int nodeAddress_t;		/** sposob adresowania node'ow na dysku */
	typedef int nodeSize_t;		/** sposob opisu rozmiarow node'a. ze znakiem na potrzeby testow */
	


}

#endif /* __DEBUG_H__ */
