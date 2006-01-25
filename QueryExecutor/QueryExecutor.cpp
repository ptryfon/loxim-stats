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
	ErrorConsole ec;
	int errcode;
	fprintf(stderr, "[QE] executeQuery()\n");
	
	if (tree != NULL)
	{
		if (tr == NULL) {
			fprintf(stderr, "[QE] Asking TransactionManager for a new transaction\n");
			if ((errcode = TransactionManager::getHandle()->createTransaction(tr)) != 0) {
				fprintf(stderr, "Error in createTransaction\n");
    				return errcode;	
			}
			else {
				fprintf(stderr, "[QE] Transaction opened\n");
			}
		}
		int nodeType = tree->type();
		fprintf(stderr, "[QE] TreeType taken\n");
	
		switch (nodeType) 
		{
		case TreeNode::TNNAME: {
			string name = tree->getName();
			fprintf(stderr, "[QE] Type: TNNAME\n");
			
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
				fprintf(stderr, "[QE] All %d Roots taken\n", vecSize);
				QueryBagResult *stackSection = new QueryBagResult();
				for (int i = 0; i < vecSize; i++ )
					{
   					ObjectPointer *optr = vec->at(i);
					LogicalID *lid = optr->getLogicalID();
					fprintf(stderr, "[QE] LogicalID received\n");
					string optrName = (optr->getName());
					fprintf(stderr, "[QE] Name received\n");
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
			fprintf(stderr, "[QE] Done!\n");
			return 0;
		}//case TNNAME

		case TreeNode::TNCREATE: {
			string name = tree->getName();
			DataValue* value;
			fprintf(stderr, "[QE] Type: TNCREATE\n");
			tree = tree->getArg();
			fprintf(stderr, "[QE] Getting node arguments\n");
			QueryResult* nextResult; 
			if ((errcode = executeQuery (tree, &nextResult)) != 0)
				{
				return errcode;
				}
			
			DBDataValue *dbValue = new DBDataValue;
			
			if (tree != NULL)
				{
				nodeType = tree->type();
				fprintf(stderr, "[QE] Node type recognized.\n");

				switch (nodeType) 
				{
				case TreeNode::TNINT:
					{
					int intValue = ((IntNode *) tree)->getValue();
					dbValue->setInt(intValue);
					fprintf(stderr, "[QE] It's 'integer'.\n");
					break;
					}
			
				case TreeNode::TNSTRING: 
					{
					string stringValue = ((StringNode *) tree)->getValue();
					dbValue->setString(stringValue);
					fprintf(stderr, "[QE] It's 'string'.\n");
					break;
					}

				case TreeNode::TNDOUBLE: 
					{
					double doubleValue = ((DoubleNode *) tree)->getValue();
					dbValue->setDouble(doubleValue);
					fprintf(stderr, "[QE] It's 'double'.\n");
					break;
					} 
				default:
					{
					ec << (EBadType | ErrQExecutor);
					fprintf(stderr, "[QE] Error - incorrect node type.\n");
					return EBadType | ErrQExecutor;
					}
			
				}//switch
				value = dbValue;
				fprintf(stderr, "[QE] Value set\n");
				} //if
			else
				{
				vector<LogicalID*> emptyVector;
				dbValue->setVector(&emptyVector);
				value = dbValue;
				fprintf(stderr, "[QE] No arguments (value = NULL)\n");
				}
			ObjectPointer *optr;
			if ((errcode = tr->createObject(name, value, optr)) != 0)
				{
				fprintf(stderr, "[QE] Error in createObject\n");
				return errcode;
				}
			if ((errcode = tr->addRoot(optr)) != 0)
				{
				fprintf(stderr, "[QE] Error in addRoot\n");
				return errcode;
				}
			*result = new QueryBagResult;
			fprintf(stderr, "[QE] QueryBagResult created\n");
			QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
			(*result)->addResult (lidres);
			fprintf(stderr, "[QE] Object added to QueryResult. Done!\n");
			return 0;
		} //case TNCREATE

		case TreeNode::TNINT: {
			int intValue = ((IntNode *) tree)->getValue();
			fprintf(stderr, "[QE] TNINT: %d\n", intValue);
			*result = new QueryBagResult();
			QueryIntResult *tmpResult = new QueryIntResult(intValue);
			(*result)->addResult(tmpResult);
			fprintf(stderr, "[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
		} //case TNINT
		
		case TreeNode::TNSTRING: {
			string stringValue = ((StringNode *) tree)->getValue();
			fprintf(stderr, "[QE] QueryStringResult created\n");
			*result = new QueryBagResult();
			QueryStringResult *tmpResult = new QueryStringResult(stringValue);
			(*result)->addResult(tmpResult);
			fprintf(stderr, "[QE] QueryStringResult created\n");
			return 0;
		} //case TNSTRING
		
		case TreeNode::TNDOUBLE: {
			double doubleValue = ((DoubleNode *) tree)->getValue();
			fprintf(stderr, "[QE] TNDOUBLE: %f\n", doubleValue);
			*result = new QueryBagResult;
			QueryDoubleResult *tmpResult = new QueryDoubleResult(doubleValue);
			(*result)->addResult(tmpResult);
			fprintf(stderr, "[QE] QueryDoubleResult (%f) created\n", doubleValue);
			return 0;
		} //case TNDOUBLE
			
		case TreeNode::TNVECTOR: {
			fprintf(stderr, "[QE] tree - Vector: not implemented, don't know what to do\n");
			*result = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return 0;
		} //case TNVECTOR
		
		case TreeNode::TNAS: {
			fprintf(stderr, "[QE] AS operator: not implemented yet\n");
			*result = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return 0;
		} //case TNAS
	
		case TreeNode::TNUNOP: {
			UnOpNode::unOp op = ((UnOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] Unary operator - type recognized\n");
			QueryResult *tmp_result;
			if ((errcode = executeQuery (tree->getArg(), &tmp_result)) != 0) {
				return errcode;
			}
			QueryResult *op_result;
			errcode = this->unOperate(op, tmp_result, op_result);
			if (errcode != 0) return errcode;
			QueryResult *final_result = new QueryBagResult();
			final_result->addResult(op_result);
			*result = final_result;
			fprintf(stderr, "[QE] Unary operation Done!\n");    
			return 0;
		} //case TNUNOP 
	
		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] Algebraic operator - type recognized\n");
			QueryResult *lResult, *rResult;
			if ((errcode = executeQuery (((AlgOpNode *) tree)->getLArg(), &lResult)) != 0) {
				return errcode;
			}
			if ((errcode = executeQuery (((AlgOpNode *) tree)->getRArg(), &rResult)) != 0) {
				return errcode;
			}
			QueryResult *op_result;
			errcode = this->algOperate(op, lResult, rResult, op_result);
			if (errcode != 0) return errcode;
			QueryResult *final_result = new QueryBagResult();
			final_result->addResult(op_result);
			*result = final_result;
			fprintf(stderr, "[QE] Algebraic operation Done!\n");
			return 0;
		} // TNALGOP

		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			fprintf(stderr, "[QE] NonAlgebraic operator - type recognized\n");
			QueryResult *l_tmp_Result;
			if ((errcode = executeQuery (((NonAlgOpNode *) tree)->getLArg(), &l_tmp_Result)) != 0) {
				return errcode;
			}
			QueryResult *lResult;
			if (((l_tmp_Result->type()) != QueryResult::QSEQUENCE) && ((l_tmp_Result->type()) != QueryResult::QBAG)) {
				lResult = new QueryBagResult();
				lResult->addResult(l_tmp_Result);
			}
			else
				lResult = l_tmp_Result;
			fprintf(stderr, "[QE] Left argument of NonAlgebraic query has been computed\n");
			QueryResult *partial_result = new QueryBagResult();
			if (((lResult->type()) == QueryResult::QSEQUENCE) || ((lResult->type()) == QueryResult::QBAG)) {
				fprintf(stderr, "[QE] For each row of this score, the right argument will be computed \n");
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
					fprintf(stderr, "[QE] nested(): function calculated for current row number %d\n", i);
					if ( (stack.push((QueryBagResult *) newStackSection)) != 0 ) {
						return -1; //this would be very strange, this function can only return 0
					}
					QueryResult *rResult;
					if ((errcode = executeQuery (((NonAlgOpNode *) tree)->getRArg(), &rResult)) != 0) {
						return errcode;
					}
					fprintf(stderr, "[QE] Computing left Argument with a new scope of ES\n");
					errcode = this->combine(op,currentResult,rResult,partial_result);
					if (errcode != 0) return errcode;
					fprintf(stderr, "[QE] Combined partial results\n");
					errcode = stack.pop();
					if (errcode != 0) return errcode;
				}
			}
			else {
				fprintf(stderr, "[QE] ERROR! NonAlgebraic operation, bad left argument\n");
				*result = new QueryNothingResult();
				return -1;
			}
			QueryResult *final_result = new QueryBagResult();
			errcode = this->merge(op,partial_result, final_result);
			if (errcode != 0) return errcode;
			fprintf(stderr, "[QE] Merged partial results into final result\n");
			*result = final_result;
			fprintf(stderr, "[QE] NonAlgebraic operation Done!\n");
			return 0;
		} //case TNNONALGOP

		case TreeNode::TNTRANS: {
			// begin, abort and commit
			*result = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return 0;
		} //case TNTRANS

		default:
			{
			fprintf(stderr, "Unknow node type\n");
			ec << (ENoType | ErrQExecutor);
			*result = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
			return ENoType | ErrQExecutor;
			}

		} // end of switch
		
	} // if tree!=Null
	else { // tree == NULL; return empty result
		fprintf(stderr, "[QE] empty tree, nothing to do\n");
		*result = new QueryNothingResult();
		fprintf(stderr, "[QE] QueryNothingResult created\n");
	}
	return 0;
    }//executeQuerry

