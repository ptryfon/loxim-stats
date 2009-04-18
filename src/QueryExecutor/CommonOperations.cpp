#include <QueryExecutor/CommonOperations.h>
#include <QueryExecutor/QueryResult.h>
#include <Errors/ErrorConsole.h>
#include <TransactionManager/Transaction.h>

namespace CommonOperations
{
	int nameTaken(string name, Transaction *tr, bool &taken)
	{
		taken = false;
		vector<LogicalID *>* lids;
		int errcode = tr->getRootsLID(name, lids);
		if (errcode || !lids)
		{
			if (lids) delete lids;
			return errcode;
		}
		if (lids->size() == 0) return 0;
		taken = true;
		return 0;	
	}
	
	int isIncluded(QueryResult *elem, QueryResult *set, bool &score) 
	{
		int errcode;
		if (set->type() != QueryResult::QBAG) 
			return (ErrQExecutor | EOtherResExp);
		bool tmp_bool = false;
		for (unsigned int i = 0; i < set->size(); i++) 
		{
			QueryResult *tmp_res;
			errcode = ((QueryBagResult *) set)->at(i, tmp_res);
			if (errcode != 0) return errcode;
			bool eq = elem->equal(tmp_res);
			if (eq)
				tmp_bool = true;
		}
		score = tmp_bool;
		return 0;
	}
	
	int merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, ErrorConsole *ec, QueryResult *&final) 
	{
		debug_print(*ec,  "[QE] merge() function applied to the partial results, creating final result");
		int errcode;
		switch (op) {
			case NonAlgOpNode::dot: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <dot>");
				final = partial;
				break;
			}
			case NonAlgOpNode::join: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <join>");
				final = partial;
				break;
			}
			case NonAlgOpNode::where: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <where>");
				final = partial;
				break;
			}
			case NonAlgOpNode::orderBy: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <orderBy>");
				((QueryBagResult *) partial)->sortBag_in_orderBy();
				errcode = ((QueryBagResult *) partial)->postSort(final);
				if (errcode != 0) return errcode;
				break;
			}
			case NonAlgOpNode::exists: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <exists>");
				bool tmp_bool = false;
				bool current_bool;
				for (unsigned i = 0; i < (partial->size()); i++) {
					QueryResult *tmp_result;
					errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
					if (errcode != 0) return errcode;
					if (not (tmp_result->isBool())) {
						debug_print(*ec,  "[QE] merge(): ERROR! operator <exists> expects boolean type arguments");
						debug_print(*ec,  (ErrQExecutor | EBoolExpected));
						return (ErrQExecutor | EBoolExpected);
					}
					errcode = (tmp_result->getBoolValue(current_bool));
					if (errcode != 0) return errcode;
					if (current_bool)
						tmp_bool = true;
				}
				QueryBoolResult * tmp_bool_res = new QueryBoolResult(tmp_bool);
				final->addResult(tmp_bool_res);
				break;
			}
			case NonAlgOpNode::forAll: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <forAll>");
				bool tmp_bool = true;
				bool current_bool;
				for (unsigned i = 0; i < (partial->size()); i++) {
					QueryResult *tmp_result;
					errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
					if (errcode != 0) return errcode;
					if (not (tmp_result->isBool())) {
						debug_print(*ec,  "[QE] merge(): ERROR! operator <forAll> expects boolean type arguments");
						debug_print(*ec,  (ErrQExecutor | EBoolExpected));
						return (ErrQExecutor | EBoolExpected);
					}
					errcode = (tmp_result->getBoolValue(current_bool));
					if (errcode != 0) return errcode;
					if (not current_bool)
						tmp_bool = false;
				}
				QueryBoolResult *tmp_bool_res = new QueryBoolResult(tmp_bool);
				final->addResult(tmp_bool_res);
				break;
			}
			case NonAlgOpNode::forEach: {
				debug_print(*ec,  "[QE] merge(): NonAlgebraic operator <forEach>");
				final = partial;
				break;
			}
			default: {
				debug_print(*ec,  "[QE] merge(): unknown NonAlgebraic operator!");
				debug_print(*ec,  (ErrQExecutor | EUnknownNode));
				return (ErrQExecutor | EUnknownNode);
				break;
			}
		}
		return 0;
	}
}
