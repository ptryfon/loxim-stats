/* Query Executor
   10.2005 - 01.2006

   Authors:
   Beata Golichowska
   Dariusz Gryglas
   Adam Michalik
   Maja Perycz */
   
#include <stdio.h>
#include <string>
#include <vector>
   
#include "QueryResult.h"
#include "TransactionManager/Transaction.h"
#include "Store/Store.h"
#include "Store/DBDataValue.h"
#include "Store/DBLogicalID.h"
#include "QueryParser/QueryParser.h"
#include "QueryParser/TreeNode.h"
#include "QueryExecutor.h"
#include "Errors/Errors.h"
#include "Errors/ErrorConsole.h"

using namespace QParser;
using namespace TManager;
using namespace Errors;
using namespace Store;
using namespace std;

namespace QExecutor {

int QueryExecutor::executeQuery(TreeNode *tree, QueryResult **result) {
	
	int errcode;
	*ec << "[QE] executeQuery()";
	
	if (tree != NULL) {
		int nodeType = tree->type();
		*ec << "[QE] TreeType taken";
		if (tr == NULL) {
			if (nodeType == TreeNode::TNTRANS) {
				if (((TransactNode *) tree)->getOp() == TransactNode::begin) {
					*ec << "[QE] Asking TransactionManager for a new transaction";
					errcode = TransactionManager::getHandle()->createTransaction(tr);
					if (errcode != 0) {
						*ec << "[QE] Error in createTransaction";
						*ec << "[QE] Transction not opened";
						*result = new QueryNothingResult();
						*ec << "[QE] nothing to do: QueryNothingResult created";
						return errcode;
					}
					else {
						*ec << "[QE] Transction opened";
						*result = new QueryNothingResult();
						*ec << "[QE] nothing else to do: QueryNothingResult created";
						return 0;
					}
				}
				else {
					*ec << "[QE] Transction not opened, type < begin; > first";
					*result = new QueryNothingResult();
					*ec << "[QE] nothing to do: QueryNothingResult created";
					return 0;
				}
			}
			else {
				*ec << "[QE] Transction not opened, type < begin; > first";
				*result = new QueryNothingResult();
				*ec << "[QE] nothing to do: QueryNothingResult created";
				return 0;
			}
		}
		switch (nodeType) 
		{
		case TreeNode::TNNAME: {
			*ec << "[QE] Type: TNNAME";
			string name = tree->getName();
			if ((stack.size()) == 1) {
			    errcode = (stack.pop());
			    if (errcode != 0) { return errcode; };
			};
			if (stack.empty()) {
				vector<ObjectPointer*>* vec;
				if ((errcode = tr->getRoots(vec)) != 0) {
					return errcode;
				};
				int vecSize = vec->size();
				ec->printf("[QE] All %d Roots taken\n", vecSize);
				QueryBagResult *stackSection = new QueryBagResult();
				for (int i = 0; i < vecSize; i++ )
					{
   					ObjectPointer *optr = vec->at(i);
					LogicalID *lid = optr->getLogicalID();
					*ec << "[QE] LogicalID received";
					string optrName = (optr->getName());
					*ec << "[QE] Name received";
					QueryReferenceResult *lidres = new QueryReferenceResult(lid);
					QueryBinderResult *newBinding = new QueryBinderResult(optrName, lidres);
					stackSection->addResult(newBinding);
				};
				if ( (stack.push(stackSection)) != 0 ) {
					return -1;
				};
			};			
			if ((stack.bindName(name, *result)) != 0) {
				return -1;
			};
			*ec << "[QE] Done!";
			return 0;
		}//case TNNAME

		case TreeNode::TNCREATE: {
			*ec << "[QE] Type: TNCREATE";
			string name = tree->getName();
			QueryResult *tmp_result;
			errcode = executeQuery (tree->getArg(), &tmp_result);
			if (errcode != 0) return errcode;
			if (not (tmp_result->isSingleValue())) {
				*ec << "[QE] ERROR! Create operation, argument must be single value type";
				*result = new QueryNothingResult();
				*ec << "[QE] QueryNothingResult created";
				return -1;
			}
			QueryResult *single;
			errcode = tmp_result->getSingleValue(single);
			if (errcode != 0) return errcode;
			DBDataValue *dbValue = new DBDataValue();
			switch (single->type()) {
				case QueryResult::QINT: {
					int intValue = ((QueryIntResult *) single)->getValue();
					dbValue->setInt(intValue);
					*ec << "[QE] < CREATE NAME ('integer'); > operation";
					break;
				}
				case QueryResult::QDOUBLE: {
					double doubleValue = ((QueryDoubleResult *) single)->getValue();
					dbValue->setDouble(doubleValue);
					*ec << "[QE] < CREATE NAME ('double'); > operation";
					break;
				}
				case QueryResult::QSTRING: {
					string stringValue = ((QueryStringResult *) single)->getValue();
					dbValue->setString(stringValue);
					*ec << "[QE] < CREATE NAME ('string'); > operation";
					break;
				}
				case QueryResult::QREFERENCE: {
					LogicalID* refValue = ((QueryReferenceResult *) single)->getValue();
					dbValue->setPointer(refValue);
					*ec << "[QE] < CREATE NAME ('reference'); > operation";
					break;
				}
				case QueryResult::QNOTHING: {
					vector<LogicalID*> emptyVector;
					dbValue->setVector(&emptyVector);
					*ec << "[QE] < CREATE NAME; > operation";
					break;
				}
				default: {
					*ec << "[QE] ERROR! Create operation, bad type argument evaluated by executeQuery";
					*ec << (EBadType | ErrQExecutor);
					*result = new QueryNothingResult();
					*ec << "[QE] QueryNothingResult created";
					return EBadType | ErrQExecutor;
				}
			}
			DataValue* value;
			value = dbValue;
			ObjectPointer *optr;
			if ((errcode = tr->createObject(name, value, optr)) != 0)
				{
				*ec << "[QE] Error in createObject";
				return errcode;
				}
			if ((errcode = tr->addRoot(optr)) != 0)
				{
				*ec << "[QE] Error in addRoot";
				return errcode;
				}
			*result = new QueryBagResult();
			QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
			(*result)->addResult (lidres);
			*ec << "[QE] CREATE Done!";
			return 0;
		} //case TNCREATE

		case TreeNode::TNINT: {
			int intValue = ((IntNode *) tree)->getValue();
			ec->printf("[QE] TNINT: %d\n", intValue);
			*result = new QueryBagResult();
			QueryIntResult *tmpResult = new QueryIntResult(intValue);
			(*result)->addResult(tmpResult);
			ec->printf("[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
		} //case TNINT
		
		case TreeNode::TNSTRING: {
			string stringValue = ((StringNode *) tree)->getValue();
			*ec << "[QE] QueryStringResult created";
			*result = new QueryBagResult();
			QueryStringResult *tmpResult = new QueryStringResult(stringValue);
			(*result)->addResult(tmpResult);
			*ec << "[QE] QueryStringResult created";
			return 0;
		} //case TNSTRING
		
		case TreeNode::TNDOUBLE: {
			double doubleValue = ((DoubleNode *) tree)->getValue();
			ec->printf("[QE] TNDOUBLE: %f\n", doubleValue);
			*result = new QueryBagResult;
			QueryDoubleResult *tmpResult = new QueryDoubleResult(doubleValue);
			(*result)->addResult(tmpResult);
			ec->printf("[QE] QueryDoubleResult (%f) created\n", doubleValue);
			return 0;
		} //case TNDOUBLE
			
		case TreeNode::TNVECTOR: {
			*ec << "[QE] tree - Vector: not implemented, don't know what to do";
			*result = new QueryNothingResult();
			*ec << "[QE] QueryNothingResult created";
			return 0;
		} //case TNVECTOR
		
		case TreeNode::TNAS: {
			*ec << "[QE] AS/GROUP_AS operation recognized";
			string name = tree->getName();
			QueryResult *tmp_result;
			errcode = executeQuery(tree->getArg(), &tmp_result);
			if (errcode != 0) return errcode;
			bool grouped = ((NameAsNode *) tree)->isGrouped();
			QueryResult *final_result = new QueryBagResult();
			if (grouped) {
				*ec << "[QE] GROUP AS operation";
				QueryResult *tmp_binder = new QueryBinderResult(name, tmp_result);
				*ec << "[QE] GROUP AS: new binder created and added to final result";
				final_result->addResult(tmp_binder);
			}
			else {
				*ec << "[QE] AS operation";
				QueryResult *partial_result;
				if (((tmp_result->type()) != QueryResult::QSEQUENCE) && ((tmp_result->type()) != QueryResult::QBAG)) {
					partial_result = new QueryBagResult();
					partial_result->addResult(tmp_result);
				}
				else
					partial_result = tmp_result;
				for (unsigned int i = 0; i < partial_result->size(); i++) {
					QueryResult *nextResult;
					errcode = ((QueryBagResult *) partial_result)->at(i, nextResult);
					if (errcode != 0) return errcode;
					QueryResult *tmp_binder = new QueryBinderResult(name, nextResult);
					*ec << "[QE] GROUP AS: new binder created and added to final result";
					final_result->addResult(tmp_binder);
				}
			}
			*result = final_result;
			*ec << "[QE] AS operation Done!";
			return 0;
		} //case TNAS
	
		case TreeNode::TNUNOP: {
			UnOpNode::unOp op = ((UnOpNode *) tree)->getOp();
			*ec << "[QE] Unary operator - type recognized";
			QueryResult *tmp_result;
			errcode = executeQuery(tree->getArg(), &tmp_result);
			if (errcode != 0) return errcode;
			QueryResult *op_result;
			errcode = this->unOperate(op, tmp_result, op_result);
			if (errcode != 0) return errcode;
			QueryResult *final_result = new QueryBagResult();
			final_result->addResult(op_result);
			*result = final_result;
			*ec << "[QE] Unary operation Done!";
			return 0;
		} //case TNUNOP 
	
		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			*ec << "[QE] Algebraic operator - type recognized";
			QueryResult *lResult, *rResult;
			errcode = executeQuery(((AlgOpNode *) tree)->getLArg(), &lResult);
			if (errcode != 0) return errcode;
			errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult);
			if (errcode != 0) return errcode;
			QueryResult *op_result;
			errcode = this->algOperate(op, lResult, rResult, op_result);
			if (errcode != 0) return errcode;
			QueryResult *final_result = new QueryBagResult();
			final_result->addResult(op_result);
			*result = final_result;
			*ec << "[QE] Algebraic operation Done!";
			return 0;
		} // TNALGOP

		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			*ec << "[QE] NonAlgebraic operator - type recognized";
			QueryResult *l_tmp_Result;
			errcode = executeQuery (((NonAlgOpNode *) tree)->getLArg(), &l_tmp_Result);
			if (errcode != 0) return errcode;
			QueryResult *lResult;
			if (((l_tmp_Result->type()) != QueryResult::QSEQUENCE) && ((l_tmp_Result->type()) != QueryResult::QBAG)) {
				lResult = new QueryBagResult();
				lResult->addResult(l_tmp_Result);
			}
			else
				lResult = l_tmp_Result;
			*ec << "[QE] Left argument of NonAlgebraic query has been computed";
			QueryResult *partial_result = new QueryBagResult();
			if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {
				*ec << "[QE] For each row of this score, the right argument will be computed";
				for (unsigned int i = 0; i < (lResult->size()); i++) {
					QueryResult *currentResult;
					QueryResult *newStackSection = new QueryBagResult();
					if ((lResult->type()) == QueryResult::QSEQUENCE)
						errcode = (((QuerySequenceResult *) lResult)->at(i, currentResult));
					else
						errcode = (((QueryBagResult *) lResult)->at(i, currentResult));
					if (errcode != 0) return errcode;
					errcode = (currentResult)->nested(tr, newStackSection);
					if (errcode != 0) return errcode;
					ec->printf("[QE] nested(): function calculated for current row number %d\n", i);
					if ( (stack.push((QueryBagResult *) newStackSection)) != 0 ) {
						return -1; //this would be very strange, this function can only return 0
					}
					QueryResult *rResult;
					errcode = executeQuery (((NonAlgOpNode *) tree)->getRArg(), &rResult);
					if (errcode != 0) return errcode;
					*ec << "[QE] Computing left Argument with a new scope of ES";
					errcode = this->combine(op,currentResult,rResult,partial_result);
					if (errcode != 0) return errcode;
					*ec << "[QE] Combined partial results";
					errcode = stack.pop();
					if (errcode != 0) return errcode;
				}
			}
			else {
				*ec << "[QE] ERROR! NonAlgebraic operation, bad left argument";
				*result = new QueryNothingResult();
				return -1;
			}
			QueryResult *final_result = new QueryBagResult();
			errcode = this->merge(op,partial_result, final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Merged partial results into final result";
			*result = final_result;
			*ec << "[QE] NonAlgebraic operation Done!";
			return 0;
		} //case TNNONALGOP

		case TreeNode::TNTRANS: {
			TransactNode::transactionOp op = ((TransactNode *) tree)->getOp();
			switch (op) {
				case TransactNode::begin: {
					*ec << "[QE] ERROR! Transaction already opened. It can't be opened once more!";
					*ec << "[QE] maybe someday, when nested transactions will be implemented...";
					*result = new QueryNothingResult();
					return -1;
				}
				case TransactNode::end: {
					errcode = tr->commit();
					if (errcode != 0) {
						*ec << "[QE] error in transaction->commit()";
						*result = new QueryNothingResult();
						return errcode;
					}
					*ec << "[QE] Transaction commited succesfully";
					tr = NULL;
					break;
				}
				case TransactNode::abort: {
					errcode = tr->abort();
					if (errcode != 0) {
						*ec << "[QE] error in transaction->abort()";
						*result = new QueryNothingResult();
						return errcode;
					}
					*ec << "[QE] Transaction aborted succesfully";
					tr = NULL;
					break;
				}
				default: {
					*ec << "[QE] ERROR! unknown transaction operator";
					*result = new QueryNothingResult();
					return -1;
				}
			}
			*result = new QueryNothingResult();
			*ec << "[QE] Nothing else to do. QueryNothingResult created";
			return 0;
		} //case TNTRANS

		default:
			{
			*ec << "Unknown node type";
			*ec << (ENoType | ErrQExecutor);
			*result = new QueryNothingResult();
			*ec << "[QE] QueryNothingResult created";
			return ENoType | ErrQExecutor;
			}

		} // end of switch
		
	} // if tree!=Null
	else { // tree == NULL; return empty result
		*ec << "[QE] empty tree, nothing to do";
		*result = new QueryNothingResult();
		*ec << "[QE] QueryNothingResult created";
	}
	return 0;
    }//executeQuerry