int QueryExecutor::derefQuery(QueryResult *arg, QueryResult *&res) {
	fprintf(stderr, "[QE] derefQuery()\n");
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
			fprintf(stderr, "[QE] derefQuery() - dereferencing Object\n");
			ObjectPointer *optr;
			errcode = tr->getObjectPointer(ref_value, Store::Read, optr);
			if (errcode != 0) {
				fprintf(stderr, "[QE] Error in getObjectPointer\n");
				return errcode;
			}
			DataValue* value = optr->getValue();
			int vType = value->getType();
			switch (vType) {
				case Store::Integer: {
					fprintf(stderr, "[QE] derefQuery() - ObjectValue = Int\n");
					int tmp_value = value->getInt();
					res = new QueryIntResult(tmp_value);
					break;
				}
				case Store::Double: {
					fprintf(stderr, "[QE] derefQuery() - ObjectValue = Double\n");
					double tmp_value = value->getDouble();
					res = new QueryDoubleResult(tmp_value);
					break;
				}
				case Store::String: {
					fprintf(stderr, "[QE] derefQuery() - ObjectValue = String\n");
					string tmp_value = value->getString();
					res = new QueryStringResult(tmp_value);
					break;
				}
				default: {
					fprintf(stderr, "[QE] derefQuery() - wrong argument type\n");
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
			fprintf(stderr, "[QE] ERROR in derefQuery() - unknown result type\n");
			res = new QueryNothingResult();
			fprintf(stderr, "[QE] QueryNothingResult created\n");
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
			fprintf(stderr, "[QE] DEREF operation\n");
			errcode = derefQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::unMinus: {
			fprintf(stderr, "[QE] UN_MINUS operation\n");
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
				fprintf (stderr, "[QE] ERROR! UN_MINUS argument must be INT or DOUBLE\n");
				final = new QueryNothingResult();
				return -1;
			}
			break;
		}
		case UnOpNode::boolNot: {
			fprintf(stderr, "[QE] NOT operation\n");
			if (arg->isBool()) {
				bool bool_tmp;
				errcode = arg->getBoolValue(bool_tmp);
				if (errcode != 0) return errcode;
				final = new QueryBoolResult(not bool_tmp);
			}
			else {
				fprintf(stderr, "[QE] ERROR! NOT argument must be BOOLEAN\n");
				final = new QueryNothingResult();
				return -1;
			}
			break;
		}
		case UnOpNode::count: {
			fprintf(stderr, "[QE] COUNT operation\n");
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			int count_res = bagArg->size();
			final = new QueryIntResult(count_res);
			break;
		}
		case UnOpNode::sum: {
			fprintf(stderr, "[QE] SUM operation\n");
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
			fprintf(stderr, "[QE] AVG operation\n");
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
			fprintf(stderr, "[QE] MIN operation\n");
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_min_res = 0;
			bool i_counted = false;
			double d_min_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				fprintf(stderr, "[QE] MIN operation - can't evaluate min value on this set, QueryNothingResult created\n");
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
					fprintf(stderr, "[QE] MIN - can't evaluate min value on this set, QueryNothingResult created\n");
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::max: {
			fprintf(stderr, "[QE] MAX operation\n");
			QueryResult *bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			unsigned int count_res = bagArg->size();
			int i_max_res = 0;
			bool i_counted = false;
			double d_max_res = 0.0;
			bool d_counted = false;
			if (count_res == 0) {
				fprintf(stderr, "[QE] MAX operation - can't evaluate max value on this set, QueryNothingResult created\n");
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
					fprintf(stderr, "[QE] MAX - can't evaluate max value on this set, QueryNothingResult created\n");
					final = new QueryNothingResult();
				}
			}
			break;
		}
		case UnOpNode::distinct: {
			fprintf(stderr, "[QE] DISTINCT operation\n");
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
			fprintf(stderr, "[QE] DELETE operation\n");
			QueryResult* bagArg = new QueryBagResult();
			bagArg->addResult(arg);
			for (unsigned int i = 0; i < bagArg->size(); i++) {
				QueryResult* toDelete;  //the object to be deleted
				errcode = ((QueryBagResult *) bagArg)->at(i, toDelete);
				if (errcode != 0) return errcode;
				int toDeleteType = toDelete->type();
				if (toDeleteType != QueryResult::QREFERENCE) {
					fprintf(stderr, "[QE] ERROR! DELETE argument must consist of QREFERENCE\n");
					final = new QueryNothingResult();
					return -1;
				}
				LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
				ObjectPointer *optr;
				if ((errcode = tr->getObjectPointer (lid, Store::Write, optr)) !=0) {
					fprintf(stderr, "[QE] Error in getObjectPointer.\n");
					return errcode;
				}
				if ((errcode = tr->removeRoot(optr)) != 0) {
					fprintf(stderr, "[QE] Error in removeRoot.\n");
					return errcode;
				}
				fprintf(stderr, "[QE] Root removed\n");
				if ((errcode = tr->deleteObject(optr)) != 0) {
					fprintf(stderr, "[QE] Error in deleteObject.\n");
					return errcode;
				}
				fprintf(stderr, "[QE] Object deleted\n");
			}
			fprintf(stderr, "[QE] delete operation complete - QueryNothingResult created\n");
			final = new QueryNothingResult();
			break;
		}
		default: { // unOperation type not known
			fprintf(stderr, "[QE] ERROR! Unary operation type not known\n");
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
					fprintf(stderr, "[QE] + operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->plus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->plus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::minus: {
					fprintf(stderr, "[QE] - operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->minus(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->minus(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::times: {
					fprintf(stderr, "[QE] * operation\n");
					if (derefL->type() == QueryResult::QINT)
						errcode = ((QueryIntResult *)derefL)->times(derefR,final);
					else
						errcode = ((QueryDoubleResult *)derefL)->times(derefR,final);
					if (errcode != 0) return errcode;
					break;
				}
				case AlgOpNode::divide: {
					fprintf(stderr, "[QE] / operation\n");
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
					fprintf (stderr, "[QE] ERROR! + arguments must be INT or DOUBLE\n");
					return -1;
				}
				case AlgOpNode::minus: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! - arguments must be INT or DOUBLE\n");
					return -1;
				}
				case AlgOpNode::times: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! * arguments must be INT or DOUBLE\n");
					return -1;
				}
				case AlgOpNode::divide: {
					final = new QueryNothingResult();
					fprintf (stderr, "[QE] ERROR! / arguments must be INT or DOUBLE\n");
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
				fprintf(stderr, "[QE] = operation\n");
				bool bool_tmp = derefL->equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::neq: {
				fprintf(stderr, "[QE] != operation\n");
				bool bool_tmp = derefL->not_equal(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::lt: {
				fprintf(stderr, "[QE] < operation\n");
				bool bool_tmp = derefL->less_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::gt: {
				fprintf(stderr, "[QE] > operation\n");
				bool bool_tmp = derefL->greater_than(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::le: {
				fprintf(stderr, "[QE] <= operation\n");
				bool bool_tmp = derefL->less_eq(derefR);
				final = new QueryBoolResult(bool_tmp);
				break;
			}
			case AlgOpNode::ge: {
				fprintf(stderr, "[QE] >= operation\n");
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
					fprintf(stderr, "[QE] OR operation\n");
					bool bool_tmp = (bool_l || bool_r);
					final = new QueryBoolResult(bool_tmp);
					break;
				}
				case AlgOpNode::boolAnd: {
					fprintf(stderr, "[QE] AND operation\n");
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
					fprintf(stderr, "[QE] ERROR! OR arguments must be BOOLEAN\n");
					final = new QueryNothingResult();
					return -1;
				}
				case AlgOpNode::boolAnd: {
					fprintf(stderr, "[QE] ERROR! AND arguments must be BOOLEAN\n");
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
				fprintf(stderr, "[QE] BAG_UNION operation\n");
				final = rightBag;
				break;
			}
			case AlgOpNode::bagIntersect: {
				fprintf(stderr, "[QE] BAG_INTERSECT operation\n");
				final = new QueryBagResult();
				break;
			}
			case AlgOpNode::bagMinus: {
				fprintf(stderr, "[QE] BAG_MINUS operation\n");
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
		fprintf(stderr, "[QE] COMMA operation\n");
		final = new QueryBagResult();
		errcode = lArg->comma(rArg,final);
		if (errcode != 0) return errcode;
		return 0;
	}
	else if (op == AlgOpNode::insert) {
		fprintf(stderr, "[QE] INSERT operation\n");
		DataValue* db_value;
		vector<LogicalID*> *insVector;
		QueryResult *lBag = new QueryBagResult();
		QueryResult *rBag = new QueryBagResult();
		lBag->addResult(lArg);
		rBag->addResult(rArg);
		for (unsigned int i = 0; i < lBag->size(); i++) {
			for (unsigned int j = 0; j < rBag->size(); j++) {
				fprintf(stderr, "[QE] trying to INSERT %d element of leftArg into %d element of rightArg\n", i, j);
				QueryResult* toInsert;  //the object to be inserted
				errcode = ((QueryBagResult *) lBag)->at(i, toInsert);
				if (errcode != 0) return errcode;
				QueryResult* outer;  //the object into which the left agument will be inserted
				errcode = ((QueryBagResult *) rBag)->at(j, outer);
				if (errcode != 0) return errcode;
				int leftResultType = toInsert->type();
				int rightResultType = outer->type();
				if ((leftResultType != QueryResult::QREFERENCE) || (rightResultType != QueryResult::QREFERENCE)) {
					fprintf(stderr, "[QE] ERROR! both arguments of insert operation must consist of QREFERENCE\n");
					final = new QueryNothingResult();
					return -1;
				}
				LogicalID *lidIn = ((QueryReferenceResult *) toInsert)->getValue();
				LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
				ObjectPointer *optrIn;
				errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn);
				if (errcode != 0) {
					fprintf(stderr, "[QE] insert operation - Error in getObjectPointer.\n");
					return errcode;
				}
				fprintf(stderr, "[QE] insert operation - getObjectPointer on leftArg done\n");
				ObjectPointer *optrOut;
				errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut);
				if (errcode != 0) {
					fprintf(stderr, "[QE] insert operation - Error in getObjectPointer.\n");
					return errcode;
				}
				fprintf(stderr, "[QE] insert operation - getObjectPointer on rightArg done\n");
				// we have to cut left argument from the roots
				errcode = tr->removeRoot(optrIn);
				if (errcode != 0) {
					fprintf(stderr, "[QE] insert operation - Error in removeRoot.\n");
					return errcode;
				}
				fprintf(stderr, "[QE] insert operation - Root removed\n");
				// now we have to modify right argument's value
				db_value = optrOut->getValue();
				fprintf(stderr, "[QE] insert operation - Value taken\n");
				int vType = db_value->getType();
				fprintf(stderr, "[QE] insert operation - Type of this value taken\n");
				if (vType != Store::Vector) {
					fprintf(stderr, "[QE] insert operation - ERROR! the Value has to be a Vector\n");
					return -1;
				}
				insVector = db_value->getVector();
				if (insVector == NULL) {
					fprintf(stderr, "[QE] insert operation - insVector == NULL\n");
					return -1;
				}
				fprintf(stderr, "[QE] Vector taken\n");
				fprintf(stderr, "[QE] Vec.size = %d\n", insVector->size());
				insVector->push_back(lidIn); // Inserting of the object
				fprintf(stderr, "[QE] %d element of leftArg INSERTED INTO %d element of rightArg SUCCESFULLY\n", i, j);
				fprintf(stderr, "[QE] New Vec.size = %d\n", insVector->size());
			}
		}
		fprintf(stderr, "[QE] INSERT operation Done. QueryNothingResult created.\n");
		final = new QueryNothingResult();
		return 0;
	}
	else { // algOperation type not known
		fprintf(stderr, "[QE] ERROR! Algebraic operation type not known\n");
		final = new QueryNothingResult();
		return -1;
	}
	return 0;
}

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *lRes, QueryResult *&partial) {
	fprintf(stderr, "[QE] combine() function applied to the partial results\n");
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <dot>\n");
			partial->addResult(lRes);
			break;
		}
		case NonAlgOpNode::join: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <join>\n");
			errcode = curr->comma(lRes,partial);
			if (errcode != 0) return errcode;
			break;
		}
		case NonAlgOpNode::where: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <where>\n");
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				if (tmp_bool_value) 
					partial->addResult(curr);
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <where> is not boolean!\n");
				return -1; // second argument of the <where> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::closeBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <closeBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <closeUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <leavesBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::orderBy: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <orderBy>\n");
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
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <exists>\n");
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryBoolResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!\n");
				return -1; // second argument of the <exists> operator must be boolean type
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			fprintf(stderr, "[QE] combine(): NonAlgebraic operator <forAll>\n");
			if (lRes->isBool()) {
				bool tmp_bool_value;
				errcode = (lRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryBoolResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				fprintf(stderr, "[QE] combine(): ERROR! Right argument of operator <forAll> is not boolean!\n");
				return -1; // second argument of the <forAll> operator must be boolean type
			}
			break;
		}
		default: {
			fprintf(stderr, "[QE] combine(): unknown NonAlgebraic operator!\n");
			return -1;
			break;
		}
	}
	return 0;
}

int QueryExecutor::merge(NonAlgOpNode::nonAlgOp op, QueryResult *partial, QueryResult *&final) {
	fprintf(stderr, "[QE] merge() function applied to the partial results, creating final result\n");
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <dot>\n");
			final = partial;
			break;
		}
		case NonAlgOpNode::join: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <join>\n");
			final = partial;
			break;
		}
		case NonAlgOpNode::where: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <where>\n");
			final = partial;
			break;
		}
		case NonAlgOpNode::closeBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <closeBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::closeUniqueBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <closeUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <leavesBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::leavesUniqueBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <leavesUniqueBy> not imlemented yet\n");
			break;
		}
		case NonAlgOpNode::orderBy: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <orderBy>\n");
			final = new QuerySequenceResult();
			errcode = ((QuerySequenceResult *) final)->sortCollection(partial);
			if (errcode != 0) return errcode;
			break;
		}
		case NonAlgOpNode::exists: {
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <exists>\n");
			bool tmp_bool = false;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					fprintf(stderr, "[QE] merge(): ERROR! operator <exists> expects boolean type arguments\n");
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
			fprintf(stderr, "[QE] merge(): NonAlgebraic operator <forAll>\n");
			bool tmp_bool = true;
			bool current_bool;
			for (unsigned i = 0; i < (partial->size()); i++) {
				QueryResult *tmp_result;
				errcode = (((QueryBagResult *) partial)->at(i,tmp_result));
				if (errcode != 0) return errcode;
				if (not (tmp_result->isBool())) {
					fprintf(stderr, "[QE] merge(): ERROR! operator <forAll> expects boolean type arguments\n");
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
		default: {
			fprintf(stderr, "[QE] merge(): unknown NonAlgebraic operator!\n");
			return -1;
			break;
		}
	}
	return 0;
}

int QueryExecutor::isIncluded(QueryResult *elem, QueryResult *set, bool &score) {
	fprintf(stderr, "[QE] isIncluded()\n");
	int errcode;
	if (set->type() != QueryResult::QBAG) {
		fprintf(stderr, "[QE] isIncluded(): ERROR! set argument must be BAG\n");
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

QueryExecutor::~QueryExecutor() { tr->abort(); stack.deleteAll(); }

EnvironmentStack::EnvironmentStack() {}
EnvironmentStack::~EnvironmentStack() {}

int EnvironmentStack::push(QueryBagResult *r) {
	es.push_back(r);
	fprintf(stderr, "[QE] Environment Stack pushed\n");
	return 0;
}

int EnvironmentStack::pop(){ 
	if (es.empty()) { 
		return -1; 
	} 
	else {
		es.pop_back();
	};
	fprintf(stderr, "[QE] Environment Stack poped\n"); 
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
	fprintf(stderr, "[QE] Name binding on ES\n");
	unsigned int number = (es.size());
	fprintf(stderr, "[QE] bindName: ES got %u sections\n", number);
	r = new QueryBagResult();
	bool found_one = false;
	QueryBagResult *section;
	unsigned int sectionSize;
	QueryResult *sth;
	string current;
	for (unsigned int i = number; i >= 1; i--) {
		section = (es.at(i - 1));
		sectionSize = (section->size());
		fprintf(stderr, "[QE] bindName: ES section %u got %u elements\n", (i - 1), sectionSize);
		for (unsigned int j = 0; j < sectionSize; j++) {
			int errNo = (section->at(j,sth));
			if (errNo != 0) { return errNo; };
			if ((sth->type()) == (QueryResult::QBINDER)) {
				current = (((QueryBinderResult *) sth)->getName());
				fprintf(stderr, "[QE] bindName: current %u name is: ", j);
				cout << current << endl;
				if (current == name) {
					found_one = true;
					fprintf(stderr, "[QE] bindName: Object added to Result\n");
					r->addResult(((QueryBinderResult *) sth)->getItem());
				};
			};
		};
		if (found_one) {
			return 0;
		};
	};
	return 0;
}

void EnvironmentStack::deleteAll() {
	for (unsigned int i = 0; i < (es.size()); i++) {
		delete es.at(i);
	};
}

}
