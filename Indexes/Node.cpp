#include <assert.h>
#include "Node.h"
#include "ToStringBuilder.h"

namespace Indexes {
	
	char* Node::afterHeader() {
		return ((char*) this) + sizeof (Node);
	}
	
	void Node::init() {
		this->left = NODE_SIZE - sizeof(Node);
		//cleanNeeded = false;
	}
	
	nodeAddress_t Node::init(nodeAddress_t id) {
		nodeAddress_t nextDroppedNode = this->nextNode; 
		this->id = id;
		init();
		return nextDroppedNode;
	}
	
	char* Node::afterAddr() {
		int offset = NODE_SIZE - this->left;
		return ((char*) this) + offset;
	}
	
	nodeSize_t Node::distanceInNode(void* smaller, void* grather) {
		return ((char*) grather) - ((char*)smaller);
	}
	
	string Node::headerToString() {
		string union2 = leaf ? "nextLeaf" : "firstChild";
		return ToStringBuilder("Node", this)
			.append("id", id)
			.append("leaf", leaf)
			.append(union2, nextLeaf)
			.append("left", left)
			.toString();
	}
	
	nodeSize_t Node::size() {
		return MAX_NODE_CAPACITY - left;
	}
	
	string NodeEntry::toString() {
		return ToStringBuilder("NodeEntry", this)
			.append("child", child)
			.append("lid", logicalID)
			.toString();
	}
}