int QueryExecutor::derefQuery(QueryResult *arg, QueryResult *&res) {
	*ec << "[QE] derefQuery()";
	int errcode;
	int argType = arg->type();
	switch (argType) {
		case QueryResult::QSEQUENCE: {
			res = new QuerySequenceResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QuerySequenceResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBAG: {
			res = new QueryBagResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryBagResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QSTRUCT: {
			res = new QueryStructResult();
			for (unsigned int i = 0; i < (arg->size()); i++) {
				QueryResult *tmp_item;
				errcode = ((QueryStructResult *) arg)->at(i, tmp_item);
				if (errcode != 0) return errcode;
				QueryResult *tmp_res;
				errcode = this->derefQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->derefQuery(((QueryBinderResult *) arg)->getItem(), tmp_item);
			if (errcode != 0) return errcode;
			res = new QueryBinderResult(tmp_name, tmp_item);
			break;
		}
		case QueryResult::QBOOL: {
			res = arg;
			break;
		}
		case QueryResult::QINT: {
			res = arg;
			break;
		}
		case QueryResult::QDOUBLE: {
			res = arg;
			break;
		}
		case QueryResult::QSTRING: {
			res = arg;
			break;
		}
		case QueryResult::QREFERENCE: {
			LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
			*ec << "[QE] derefQuery() - dereferencing Object";
			ObjectPointer *optr;
			errcode = tr->getObjectPointer(ref_value, Store::Read, optr);
			if (errcode != 0) {
				*ec << "[QE] Error in getObjectPointer";
				return errcode;
			}
			DataValue* value = optr->getValue();
			int vType = value->getType();
			switch (vType) {
				case Store::Integer: {
					*ec << "[QE] derefQuery() - ObjectValue = Int";
					int tmp_value = value->getInt();
					res = new QueryIntResult(tmp_value);
					break;
				}
				case Store::Double: {
					*ec << "[QE] derefQuery() - ObjectValue = Double";
					double tmp_value = value->getDouble();
					res = new QueryDoubleResult(tmp_value);
					break;
				}
				case Store::String: {
					*ec << "[QE] derefQuery() - ObjectValue = String";
					string tmp_value = value->getString();
					res = new QueryStringResult(tmp_value);
					break;
				}
				default: {
					*ec << "[QE] derefQuery() - wrong argument type";
					res = new QueryNothingResult();
					break;
				}
			}
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		default: {
			*ec << "[QE] ERROR in derefQuery() - unknown result type";
			res = new QueryNothingResult();
			*ec << "[QE] QueryNothingResult created";
			return -1;
		}
	}
	if (res->isSingleValue()) {
		QueryResult *single;
		errcode = res->getSingleValue(single);
		if (errcode != 0) return errcode;
		res = single;
	}
	return 0;
}

int QueryExecutor::unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final) {
	int errcode;
	switch (op) {
		case UnOpNode::deref: {
			*ec << "[QE] DEREF operation";
			errcode = derefQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::unMinus: {
			*ec << "[QE] UN_MINUS operation";
			QueryResult *derefArg;
			errcode = this->derefQuery(arg,derefArg);
			if ((derefArg->type() == QueryResult::QINT) || (derefArg->type() == QueryResult::QDOUBLE)) {
				if (derefArg->type() == QueryResult::QINT)
					errcode = ((QueryIntResult *)derefArg)->minus(final);
				else
					errcode = ((QueryDoubleResult *)derefArg)->minus(final);
				if (errcode != 0) return errcode;
			}
			else {
				*ec << "[QE] ERROR! UN_MINUS argument must be INT or DOUBLE";
				final = new QueryNothingResult();
				return -1;
			}
			break;
		}
		case UnOpNode::boolNot: {
			*ec << "[QE] NOT operation";
			if (arg->isBool()) {
				bool bool_tmp;
				errcode = arg->getBoolValue(bool_tmp);
				if (errcode != 0) return errcode;
				final = new QueryBoolResult(not bool_tmp);
			}
			else {
				*ec << "[QE] ERROR! NOT argument must be BOOLEAN";
				final = new QueryNothingResult();
				return -1;
			}
			break;
		}
		case UnOpNode::count: {
			*ec << "[QE] COUNT operation";
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			int count_res = bagArg->size();
			final = new QueryIntResult(count_res);
			break;
		}
		case UnOpNode::sum: {
			*ec << "[QE] SUM operation";
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_sum_res = 0;
			int i_count = 0;
			double d_sum_res = 0.0;
			int d_count = 0;
			if (count_res == 0)
				final = new QueryIntResult(0);
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						i_sum_res = i_sum_res + (((QueryIntResult *) curr_res)->getValue());
						i_count++;
						break;
					}
					case QueryResult::QDOUBLE: {
						d_sum_res = d_sum_res + (((QueryDoubleResult *) curr_res)->getValue());
						d_count++;
						break;
					}
					default : { break; }
				}
			}
			if (d_count > 0) {
				d_sum_res = d_sum_res + ((double) i_sum_res);
				final = new QueryDoubleResult(d_sum_res);
			}
			else {
				final = new QueryIntResult(i_sum_res);
			}
			break;
		}
		case UnOpNode::avg: {
			*ec << "[QE] AVG operation";
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_sum_res = 0;
			int i_count = 0;
			double d_sum_res = 0.0;
			int d_count = 0;
			if (count_res == 0)
				final = new QueryIntResult(0);
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						i_sum_res = i_sum_res + (((QueryIntResult *) curr_res)->getValue());
						i_count++;
						break;
					}
					case QueryResult::QDOUBLE: {
						d_sum_res = d_sum_res + (((QueryDoubleResult *) curr_res)->getValue());
						d_count++;
						break;
					}
					default : { break; }
				}
			}
			d_sum_res = d_sum_res + ((double) i_sum_res);
			double total = d_sum_res / (i_count + d_count);
			final = new QueryDoubleResult(total);
			break;
		}
		case UnOpNode::min: {
			*ec << "[QE] MIN operation";
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_min_res = 0;
			bool i_counted = false;
			double d_min_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				*ec << "[QE] MIN operation - can't evaluate min value on this set, QueryNothingResult created";
				final = new QueryNothingResult();
			}
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						if (not i_counted) {
							i_min_res = (((QueryIntResult *) curr_res)->getValue());
							i_counted = true;
						}
						else {
							if ((((QueryIntResult *) curr_res)->getValue()) < i_min_res)
								i_min_res = (((QueryIntResult *) curr_res)->getValue());
						}
						break;
					}
					case QueryResult::QDOUBLE: {
						if (not d_counted) {
							d_min_res = (((QueryDoubleResult *) curr_res)->getValue());
							d_counted = true;
						}
						else {
							if ((((QueryDoubleResult *) curr_res)->getValue()) < d_min_res)
								d_min_res = (((QueryDoubleResult *) curr_res)->getValue());
						}
						break;
					}
					default : { break; }
				}
			}
			if (i_counted) {
				if (d_counted) {
					if (i_min_res <= d_min_res)
						final = new QueryIntResult(i_min_res);
					else
						final = new QueryDoubleResult(d_min_res);
				}
				else {
					final = new QueryIntResult(i_min_res);
				}
			}
			else {
				if (d_counted) {
					final = new QueryDoubleResult(d_min_res);
				}
				else { //no int or double value
					*ec << "[QE] MIN - can't evaluate min value on this set, QueryNothingResult created";
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::max: {
			*ec << "[QE] MAX operation";
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_max_res = 0;
			bool i_counted = false;
			double d_max_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				*ec << "[QE] MAX operation - can't evaluate max value on this set, QueryNothingResult created";
				final = new QueryNothingResult();
			}
			else for (unsigned int i = 0; i < count_res; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				QueryResult *curr_res;
				errcode = this->derefQuery(current, curr_res);
				if (errcode != 0) return errcode;
				switch (curr_res->type()) {
					case QueryResult::QINT: {
						if (not i_counted) {
							i_max_res = (((QueryIntResult *) curr_res)->getValue());
							i_counted = true;
						}
						else {
							if ((((QueryIntResult *) curr_res)->getValue()) > i_max_res)
								i_max_res = (((QueryIntResult *) curr_res)->getValue());
						}
						break;
					}
					case QueryResult::QDOUBLE: {
						if (not d_counted) {
							d_max_res = (((QueryDoubleResult *) curr_res)->getValue());
							d_counted = true;
						}
						else {
							if ((((QueryDoubleResult *) curr_res)->getValue()) > d_max_res)
								d_max_res = (((QueryDoubleResult *) curr_res)->getValue());
						}
						break;
					}
					default : { break; }
				}
			}
			if (i_counted) {
				if (d_counted) {
					if (i_max_res >= d_max_res)
						final = new QueryIntResult(i_max_res);
					else
						final = new QueryDoubleResult(d_max_res);
				}
				else {
					final = new QueryIntResult(i_max_res);
				}
			}
			else {
				if (d_counted) {
					final = new QueryDoubleResult(d_max_res);
				}
				else { //no int or double value
					*ec << "[QE] MAX - can't evaluate max value on this set, QueryNothingResult created";
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::distinct: {
			*ec << "[QE] DISTINCT operation";
			final = new QueryBagResult();
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int bag_size = bagArg->size();
			for (unsigned int i = 0; i < bag_size; i++) {
				QueryResult *current;
				errcode = bagArg->getResult(current);
				if (errcode != 0) return errcode;
				bool is_incl;
				errcode = this->isIncluded(current, bagArg, is_incl);
				if (errcode != 0) return errcode;
				if (not is_incl)
					final->addResult(current);
			}
			break;
		}
		case UnOpNode::deleteOp: {
			*ec << "[QE] DELETE operation";
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			for (unsigned int i = 0; i < bagArg->size(); i++) {
				QueryResult* toDelete;  //the object to be deleted
				errcode = ((QueryBagResult *) bagArg)->at(i, toDelete);
				if (errcode != 0) return errcode;
				int toDeleteType = toDelete->type();
				if (toDeleteType != QueryResult::QREFERENCE) {
					*ec << "[QE] ERROR! DELETE argument must consist of QREFERENCE";
					final = new QueryNothingResult();
					return -1;
				}
				LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
				ObjectPointer *optr;
				if ((errcode = tr->getObjectPointer (lid, Store::Write, optr)) !=0) {
					*ec << "[QE] Error in getObjectPointer.";
					return errcode;
				}
				if ((errcode = tr->removeRoot(optr)) != 0) {
					*ec << "[QE] Error in removeRoot.";
					return errcode;
				}
				*ec << "[QE] Root removed";
				if ((errcode = tr->deleteObject(optr)) != 0) {
					*ec << "[QE] Error in deleteObject.";
					return errcode;
				}
				*ec << "[QE] Object deleted";
			}
			*ec << "[QE] delete operation complete - QueryNothingResult created";
			final = new QueryNothingResult();
			break;
		}
		default: { // unOperation type not known
			*ec << "[QE] ERROR! Unary operation type not known";
			final = new QueryNothingResult();
			return -1;
		}
	}
	return 0;
}

int QueryExecutor::algOperate(AlgOpNode::algOp op, QueryResult *lArg, QueryResult *rArg, QueryResult *&final) {
	int errcode;
	if ((op == AlgOpNode::plus) || (op == AlgOpNode::minus) || (op == AlgOpNode::times) || (op == AlgOpNode::divide)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		if ((derefL->type() == QueryResult::QINT) || (derefL->type() == QueryResult::QDOUBLE)) {
			switch (op) {
				case AlgOpNode::plus: {
					*ec << "[QE] + operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->plus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->plus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::minus: {
					*ec << "[QE] - operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->minus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->minus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::times: {
					*ec << "[QE] * operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->times(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->times(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::divide: {
					*ec << "[QE] / operation";
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->divide_by(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->divide_by(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				default: { break; }
			}
		}
		else {
			switch (op) {
				case AlgOpNode::plus: {
					final = new QueryNothingResult();
					*ec << "[QE] ERROR! + arguments must be INT or DOUBLE";
					return -1;
				}
				case AlgOpNode::minus: {
					final = new QueryNothingResult();
					*ec << "[QE] ERROR! - arguments must be INT or DOUBLE";
					return -1;
				}
				case AlgOpNode::times: {
					final = new QueryNothingResult();
					*ec << "[QE] ERROR! * arguments must be INT or DOUBLE";
					return -1;
				}
				case AlgOpNode::divide: {
					final = new QueryNothingResult();
					*ec << "[QE] ERROR! / arguments must be INT or DOUBLE";
					return -1;
				}
				default: { break; }
			}
		}
		return 0;
	}
	else if ((op == AlgOpNode::eq) || (op == AlgOpNode::neq) || (op == AlgOpNode::lt) || 
	(op == AlgOpNode::gt) || (op == AlgOpNode::le) || (op == AlgOpNode::ge)) {
		QueryResult *derefL, *derefR;
		errcode = this->derefQuery(lArg,derefL);
		if (errcode != 0) return errcode;
		errcode = this->derefQuery(rArg,derefR);
		if (errcode != 0) return errcode;
		switch (op) {
			case AlgOpNode::eq: {
				*ec << "[QE] = operation";
				bool bool_tmp = derefL->equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::neq: {
				*ec << "[QE] != operation";
				bool bool_tmp = derefL->not_equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::lt: {
				*ec << "[QE] < operation";
				bool bool_tmp = derefL->less_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::gt: {
				*ec << "[QE] > operation";
				bool bool_tmp = derefL->greater_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::le: {
				*ec << "[QE] <= operation";
				bool bool_tmp = derefL->less_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::ge: {
				*ec << "[QE] >= operation";
				bool bool_tmp = derefL->greater_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			default : { break; }
		}
		return 0;
	} 
	else if ((op == AlgOpNode::boolOr) || (op == AlgOpNode::boolAnd)) {
		if ((lArg->isBool()) && (rArg->isBool())) {
			bool bool_l, bool_r;
			errcode = lArg->getBoolValue(bool_l);
			if (errcode != 0) return errcode;
			errcode = rArg->getBoolValue(bool_r);
			if (errcode != 0) return errcode;
			switch (op) {
				case AlgOpNode::boolOr: {
					*ec << "[QE] OR operation";
					bool bool_tmp = (bool_l || bool_r);
					final = new QueryBoolResult(bool_tmp);
					break;
				}
				case AlgOpNode::boolAnd: {
					*ec << "[QE] AND operation";
					bool bool_tmp = (bool_l && bool_r);
					final = new QueryBoolResult(bool_tmp);
					break;
				}
				default : { break; }
			}
		}
		else {
			switch (op) {
				case AlgOpNode::boolOr: {
					*ec << "[QE] ERROR! OR arguments must be BOOLEAN";
					final = new QueryNothingResult();
					return -1;
				}
				case AlgOpNode::boolAnd: {
					*ec << "[QE] ERROR! AND arguments must be BOOLEAN";
					final = new QueryNothingResult();
					return -1;
				}
				default : { break; }
			}
		}
		return 0;
	} 
	else if ((op == AlgOpNode::bagUnion) || (op == AlgOpNode::bagIntersect) || (op == AlgOpNode::bagMinus)) {
		QueryResult *leftBag = new QueryBagResult();
		QueryResult *rightBag = new QueryBagResult();
		leftBag->addResult(lArg);
		rightBag->addResult(rArg);
		switch (op) {
			case AlgOpNode::bagUnion: {
				*ec << "[QE] BAG_UNION operation";
				final = rightBag;
				break;
			}
			case AlgOpNode::bagIntersect: {
				*ec << "[QE] BAG_INTERSECT operation";
				final = new QueryBagResult();
				break;
			}
			case AlgOpNode::bagMinus: {
				*ec << "[QE] BAG_MINUS operation";
				final = new QueryBagResult();
				break;
			}
			default: { break; }
		}
		for (unsigned int i = 0; i < leftBag->size(); i++) {
			QueryResult *tmp_res;
			errcode = ((QueryBagResult *) leftBag)->at(i, tmp_res);
			if (errcode != 0) return errcode;
			bool isIncl;
			errcode = this->isIncluded(tmp_res, rightBag, isIncl);
			if (errcode != 0) return errcode;
			switch (op) {
				case AlgOpNode::bagUnion: {
					if (not isIncl)
						final->addResult(tmp_res);
					break;
				}
				case AlgOpNode::bagIntersect: {
					if (isIncl)
						final->addResult(tmp_res);
					break;
				}
				case AlgOpNode::bagMinus: {
					if (not isIncl)
						final->addResult(tmp_res);
					break;
				}
				default: { break; }
			}
		}
		return 0;
	}
	else if (op == AlgOpNode::comma) {
		*ec << "[QE] COMMA operation";
		final = new QueryBagResult();
		errcode = lArg->comma(rArg,final);
		if (errcode != 0) return errcode;
		return 0;
	}
	else if (op == AlgOpNode::insert) {
		*ec << "[QE] INSERT operation";
		DataValue* db_value;
		vector<LogicalID*> *insVector;
		QueryResult *lBag = new QueryBagResult();
		QueryResult *rBag = new QueryBagResult();
		lBag->addResult(lArg);
		rBag->addResult(rArg);
		for (unsigned int i = 0; i < lBag->size(); i++) {
			for (unsigned int j = 0; j < rBag->size(); j++) {
				ec->printf("[QE] trying to INSERT %d element of leftArg into %d element of rightArg\n", i, j);
				QueryResult* toInsert;  //the object to be inserted
				errcode = ((QueryBagResult *) lBag)->at(i, toInsert);
				if (errcode != 0) return errcode;
				QueryResult* outer;  //the object into which the left agument will be inserted
				errcode = ((QueryBagResult *) rBag)->at(j, outer);
				if (errcode != 0) return errcode;
				int leftResultType = toInsert->type();
				int rightResultType = outer->type();
				if ((leftResultType != QueryResult::QREFERENCE) || (rightResultType != QueryResult::QREFERENCE)) {
					*ec << "[QE] ERROR! both arguments of insert operation must consist of QREFERENCE";
					final = new QueryNothingResult();
					return -1;
				}
				LogicalID *lidIn = ((QueryReferenceResult *) toInsert)->getValue();
				LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
				ObjectPointer *optrIn;
				errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn);
				if (errcode != 0) {
					*ec << "[QE] insert operation - Error in getObjectPointer.";
					return errcode;
				}
				*ec << "[QE] insert operation - getObjectPointer on leftArg done";
				ObjectPointer *optrOut;
				errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut);
				if (errcode != 0) {
					*ec << "[QE] insert operation - Error in getObjectPointer.";
					return errcode;
				}
				*ec << "[QE] insert operation - getObjectPointer on rightArg done";
				// we have to cut left argument from the roots
				errcode = tr->removeRoot(optrIn);
				if (errcode != 0) {
					*ec << "[QE] insert operation - Error in removeRoot.";
					return errcode;
				}
				*ec << "[QE] insert operation - Root removed";
				// now we have to modify right argument's value
				db_value = optrOut->getValue();
				*ec << "[QE] insert operation - Value taken";
				int vType = db_value->getType();
				*ec << "[QE] insert operation - Type of this value taken";
				if (vType != Store::Vector) {
					*ec << "[QE] insert operation - ERROR! the Value has to be a Vector";
					return -1;
				}
				insVector = db_value->getVector();
				if (insVector == NULL) {
					*ec << "[QE] insert operation - insVector == NULL";
					return -1;
				}
				*ec << "[QE] Vector taken";
				ec->printf("[QE] Vec.size = %d\n", insVector->size());
				insVector->push_back(lidIn); // Inserting of the object
				ec->printf("[QE] %d element of leftArg INSERTED INTO %d element of rightArg SUCCESFULLY\n", i, j);
				ec->printf("[QE] New Vec.size = %d\n", insVector->size());
			}
		}
		*ec << "[QE] INSERT operation Done. QueryNothingResult created.";
		final = new QueryNothingResult();
		return 0;
	}
	else { // algOperation type not known
		*ec << "[QE] ERROR! Algebraic operation type not known";
		final = new QueryNothingResult();
		return -1;
	}
	return 0;
}

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial) {
	*ec << "[QE] combine() function applied to the partial results";
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			*ec << "[QE] combine(): NonAlgebraic operator <dot>";
			partial->addResult(lRes);
			break;
		}
		case NonAlgOpNode::join: {
			*ec << "[QE] combine(): NonAlgebraic operator <join>";
			errcode = curr->comma(lRes,partial);
			if (errcode != 0) return errcode;
			break;
		}
		case NonAlgOpNode::where: {
			*ec << "[QE] combine(): NonAlgebraic operator <where>";
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				if (tmp_bool_value) 
					partial->addResult(curr);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <where> is not boolean!";
				return -1; // second argument of the <where> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::closeBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <closeBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <closeUniqueBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::leavesBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <leavesBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::orderBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <orderBy>";
			QueryResult *derefR;
			errcode = this->derefQuery(lRes,derefR);
			if (errcode != 0) return errcode;
			QueryResult *currRow = new QueryStructResult();
			currRow->addResult(curr);
			currRow->addResult(derefR);
			partial->addResult(currRow);
			break;
		}
		case NonAlgOpNode::exists: {
			*ec << "[QE] combine(): NonAlgebraic operator <exists>";
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!";
				return -1; // second argument of the <exists> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			*ec << "[QE] combine(): NonAlgebraic operator <forAll>";
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <forAll> is not boolean!";
				return -1; // second argument of the <forAll> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::assign: {
			*ec << "[QE] combine(): NonAlgebraic operator <assign>";
			if (not ((curr->isSingleValue()) && (lRes->isSingleValue()))) {
				*ec << "[QE] combine() ERROR! operator <assign> needs single type arguments, not collections";
				QueryResult *sth = new QueryNothingResult();
				partial->addResult(sth);
				return -1;
			}
			QueryResult* old_value;
			errcode = curr->getSingleValue(old_value);
			if (errcode != 0) return errcode;
			QueryResult* new_value;
			errcode = lRes->getSingleValue(new_value);
			if (errcode != 0) return errcode;
			int oldType = old_value->type();
			int newType = new_value->type();
			if (oldType != QueryResult::QREFERENCE) {
				*ec << "[QE] combine() ERROR! operator <assign> expects that left argument is REFERENCE";
				QueryResult *sth = new QueryNothingResult();
				partial->addResult(sth);
				return -1;
			}
			LogicalID *old_lid = ((QueryReferenceResult *) old_value)->getValue();
			ObjectPointer *old_optr;
			errcode = tr->getObjectPointer (old_lid, Store::Write, old_optr);
			if (errcode != 0) {
				*ec << "[QE] combine() operator <assign>: error in getObjectPointer()";
				return errcode;
			}
			*ec << "[QE] combine() operator <assign>: getObjectPointer on left argument done";
			DBDataValue *dbValue = new DBDataValue();
			switch (newType) {
				case QueryResult::QINT: {
					int intValue = ((QueryIntResult *) new_value)->getValue();
					dbValue->setInt(intValue);
					*ec << "[QE] < query := ('integer'); > operation";
					break;
				}
				case QueryResult::QDOUBLE: {
					double doubleValue = ((QueryDoubleResult *) new_value)->getValue();
					dbValue->setDouble(doubleValue);
					*ec << "[QE] < query := ('double'); > operation";
					break;
				}
				case QueryResult::QSTRING: {
					string stringValue = ((QueryStringResult *) new_value)->getValue();
					dbValue->setString(stringValue);
					*ec << "[QE] < query := ('string'); > operation";
					break;
				}
				case QueryResult::QREFERENCE: {
					LogicalID* refValue = ((QueryReferenceResult *) new_value)->getValue();
					dbValue->setPointer(refValue);
					*ec << "[QE] < query := ('reference'); > operation";
					break;
				}
				case QueryResult::QNOTHING: {
					vector<LogicalID*> emptyVector;
					dbValue->setVector(&emptyVector);
					*ec << "[QE] < query := {}; > operation";
					break;
				}
				default: {
					*ec << "[QE] combine() operator <assign>: error, bad type right argument evaluated by executeQuery";
					*ec << (EBadType | ErrQExecutor);
					QueryResult *sth = new QueryNothingResult();
					partial->addResult(sth);
					return EBadType | ErrQExecutor;
				}
			}
			DataValue* value;
			value = dbValue;
			old_optr->setValue(value);
			*ec << "[QE] combine() operator <assign>: value of the object was changed";
			QueryResult *sth = new QueryNothingResult();
			partial->addResult(sth);
			break;
		}
		default: {
			*ec << "[QE] combine(): unknown NonAlgebraic operator!";
			return -1;
			break;
		}
	}
	return 0;
}

int QueryExecutor::merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final) {
	*ec << "[QE] merge() function applied to the partial results, creating final result";
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			*ec << "[QE] merge(): NonAlgebraic operator <dot>";
			final = partial;
			break;
		}
		case NonAlgOpNode::join: {
			*ec << "[QE] merge(): NonAlgebraic operator <join>";
			final = partial;
			break;
		}
		case NonAlgOpNode::where: {
			*ec << "[QE] merge(): NonAlgebraic operator <where>";
			final = partial;
			break;
		}
		case NonAlgOpNode::closeBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <closeBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <closeUniqueBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::leavesBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <leavesBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet";
			break;
		}
		case NonAlgOpNode::orderBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <orderBy>";
			final = new QuerySequenceResult();
			errcode = ((QuerySequenceResult *) final)->sortCollection(partial);
			if (errcode != 0) return errcode;
			break;
		}
		case NonAlgOpNode::exists: {
			*ec << "[QE] merge(): NonAlgebraic operator <exists>";
			bool tmp_bool = false;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					*ec << "[QE] merge(): ERROR! operator <exists> expects boolean type arguments";
					return -1; //something went odd, partial results should be boolean type
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
			*ec << "[QE] merge(): NonAlgebraic operator <forAll>";
			bool tmp_bool = true;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					*ec << "[QE] merge(): ERROR! operator <forAll> expects boolean type arguments";
					return -1; //something went odd, partial results should be boolean type
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
		case NonAlgOpNode::assign: {
			*ec << "[QE] merge(): NonAlgebraic operator <assign>";
			QueryResult *sth = new QueryNothingResult();
			final->addResult(sth);
			break;
		}
		default: {
			*ec << "[QE] merge(): unknown NonAlgebraic operator!";
			return -1;
			break;
		}
	}
	return 0;
}

int QueryExecutor::isIncluded(QueryResult *elem, QueryResult *set, bool &score) {
	*ec << "[QE] isIncluded()";
	int errcode;
	if (set->type() != QueryResult::QBAG) {
		*ec << "[QE] isIncluded(): ERROR! set argument must be BAG";
		return -1;
	}
	QueryResult *derefElem;
	errcode = this->derefQuery(elem,derefElem);
	if (errcode != 0) return errcode;
	bool tmp_bool = false;
	for (unsigned int i = 0; i < set->size(); i++) {
		QueryResult *tmp_res;
		errcode = ((QueryBagResult *) set)->at(i, tmp_res);
		if (errcode != 0) return errcode;
		QueryResult *derefSetElem;
		errcode = this->derefQuery(tmp_res, derefSetElem);
		if (errcode != 0) return errcode;
		bool eq = derefElem->equal(derefSetElem);
		if (eq) 
			tmp_bool = true;
	} 
	score = tmp_bool;
	return 0;
}

QueryExecutor::~QueryExecutor() { tr->abort(); stack.deleteAll(); delete ec; }

EnvironmentStack::EnvironmentStack() { ec = new ErrorConsole("QueryExecutor"); }
EnvironmentStack::~EnvironmentStack() { delete ec; }

int EnvironmentStack::push(QueryBagResult *r) {
	es.push_back(r);
	*ec << "[QE] Environment Stack pushed";
	return 0;
}

int EnvironmentStack::pop(){ 
	if (es.empty()) { 
		return -1; 
	} 
	else {
		es.pop_back();
	};
	*ec << "[QE] Environment Stack poped"; 
	return 0;
}

int EnvironmentStack::top(QueryBagResult *&r) {
	if (es.empty()) {
		return -1;
	}
	else {
		r=(es.back());
	};
	return 0;
}

bool EnvironmentStack::empty() { return es.empty(); }
int EnvironmentStack::size() { return es.size(); }

int EnvironmentStack::bindName(string name, QueryResult *&r) {
	int errcode;
	*ec << "[QE] Name binding on ES";
	unsigned int number = (es.size());
	ec->printf("[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	bool found_one = false;
	QueryBagResult *section;
	unsigned int sectionSize;
	QueryResult *sth;
	string current;
	for (unsigned int i = number; i >= 1; i--) {
		section = (es.at(i - 1));
		sectionSize = (section->size());
		ec->printf("[QE] bindName: ES section %u got %u elements\n", (i - 1), sectionSize);
		for (unsigned int j = 0; j < sectionSize; j++) {
			errcode = (section->at(j,sth));
			if (errcode != 0) return errcode;
			if ((sth->type()) == (QueryResult::QBINDER)) {
				current = (((QueryBinderResult *) sth)->getName());
				ec->printf("[QE] bindName: current %u name is: %s\n", j, current.c_str());
				if (current == name) {
					found_one = true;
					*ec << "[QE] bindName: Object added to Result";
					r->addResult(((QueryBinderResult *) sth)->getItem());
				}
			}
		}
		if (found_one) {
			return 0;
		}
	}
	return 0;
}

int EnvironmentStack::bindName(string name, int es_section, QueryResult *&r) {
	*ec << "[QE] Name binding on ES";
	int errcode;
	int number = (es.size());
	ec->printf("[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	QueryBagResult *section;
	unsigned int sectionSize;
	if ((es_section < 0) || (es_section >= number)) {
		*ec << "[QE] bindName ERROR: ES section number given by optimalizator is wrong!";
		return -1;
	}
	*ec << "[QE] bindName: searching section number %d - optimalizator said, that i should search there";
	QueryResult *sth;
	string current;
	section = (es.at(es_section));
	sectionSize = (section->size());
	ec->printf("[QE] bindName: ES section %u got %u elements\n", es_section, sectionSize);
	for (unsigned int j = 0; j < sectionSize; j++) {
		errcode = (section->at(j,sth));
		if (errcode != 0) return errcode;
		if ((sth->type()) == (QueryResult::QBINDER)) {
			current = (((QueryBinderResult *) sth)->getName());
			ec->printf("[QE] bindName: current %u name is: %s\n", j, current.c_str());
			if (current == name) {
				*ec << "[QE] bindName: Object added to Result";
				r->addResult(((QueryBinderResult *) sth)->getItem());
			}
		}
	}
	return 0;
}

void EnvironmentStack::deleteAll() {
	for (unsigned int i = 0; i < (es.size()); i++) {
		delete es.at(i);
	};
}

}
