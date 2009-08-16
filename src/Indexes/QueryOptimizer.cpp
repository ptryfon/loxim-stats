#include <Indexes/QueryOptimizer.h>
#include <QueryParser/TreeNode.h>
#include <assert.h>

namespace Indexes {

	bool QueryOptimizer::implicitIndexCall = false;
	
	QueryOptimizer::QueryOptimizer() {
		firstUpdate = true;
		differentNodeCount = 0;
		bestField = NULL;
	}
	
	QueryOptimizer::~QueryOptimizer() {
	}
	
	AlgOpNode::algOp QueryOptimizer::opposite(AlgOpNode::algOp op) {
		switch (op) {
			case AlgOpNode::eq:
				return AlgOpNode::eq;
			case AlgOpNode::lt:
				return AlgOpNode::gt;
			case AlgOpNode::gt:
				return AlgOpNode::lt;
			case AlgOpNode::le:
				return AlgOpNode::ge;
			case AlgOpNode::ge:
				return AlgOpNode::le;
			default:
				assert(false);
		}
		//not reached
		return AlgOpNode::algOp();
	}
	
	void QueryOptimizer::findPossibleFields(TreeNode* node) {
		//node powinien byc AND'em albo ktoryms z operatorow < > = >= <=
		if (node->type() == TreeNode::TNALGOP) {
			//jesli jestesmy tu pierwszy raz to musi byc AND. mark as good node
			AlgOpNode* algNode = dynamic_cast<AlgOpNode*>(node);
			switch (algNode->getOp()) {
				case AlgOpNode::boolAnd:
					goodNode();
					findPossibleFields(algNode->getLArg());
					findPossibleFields(algNode->getRArg());
					break;
				case AlgOpNode::eq:
				case AlgOpNode::lt:
				case AlgOpNode::gt:
				case AlgOpNode::le:
				case AlgOpNode::ge:
					goodNode();
					checkComparison(algNode->getLArg(), algNode->getRArg(), algNode->getOp());
					checkComparison(algNode->getRArg(), algNode->getLArg(), opposite(algNode->getOp()));
					break;
				default:
					//jesli pierwszy zly to nie ma optymalizacji. jesli nie pierwszy to trzeba dokleic
					differentNode();
					break;
			}
			
		} else {
			//jesli pierwszy zly to nie ma optymalizacji. jesli nie pierwszy to trzeba dokleic
			differentNode();
		}
		
	}
	
	void QueryOptimizer::goodNode() {
		if (firstUpdate) {
			correctNode = true;
			firstUpdate = false;
			return;
		}
		
	}
	
	void QueryOptimizer::differentNode() {
		if (firstUpdate) {
			correctNode = false;
			firstUpdate = false;
		}
		differentNodeCount++;
	}
	
	bool QueryOptimizer::eligibleValue(TreeNode* value){
		//lepiej byloby gdyby tu nie bylo case'a. moze daloby sie sprawdzic jaka konkretnie wartosc jest wymagana. wtedy moznaby zbierac po kilka ograniczen
		switch (value->type()) {
			case TreeNode::TNINT:
			case TreeNode::TNSTRING:
			case TreeNode::TNDOUBLE:
				return true;
			default:
				return false;
		}
	}
	
	void QueryOptimizer::checkComparison(TreeNode* field, TreeNode* value, AlgOpNode::algOp boundType) {
		if ((field->type() == TreeNode::TNNAME) && eligibleValue(value)) {
			NameNode* name = dynamic_cast<NameNode*>(field);
			IndexHandler* ih;
			if (!(ih = IndexManager::getHandle()->getIndexFromField(name->getName(), indexedFields))) {
				differentNode();
				return;
			}
			optimizedFields_t::iterator it = optimizedFields.find(name->getName());
			OptimizedField* oField;
			if (it == optimizedFields.end()) {
				oField = new OptimizedField(ih);
				optimizedFields[name->getName()] = oField; 
			} else {
				oField = it->second;
			}
			
			if (!oField->addBound(dynamic_cast<QueryNode*>(value), boundType)) {
				//jesli ten oField ma juz 2 ograniczenia to nie poszerzac ograniczenia?
				differentNode(); // jesli nie uwzgledniamy tego wystapienia
				return;
			} else {
				if (!bestField || oField->howRestrictive() > bestField->howRestrictive()) {
					bestField = oField;
				}
			}
			
			//jesli ograniczenie z obu stron to w tym algorytmie mozna przerwac dalsze poszukiwania
		}
		
	}
	
	OptimizedField* QueryOptimizer::chooseBestField() {
		return bestField;
	}
	
