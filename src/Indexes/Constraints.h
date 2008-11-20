#ifndef CONSTRAINTS_H_
#define CONSTRAINTS_H_

#include <QueryParser/TreeNode.h>
#include <Indexes/EntrySieve.h>
#include <Indexes/LeafTraveller.h>

using namespace QParser;
namespace Indexes {

	//class QParser::QueryNode;

	class Constraints {
		protected:
			LeafTraveller* traveler;
			vector<QueryNode*> subqueries;
			
			Constraints();
			virtual int find(BTree* tree)=0;
		public:
			virtual ~Constraints();
			
			/** metoda wykonujaca cale wyszukiwanie */
			int search(BTree* tree, EntrySieve* sieve);
			
			/** ustawia obliczone podzapytania */
			virtual void setSubresults(vector<QueryResult*> subresults)=0;
			vector<QueryNode*> getSubqueries();
			
			/** na potrzeby testow. konweruje proste wartosci */
			QueryResult* convert(QueryNode* value);
			virtual string toString()=0;
	};
	
	class ExactConstraints : public Constraints {
		private:
			QueryResult* value;
			virtual int find(BTree* tree);
		public:
			ExactConstraints(QParser::QueryNode* value);
			virtual ~ExactConstraints();
			virtual void setSubresults(vector<QueryResult*> subresults);
			virtual string toString();
	};
	
	class TwoSideConstraints : public Constraints {
		private:
			QueryResult *leftValue, *rightValue;
			bool inclusiveLeft, inclusiveRight;
		protected:
			virtual int find(BTree* tree);
		public:
			TwoSideConstraints(bool inclusiveLeft, QParser::QueryNode* leftValue, QParser::QueryNode* rightValue, bool inclusiveRight);
			virtual ~TwoSideConstraints();
			virtual void setSubresults(vector<QueryResult*> subresults);
			virtual string toString();
	};
	
	class OneSideBoundedConstraints : public Constraints {
		protected:
			bool inclusive;
			QueryResult* value;
		protected:
			//virtual int find(BTree* tree);
		public:
			OneSideBoundedConstraints(bool inclusive, QueryNode *value);
			virtual ~OneSideBoundedConstraints(); 
			virtual void setSubresults(vector<QueryResult*> subresults);
		
	};
	
	class LeftBoundedConstraints : public OneSideBoundedConstraints {
		protected:
			virtual int find(BTree* tree);
		public:
			LeftBoundedConstraints(bool leftInclusive, QueryNode* leftValue);
			virtual ~LeftBoundedConstraints();
			virtual string toString();
	};
	
	class RightBoundedConstraints : public OneSideBoundedConstraints {
		protected:
			virtual int find(BTree* tree);
		public:
			RightBoundedConstraints(QueryNode* rightValue, bool rightInclusive);
			virtual ~RightBoundedConstraints();
			virtual string toString();
	};

}

#endif /*CONSTRAINTS_H_*/
