#ifndef CONST_H_
#define CONST_H_

#include <algorithm>

#include <Indexes/Node.h>
#include <Indexes/RootEntry.h>

namespace Indexes {

	//#define INDEX_DEBUG_MODE
	
	const unsigned int MAX_CACHED_NODES	= 16;
	const int NODE_SIZE	= 8192;
	const unsigned int NEW_NODE = UINT_MAX;
	
	/** maksymalna ilosc znakow jaka bedzie uzywana podczas porownywania stringow w indeksach */
	const int MAX_STRING_LEN = 255;
	
	struct RootEntry;
	struct NodeEntry;
	struct Node;

	/**  maksymalny adres wezla. zeby nie przekroczyc zakresu */
	const unsigned int MAX_NODE_ADDR = (UINT_MAX - NODE_SIZE - 1);
	
	const int MAX_INDEX_STRUCT_SIZE = (max(sizeof(RootEntry), sizeof(NodeEntry)));
	
	const int MAX_NODE_CAPACITY	= (NODE_SIZE - sizeof(Node));

	

}

#endif /*CONST_H_*/