	bool QueryOptimizer::optimizationPossible() {
		return optimizedFields.size() > 0 && correctNode;
	}
	
	void QueryOptimizer::cutOff(TreeNode* value) {
		AlgOpNode* compOp = dynamic_cast<AlgOpNode*>(value->getParent()); //< > = ...
		AlgOpNode* andOp = dynamic_cast<AlgOpNode*>(compOp->getParent()); // and lub where? raczej tylko and?
		TwoArgsNode* overAnd = dynamic_cast<TwoArgsNode*>(andOp->getParent());
		
		QueryNode* toUp; //w gore trzeba podniesc ten wezel ktory nie jest usuwany
		if (andOp->getLArg() == compOp) {
			toUp = andOp->getRArg();
			andOp->setRArg(new IntNode(0)); //nie da sie wstawic null
		} else {
			toUp = andOp->getLArg();
			andOp->setLArg(new IntNode(0)); //nie da sie wstawic null
		}
		
	//	toUp = dynamic_cast<QueryNode*>(toUp->clone());
	//	toUp->setParent(overAnd);
		
		if (overAnd->getLArg() == andOp) {
			overAnd->setLArg(toUp);
		} else {
			overAnd->setRArg(toUp);
		}
		
		delete andOp;
	}
	
	void QueryOptimizer::createResult(TreeNode* &tree) {
		if (!optimizationPossible()) {
			//nie znaleziono pol ktore mozna wykorzystac do optymalizacji
			return;
		}
		
		OptimizedField* bestField = chooseBestField();
		if (bestField->someBoundsRejected()) {
			differentNode();
		}
		IndexSelectNode* selectNode = bestField->getSelectNode();
		
		if (differentNodeCount > 0) {
			//oprocz tego pola po ktorym indeksujemy sa jeszcze inne. trzeba je poustawiac metoda cutOff
			vector<TreeNode*> usedBounds = bestField->getUsedBounds();
			vector<TreeNode*>::iterator it;
			/*for (it = usedBounds.begin(); it != usedBounds.end(); it++) {
				TreeNode* t = *it;
				cutOff(*it);
			}*/
			for (unsigned int i = 0; i < usedBounds.size(); i++) {
				cutOff(usedBounds.at(i));
			}
			//glowny wezel optymalizowanego drzewa to where
			TwoArgsNode* whereNode = dynamic_cast<TwoArgsNode*> (tree);
			delete whereNode->getLArg();
			whereNode->setLArg(selectNode);
			return;
		} else {
			//trzeba tylko podmienic tree na select
			delete tree;
			tree = selectNode;
		}
		
	}
	
	void QueryOptimizer::whereReplace(TreeNode* tree) {
		if (tree->type() != TreeNode::TNNONALGOP) {
			differentNode();
			return;
		}
		NonAlgOpNode* whereNode = dynamic_cast<NonAlgOpNode*>(tree);
		if (whereNode->getOp() != NonAlgOpNode::where) {
			differentNode();
			return;
		}
		TreeNode* rootNameNode = whereNode->getLArg();
		if (rootNameNode->type() != TreeNode::TNNAME) {
			differentNode();
			return;
		}
		
		string rootName = (dynamic_cast<NameNode*>(rootNameNode))->getName();
		
		IndexManager *im = IndexManager::getHandle(); 
		
		im->indexListSem->lock_read();
		if (! im->isRootIndexed(rootName, indexedFields)) {
			differentNode();
			im->indexListSem->unlock();
			return;
		}
		im->indexListSem->unlock();
		//bool whereNeeded = false;
		TreeNode* andTree = whereNode->getRArg();
		//IndexSelectNode* sNode = prepareSelectNode(andTree, it->second, whereNeeded);
		
		findPossibleFields(andTree);
		
	}
	
	void QueryOptimizer::optimizeWithIndexes(TreeNode* &tree) {
		if (implicitIndexCall) {
			QueryOptimizer opt;
			opt.whereReplace(tree);
				//tylko do wydruku kontrolnego
				bool possible = opt.optimizationPossible();
			opt.createResult(tree);
			
			//wydruk kontrolny po optymalizacji
			if (possible) {
				debug_printf(*((IndexManager::getHandle())->ec), "index implicitly used: %s\n", ((dynamic_cast<QueryNode*>(tree))->deParse()).c_str());
			} else {
				debug_print((*((IndexManager::getHandle())->ec)),  "no implicit index optimisation");
			}
		}
	}
	
	void QueryOptimizer::setImplicitIndexCall(bool implicitIndexCall) {
		QueryOptimizer::implicitIndexCall = implicitIndexCall;
	}

}
