#include <Indexes/OptimizedField.h>
#include <assert.h>
#include <vector>
#include <Indexes/IndexHandler.h>

namespace Indexes {

	OptimizedField::OptimizedField(IndexHandler* ih) {
		exact = NULL;
		greaterThan = smallerThan = NULL;
		boundsRejected = false;
		this->ih = ih;
	}
	
	OptimizedField::~OptimizedField() {}
	
	bool OptimizedField::someBoundsRejected() {
		return boundsRejected;
	}
	
	vector<TreeNode*> OptimizedField::getUsedBounds() {
		return usedBounds;
	}
	
	int OptimizedField::howRestrictive() {
		return exact ? 10 : usedBounds.size();
	}
	
	IndexSelectNode* OptimizedField::getSelectNode() {
		IndexSelectNode* result;
		if (exact) {			
			result = exact;
		} else
		if (greaterThan && smallerThan) {
			result = new IndexSelectNode(greaterThan, smallerThan);
		} else
		if (greaterThan) {
			result = new IndexSelectNode(greaterThan, false);
		} else
		if (smallerThan) {
			result = new IndexSelectNode(false, smallerThan);
		} else {
			//nie moze byc wolane jesli optymalizacja nie jest mozliwa
			assert(false);
		}
		result->setIndexName(ih->getName());
		return result;
	}
	
	bool OptimizedField::addBound(QueryNode* value, AlgOpNode::algOp boundType) {
		//jesli mamy juz = to pomin
		if (exact) {
			return false;
		}
		bool inclusive = ((boundType == AlgOpNode::ge) || (boundType == AlgOpNode::le));
		
		switch (boundType) {
			case AlgOpNode::eq:
				exact = new IndexSelectNode((QueryNode*)value->clone());
				boundsRejected = greaterThan || smallerThan;
				if (greaterThan) {delete greaterThan;}
				if (smallerThan) {delete smallerThan;}
				usedBounds.clear();
				usedBounds.push_back(value);
				return true;
			case AlgOpNode::gt:
			case AlgOpNode::ge:
				if (greaterThan) {
					return false;
				}
				greaterThan = new IndexBoundaryNode(inclusive, (QueryNode*)value->clone());
				usedBounds.push_back(value);
				return true;
			case AlgOpNode::lt:
			case AlgOpNode::le:
				if (smallerThan) {
					return false;
				}
				smallerThan = new IndexBoundaryNode(inclusive, (QueryNode*)value->clone());
				usedBounds.push_back(value);
				return true;
			default:
				assert(false);
		}	
	}

}
