#ifndef ADMIN_TREE_NODE_H
#define ADMIN_TREE_NODE_H

namespace AdminParser{
    class AdminTreeNode{
	public:
	    virtual bool is_executable() = 0;
	    virtual ~AdminTreeNode() = 0;
    };
}

#endif
