#include <assert.h>
#include <Indexes/CachedNode.h>
#include <Indexes/IndexHandler.h>

namespace Indexes {

//class IndexHandler;
	
	int CachedNode::getNode(Node* &result) {
		int err = 0;
		mutex->down();
		assert(users > 0); //do testow, flushNode pobiera wezel mimo ze nikt go nie uzywa 
		if (!loaded) {
			node = (Node*) malloc(NODE_SIZE);
			if (node == NULL) {
				err = errno | ErrIndexes;
				goto fail;
			}
			if ((err = IndexHandler::readNode(node, nodeID))) {
				goto fail;
			}
			loaded = true;
		}
		fail:
		mutex->up();
		result = node;
		return err;
	}
	
	int CachedNode::getNode(Node* &result, int priority, NodeAccessMode mode) {
		if (mode == NODE_READ) {
			sem->lock_read();
		} else {
			sem->lock_write();
		}
		depth = priority;
		int err = 0;
		err = getNode(result);
		if (result->level != priority) {
			assert(mode == NODE_WRITE);
			assert(dirty);
			result->level = priority;  
		}
		return err;
	}
	
	int CachedNode::tryGetNode(Node* &result, int priority) {
		int err;
		if ((err = sem->try_lock_write())) {
			return err;
		}
		depth = priority;
		return getNode(result);
	}
	
	Node* CachedNode::getNode() {
		assert(loaded);
		assert(users > 0); //do testow, flushNode pobiera wezel mimo ze nikt go nie uzywa 
		return node;
	}
	
	void CachedNode::setDirty() {
		dirty = true;
	}
	
	int CachedNode::parentPriority() {
		return depth + 1;
	}
	
	int CachedNode::siblingPriority() {
		return depth;
	}
	
	int CachedNode::childPriority() {
		return depth - 1;
	}
	
	void CachedNode::init(nodeAddress_t nodeID) {
		this->nodeID = nodeID; 
		users = 0;
		node = NULL;
		mutex = new Mutex();
		mutex->init();
		sem = new RWUJSemaphore();
		sem->init();
		dirty = false;
	}
	
	int CachedNode::unlock() {
		return sem->unlock();
	}
	
	CachedNode::CachedNode(nodeAddress_t nodeID) {
		init(nodeID); 
		loaded = false;
	}
	
	CachedNode::CachedNode(Node* node) {
		init(node->id);
		this->node = node;
		loaded = true;
	}
	
	nodeAddress_t CachedNode::getNodeID() {
		if (node) {
			return node->id;
		} else {
			return nodeID;
		}	
	}
	
	CachedNode::~CachedNode() {
		delete mutex;
		delete sem;
		if (node) {
			free(node);
		}
	}
}
