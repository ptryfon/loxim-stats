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
						tr = NULL;
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
		switch (nodeType) // tr != NULL
		{
		case TreeNode::TNTRANS: {
			TransactNode::transactionOp op = ((TransactNode *) tree)->getOp();
			switch (op) {
				case TransactNode::begin: {
					*ec << "[QE] ERROR! Transaction already opened. It can't be opened once more!";
					*ec << "[QE] maybe someday, when nested transactions will be implemented...";
					*result = new QueryNothingResult();
					*ec << (ErrQExecutor | EQEUnexpectedErr);
					return ErrQExecutor | EQEUnexpectedErr;
				}
				case TransactNode::end: {
					errcode = tr->commit();
					tr = NULL;
					if (errcode != 0) {
						*ec << "[QE] error in transaction->commit()";
						*result = new QueryNothingResult();
						return errcode;
					}
					*ec << "[QE] Transaction commited succesfully";
					break;
				}
				case TransactNode::abort: {
					errcode = tr->abort();
					tr = NULL;
					if (errcode != 0) {
						*ec << "[QE] error in transaction->abort()";
						*result = new QueryNothingResult();
						return errcode;
					}
					*ec << "[QE] Transaction aborted succesfully";
					break;
				}
				default: {
					*ec << "[QE] ERROR! unknown transaction operator";
					*result = new QueryNothingResult();
					*ec << (ErrQExecutor | EUnknownNode);
					return ErrQExecutor | EUnknownNode;
				}
			}
			*result = new QueryNothingResult();
			*ec << "[QE] Nothing else to do. QueryNothingResult created";
			return 0;
		} //case TNTRANS

		default: {
			QueryResult *tmp_result;
			errcode = this->executeRecQuery(tree);
			if (errcode != 0) return errcode;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			*result = tmp_result;
			*ec << "[QE] Done!";
			return 0;
			}

		} // end of switch
		
	} // if tree!=Null
	else { // tree == NULL; return empty result
		*ec << "[QE] empty tree, nothing to do";
		*result = new QueryNothingResult();
		*ec << "[QE] QueryNothingResult created";
	}
	return 0;
}

int QueryExecutor::executeRecQuery(TreeNode *tree) {
	
	int errcode;
	*ec << "[QE] executeRecQuery()";
	
	if (this->evalStopped()) {
		*ec << "[QE] query evaluating stopped by Server, aborting transaction ";
		errcode = tr->abort();
		tr = NULL;
		if (errcode != 0) {
			*ec << "[QE] error in transaction->abort()";
			return errcode;
		}
		*ec << "[QE] Transaction aborted succesfully";
		*ec << (ErrQExecutor | EQEUnexpectedErr);
		return ErrQExecutor | EQEUnexpectedErr;
	}
	else {
		*ec << "[QE] query evaluating NOT stopped by Server (RECURSIVE CHECK) ";
	}
	
	if (tree != NULL) {
		int nodeType = tree->type();
		*ec << "[QE] TreeType taken";
		switch (nodeType) 
		{
		case TreeNode::TNNAME: {
			*ec << "[QE] Type: TNNAME";
			string name = tree->getName();
			int sectionNumber = ((NameNode *) tree)->getBindSect();
			QueryResult *result = new QueryBagResult();
			errcode = envs->bindName(name, sectionNumber, result);
			if (errcode != 0) return errcode;
			if ((result->size()) == 0) {
				vector<LogicalID*>* vec;
				if ((errcode = tr->getRootsLID(name, vec)) != 0) {
					tr->abort();
					tr = NULL;
					return errcode;
				}
				int vecSize = vec->size();
				ec->printf("[QE] %d Roots LID by name taken\n", vecSize);
				for (int i = 0; i < vecSize; i++ ) {
					LogicalID *lid = vec->at(i);
					*ec << "[QE] LogicalID received";
					QueryReferenceResult *lidres = new QueryReferenceResult(lid);
					result->addResult(lidres);
				}
				
				delete vec;
			}
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Done!";
			return 0;
		}//case TNNAME

		case TreeNode::TNCREATE: {
			*ec << "[QE] Type: TNCREATE";
			QueryResult *result = new QueryBagResult();
			string name = tree->getName();
			QueryResult *tmp_result;
			errcode = executeRecQuery (tree->getArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			QueryResult *bagRes = new QueryBagResult();
			bagRes->addResult(tmp_result);
			for (unsigned int i = 0; i < bagRes->size(); i++) {
				QueryResult *binder;
				errcode = ((QueryBagResult *) tmp_result)->at(i, binder);
				if (errcode != 0) return errcode;
				ObjectPointer *optr;
				errcode = this->objectFromBinder(binder, optr);
				if (errcode != 0) return errcode;
				if ((errcode = tr->addRoot(optr)) != 0) {
					*ec << "[QE] Error in addRoot";
					tr->abort();
					tr = NULL;
					return errcode;
				}
				QueryReferenceResult *lidres = new QueryReferenceResult(optr->getLogicalID());
				((QueryBagResult *)result)->addResult (lidres);
			}
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] CREATE Done!";
			return 0;
		} //case TNCREATE

		case TreeNode::TNINT: {
			int intValue = ((IntNode *) tree)->getValue();
			ec->printf("[QE] TNINT: %d\n", intValue);
			QueryResult *result = new QueryBagResult();
			QueryIntResult *tmpResult = new QueryIntResult(intValue);
			((QueryBagResult *)result)->addResult(tmpResult);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			ec->printf("[QE] QueryIntResult (%d) created\n",intValue);
			return 0;
		} //case TNINT
		
		case TreeNode::TNSTRING: {
			string stringValue = ((StringNode *) tree)->getValue();
			*ec << "[QE] QueryStringResult created";
			QueryResult *result = new QueryBagResult();
			QueryStringResult *tmpResult = new QueryStringResult(stringValue);
			((QueryBagResult *) result)->addResult(tmpResult);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] QueryStringResult created";
			return 0;
		} //case TNSTRING
		
		case TreeNode::TNDOUBLE: {
			double doubleValue = ((DoubleNode *) tree)->getValue();
			ec->printf("[QE] TNDOUBLE: %f\n", doubleValue);
			QueryResult *result = new QueryBagResult();
			QueryDoubleResult *tmpResult = new QueryDoubleResult(doubleValue);
			((QueryBagResult *)result)->addResult(tmpResult);
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			ec->printf("[QE] QueryDoubleResult (%f) created\n", doubleValue);
			return 0;
		} //case TNDOUBLE
			
		case TreeNode::TNVECTOR: {
			*ec << "[QE] tree - Vector: not implemented, don't know what to do";
			QueryResult *result = new QueryNothingResult();
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] QueryNothingResult created";
			return 0;
		} //case TNVECTOR
		
		case TreeNode::TNAS: {
			*ec << "[QE] AS/GROUP_AS operation recognized";
			string name = tree->getName();
			QueryResult *tmp_result;
			errcode = executeRecQuery(tree->getArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(tmp_result);
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
					*ec << "[QE] AS: new binder created and added to final result";
					final_result->addResult(tmp_binder);
				}
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] AS operation Done!";
			return 0;
		} //case TNAS
	
		case TreeNode::TNUNOP: {
			UnOpNode::unOp op = ((UnOpNode *) tree)->getOp();
			*ec << "[QE] Unary operator - type recognized";
			QueryResult *tmp_result;
			errcode = executeRecQuery(tree->getArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(tmp_result);
			if (errcode != 0) return errcode;
			QueryResult *op_result;
			errcode = this->unOperate(op, tmp_result, op_result);
			if (errcode != 0) return errcode;
			QueryResult *final_result = new QueryBagResult();
			final_result->addResult(op_result);
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Unary operation Done!";
			return 0;
		} //case TNUNOP 
	
		case TreeNode::TNALGOP: {
			AlgOpNode::algOp op = ((AlgOpNode *) tree)->getOp();
			*ec << "[QE] Algebraic operator - type recognized";
			QueryResult *lResult, *rResult;
			errcode = executeRecQuery(((AlgOpNode *) tree)->getLArg());
			if (errcode != 0) return errcode;
			errcode = executeRecQuery (((AlgOpNode *) tree)->getRArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(rResult);
			if (errcode != 0) return errcode;
			errcode = qres->pop(lResult);
			if (errcode != 0) return errcode;
			QueryResult *op_result;
			errcode = this->algOperate(op, lResult, rResult, op_result);
			if (errcode != 0) return errcode;
			QueryResult *final_result = new QueryBagResult();
			final_result->addResult(op_result);
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Algebraic operation Done!";
			return 0;
		} // TNALGOP

		case TreeNode::TNNONALGOP: {
			NonAlgOpNode::nonAlgOp op = ((NonAlgOpNode *) tree)->getOp();
			*ec << "[QE] NonAlgebraic operator - type recognized";
			QueryResult *l_tmp_Result;
			errcode = executeRecQuery (((NonAlgOpNode *) tree)->getLArg());
			if (errcode != 0) return errcode;
			errcode = qres->pop(l_tmp_Result);
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
			QueryResult *visited_result = new QueryBagResult();
			QueryResult *final_result = new QueryBagResult();
			if ((op == NonAlgOpNode::closeBy) || (op == NonAlgOpNode::closeUniqueBy) || (op == NonAlgOpNode::leavesBy) || (op == NonAlgOpNode::leavesUniqueBy)) {
				switch (op) {
					case NonAlgOpNode::closeBy: {
						*ec << "[QE] NonAlgebraic recursive operator <closeBy>";
						partial_result->addResult(lResult);
						final_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::closeUniqueBy: {
						*ec << "[QE] NonAlgebraic recursive operator <closeUniqueBy>";
						partial_result->addResult(lResult);
						final_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::leavesBy: {
						*ec << "[QE] NonAlgebraic recursive operator <leavesBy>";
						partial_result->addResult(lResult);
						break;
					}
					case NonAlgOpNode::leavesUniqueBy: {
						*ec << "[QE] NonAlgebraic recursive operator <leavesUniqueBy>";
						partial_result->addResult(lResult);
						visited_result->addResult(lResult);
						break;
					}
					default: {
						break;
					}
				}//switch
				while (not (partial_result->isEmpty())) {
				
					if (this->evalStopped()) {
						*ec << "[QE] query evaluating stopped by Server, aborting transaction ";
						errcode = tr->abort();
						tr = NULL;
						if (errcode != 0) {
							*ec << "[QE] error in transaction->abort()";
							return errcode;
						}
						*ec << "[QE] Transaction aborted succesfully";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return ErrQExecutor | EQEUnexpectedErr;
					}
					else {
						*ec << "[QE] query evaluating NOT stopped by Server (LOOP CHECK) ";
					}
					
					QueryResult *currentResult;
					errcode = partial_result->getResult(currentResult);
					if (errcode != 0) return errcode;
					QueryResult *newStackSection = new QueryBagResult();
					errcode = (currentResult)->nested(tr, newStackSection);
					if (errcode != 0) return errcode;
					errcode = envs->push((QueryBagResult *) newStackSection);
					if (errcode != 0) return errcode;
					QueryResult *newResult;
					errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
					if (errcode != 0) return errcode;
					errcode = qres->pop(newResult);
					if (errcode != 0) return errcode;
					*ec << "[QE] Computing right Argument with a new scope of ES";
					errcode = envs->pop();
					if (errcode != 0) return errcode;
					switch (op) {
						case NonAlgOpNode::closeBy: {
							QueryResult *nResult;
							if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
								nResult = new QueryBagResult();
								nResult->addResult(newResult);
							}
							else
								nResult = newResult;
							for (unsigned int i = 0; i < (nResult->size()); i++) {
								QueryResult *current_new_result;
								if ((nResult->type()) == QueryResult::QSEQUENCE)
									errcode = (((QuerySequenceResult *) nResult)->at(i, current_new_result));
								else
									errcode = (((QueryBagResult *) nResult)->at(i, current_new_result));
								if (errcode != 0) return errcode;
								partial_result->addResult(current_new_result); // this can cause an infinite loop !
								final_result->addResult(current_new_result);
								
							}
							break;
						}
						case NonAlgOpNode::closeUniqueBy: {
							QueryResult *nResult;
							if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
								nResult = new QueryBagResult();
								nResult->addResult(newResult);
							}
							else
								nResult = newResult;
							for (unsigned int i = 0; i < (nResult->size()); i++) {
								QueryResult *current_new_result;
								if ((nResult->type()) == QueryResult::QSEQUENCE)
									errcode = (((QuerySequenceResult *) nResult)->at(i, current_new_result));
								else
									errcode = (((QueryBagResult *) nResult)->at(i, current_new_result));
								if (errcode != 0) return errcode;
								bool isIncl;
								errcode = this->isIncluded(current_new_result, final_result, isIncl);
								if (errcode != 0) return errcode;
								if (not (isIncl)) { // occur check
									partial_result->addResult(current_new_result);
									final_result->addResult(current_new_result);
								}
							}
							break;
						}
						case NonAlgOpNode::leavesBy: {
							if (newResult->isNothing())
								final_result->addResult(currentResult);
							else {
								QueryResult *nResult;
								if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
									nResult = new QueryBagResult();
									nResult->addResult(newResult);
								}
								else
									nResult = newResult;
								for (unsigned int i = 0; i < (nResult->size()); i++) {
									QueryResult *c_result;
									if ((nResult->type()) == QueryResult::QSEQUENCE)
										errcode = (((QuerySequenceResult *) nResult)->at(i, c_result));
									else
										errcode = (((QueryBagResult *) nResult)->at(i, c_result));
									if (errcode != 0) return errcode;
									partial_result->addResult(c_result); // this can cause an infinite loop !
								}
							}
							break;
						}
						case NonAlgOpNode::leavesUniqueBy: {
							if (newResult->isNothing())
								final_result->addResult(currentResult);
							else {
								QueryResult *nResult;
								if (((newResult->type()) != QueryResult::QSEQUENCE) && ((newResult->type()) != QueryResult::QBAG)) {
									nResult = new QueryBagResult();
									nResult->addResult(newResult);
								}
								else
									nResult = newResult;
								for (unsigned int i = 0; i < (nResult->size()); i++) {
									QueryResult *c_result;
									if ((nResult->type()) == QueryResult::QSEQUENCE)
										errcode = (((QuerySequenceResult *) nResult)->at(i, c_result));
									else
										errcode = (((QueryBagResult *) nResult)->at(i, c_result));
									if (errcode != 0) return errcode;
									bool isIncl;
									errcode = this->isIncluded(c_result, visited_result, isIncl);
									if (errcode != 0) return errcode;
									if (not (isIncl)) { // occur check
										partial_result->addResult(c_result);
										visited_result->addResult(c_result);
									}
								}
							}
							break;
						}
						default: {
							break;
						}
					}//switch
				}//while
			}//if
			else { // this is normal (not recursive) non algebraic operation
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
						errcode = envs->push((QueryBagResult *) newStackSection);
						if (errcode != 0) return errcode;
						QueryResult *rResult;
						errcode = executeRecQuery (((NonAlgOpNode *) tree)->getRArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(rResult);
						if (errcode != 0) return errcode;
						*ec << "[QE] Computing right Argument with a new scope of ES";
						errcode = this->combine(op,currentResult,rResult,partial_result);
						if (errcode != 0) return errcode;
						*ec << "[QE] Combined partial results";
						errcode = envs->pop();
						if (errcode != 0) return errcode;
					}
				}
				else {
					*ec << "[QE] ERROR! NonAlgebraic operation, bad left argument";
					*ec << (ErrQExecutor | EOtherResExp);
					return ErrQExecutor | EOtherResExp;
				}
				errcode = this->merge(op,partial_result, final_result);
				if (errcode != 0) return errcode;
				*ec << "[QE] Merged partial results into final result";
			}
			errcode = qres->push(final_result);
			if (errcode != 0) return errcode;
			*ec << "[QE] NonAlgebraic operation Done!";
			return 0;
		} //case TNNONALGOP
		
		case TreeNode::TNCOND: {
			CondNode::condOp op = ((CondNode *) tree)->getOp();
			*ec << "[QE] Conditional operator - type recognized";
			switch (op) {
				case CondNode::iff: {
					*ec << "[QE] IF <q1> THEN <q2> FI";
					QueryResult *tmp_result;
					errcode = executeRecQuery(((CondNode *) tree)->getCondition());
					if (errcode != 0) return errcode;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					bool bool_val;
					if (tmp_result->isBool()) {
						errcode = tmp_result->getBoolValue(bool_val);
						if (errcode != 0) return errcode;
					}
					else {
						*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
						*ec << (ErrQExecutor | EBoolExpected);
						return ErrQExecutor | EBoolExpected;
					}
					if (bool_val) {
						errcode = executeRecQuery(((CondNode *) tree)->getLArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
					}
					break;
				}
				case CondNode::ifElsee: {
					*ec << "[QE] IF <q1> THEN <q2> ELSE <q3> FI";
					QueryResult *tmp_result;
					errcode = executeRecQuery(((CondNode *) tree)->getCondition());
					if (errcode != 0) return errcode;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					bool bool_val;
					if (tmp_result->isBool()) {
						errcode = tmp_result->getBoolValue(bool_val);
						if (errcode != 0) return errcode;
					}
					else {
						*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
						*ec << (ErrQExecutor | EBoolExpected);
						return ErrQExecutor | EBoolExpected;
					}
					if (bool_val) {
						errcode = executeRecQuery(((CondNode *) tree)->getLArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
					}
					else {
						errcode = executeRecQuery(((CondNode *) tree)->getRArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
					}
					break;
				}
				case CondNode::whilee: {
					*ec << "[QE] WHILE <q1> DO <q2> OD";
					QueryResult *tmp_result;
					errcode = executeRecQuery(((CondNode *) tree)->getCondition());
					if (errcode != 0) return errcode;
					errcode = qres->pop(tmp_result);
					if (errcode != 0) return errcode;
					bool bool_val;
					if (tmp_result->isBool()) {
						errcode = tmp_result->getBoolValue(bool_val);
						if (errcode != 0) return errcode;
					}
					else {
						*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
						*ec << (ErrQExecutor | EBoolExpected);
						return ErrQExecutor | EBoolExpected;
					}
					while (bool_val) {
						errcode = executeRecQuery(((CondNode *) tree)->getLArg());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
						errcode = executeRecQuery(((CondNode *) tree)->getCondition());
						if (errcode != 0) return errcode;
						errcode = qres->pop(tmp_result);
						if (errcode != 0) return errcode;
						if (tmp_result->isBool()) {
							errcode = tmp_result->getBoolValue(bool_val);
							if (errcode != 0) return errcode;
						}
						else {
							*ec << "[QE] ERROR! <q1> must be BOOLEAN ";
							*ec << (ErrQExecutor | EBoolExpected);
							return ErrQExecutor | EBoolExpected;
						}
					}
					break;
				}
				default: {
					*ec << "[QE] ERROR! Condition operation type not known";
					*ec << (ErrQExecutor | EUnknownNode);
					return ErrQExecutor | EUnknownNode;
				}
			}
			QueryResult *result = new QueryBagResult();
			errcode = qres->push(result);
			if (errcode != 0) return errcode;
			*ec << "[QE] Condition operation Done!";
			return 0;
		} //case TNCOND 

		default:
			{
			*ec << "Unknown node type";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
			}

		} // end of switch
		
	} // if tree!=Null
	else { // tree == NULL; return empty result
		*ec << "[QE] empty tree, nothing to do";
		QueryResult *result = new QueryNothingResult();
		errcode = qres->push(result);
		if (errcode != 0) return errcode;
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
			if (((QueryReferenceResult *) arg)->wasRefed()) {
				res = arg;
				break;
			}
			LogicalID *ref_value = ((QueryReferenceResult *) arg)->getValue();
			*ec << "[QE] derefQuery() - dereferencing Object";
			ObjectPointer *optr;
			errcode = tr->getObjectPointer(ref_value, Store::Read, optr);
			if (errcode != 0) {
			
				*ec << "[QE] Error in getObjectPointer";
				tr->abort();
				tr = NULL;
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
				case Store::Pointer: {
					*ec << "[QE] derefQuery() - ObjectValue = Pointer";
					LogicalID *tmp_value = value->getPointer();
					res = new QueryReferenceResult(tmp_value);
					break;
				}
				case Store::Vector: {
					*ec << "[QE] derefQuery() - ObjectValue = Vector";
					vector<LogicalID*>* tmp_vec = value->getVector();
					res = new QueryStructResult();
					int tmp_vec_size = tmp_vec->size();
					for (int i = 0; i < tmp_vec_size; i++) {
						LogicalID *currID = tmp_vec->at(i);
						ObjectPointer *currOptr;
						errcode = tr->getObjectPointer(currID, Store::Read, currOptr);
						if (errcode != 0) {
							*ec << "[QE] Error in getObjectPointer";
							tr->abort();
							tr = NULL;
							return errcode;
						}
						string currName = currOptr->getName();
						QueryResult *currIDRes = new QueryReferenceResult(currID);
						QueryResult *currInside;
						this->derefQuery(currIDRes, currInside);
						QueryResult *currBinder = new QueryBinderResult(currName, currInside);
						res->addResult(currBinder);
					}
					break;
				}
				default: {
					*ec << "[QE] derefQuery() - wrong argument type";
					*ec << (ErrQExecutor | EUnknownValue);
					return ErrQExecutor | EUnknownValue;
					break;
				}
			}
			break;
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		default: {
			*ec << "[QE] ERROR in derefQuery() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
	}
	QueryResult *tmp_res = res;
	int tmp_resType = tmp_res->type();
	if (tmp_resType == QueryResult::QBAG) {
		if (tmp_res->size() == 1) {
			errcode = ((QueryBagResult *) tmp_res)->at(0, res);
			if (errcode != 0) return errcode;
		}
	}
	else if (tmp_resType == QueryResult::QSEQUENCE) {
		if (tmp_res->size() == 1) {
			errcode = ((QuerySequenceResult *) tmp_res)->at(0, res);
			if (errcode != 0) return errcode;
		}
	}
	return 0;
}

int QueryExecutor::refQuery(QueryResult *arg, QueryResult *&res) {
	*ec << "[QE] refQuery()";
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
				errcode = this->refQuery(tmp_item, tmp_res);
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
				errcode = this->refQuery(tmp_item, tmp_res);
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
				errcode = this->refQuery(tmp_item, tmp_res);
				if (errcode != 0) return errcode;
				res->addResult(tmp_res);
			}
			break;
		}
		case QueryResult::QBINDER: {
			string tmp_name = ((QueryBinderResult *) arg)->getName();
			QueryResult *tmp_item;
			errcode = this->refQuery(((QueryBinderResult *) arg)->getItem(), tmp_item);
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
			res = arg;
			((QueryReferenceResult *) res)->setRef();
			break;
		}
		case QueryResult::QNOTHING: {
			res = arg;
			break;
		}
		default: {
			*ec << "[QE] ERROR in refQuery() - unknown result type";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
	}
	return 0;
}

int QueryExecutor::unOperate(UnOpNode::unOp op, QueryResult *arg, QueryResult *&final) {
	int errcode;
	switch (op) {
		case UnOpNode::deref: {
			*ec << "[QE] DEREF operation";
			errcode = this->derefQuery(arg, final);
			if (errcode != 0) return errcode;
			break;
		}
		case UnOpNode::ref: {
			*ec << "[QE] REF operation";
			errcode = this->refQuery(arg, final);
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
				*ec << (ErrQExecutor | ENumberExpected);
				return ErrQExecutor | ENumberExpected;
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
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
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
			if ((bagArg->size()) == 0) 
				break;
			QueryBagResult *argSorted;
			errcode = this->sortBag((QueryBagResult *) bagArg, argSorted);
			if (errcode != 0) return errcode;
			QueryResult *current;
			errcode = argSorted->getResult(current);
			if (errcode != 0) return errcode;
			final->addResult(current);
			while ((argSorted->size()) > 0) { 
				QueryResult *next;
				errcode = argSorted->getResult(next);
				if (errcode != 0) return errcode;
				if (next->not_equal(current)) {
					current = next;
					final->addResult(current);
				}
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
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}
				LogicalID *lid = ((QueryReferenceResult *) toDelete)->getValue();
				ObjectPointer *optr;
				if ((errcode = tr->getObjectPointer (lid, Store::Write, optr)) !=0) {
					*ec << "[QE] Error in getObjectPointer.";
					tr->abort();
					tr = NULL;
					return errcode;
				}
				if (optr->isRoot()) {
					if ((errcode = tr->removeRoot(optr)) != 0) {
						*ec << "[QE] Error in removeRoot.";
						tr->abort();
						tr = NULL;
						return errcode;
					}
					*ec << "[QE] Root removed";
				}
				if ((errcode = tr->deleteObject(optr)) != 0) {
					*ec << "[QE] Error in deleteObject.";
					tr->abort();
					tr = NULL;
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
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
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
					*ec << "[QE] ERROR! + arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
				}
				case AlgOpNode::minus: {
					*ec << "[QE] ERROR! - arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
				}
				case AlgOpNode::times: {
					*ec << "[QE] ERROR! * arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
				}
				case AlgOpNode::divide: {
					*ec << "[QE] ERROR! / arguments must be INT or DOUBLE";
					*ec << (ErrQExecutor | ENumberExpected);
					return ErrQExecutor | ENumberExpected;
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
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
				}
				case AlgOpNode::boolAnd: {
					*ec << "[QE] ERROR! AND arguments must be BOOLEAN";
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
				}
				default : { break; }
			}
		}
		return 0;
	} 
	else if ((op == AlgOpNode::bagUnion) || (op == AlgOpNode::bagIntersect) || (op == AlgOpNode::bagMinus)) {
		QueryResult *leftBag = new QueryBagResult();
		QueryResult *rightBag = new QueryBagResult();
		final = new QueryBagResult();
		leftBag->addResult(lArg);
		rightBag->addResult(rArg);
		if (op == (AlgOpNode::bagUnion)) {
			*ec << "[QE] BAG_UNION operation";
			((QueryBagResult *) final)->addResult(leftBag);
			((QueryBagResult *) final)->addResult(rightBag);
			return 0;
		} else {
			if (op == (AlgOpNode::bagIntersect)) 
				*ec << "[QE] BAG_INTERSECT operation";
			if (op == (AlgOpNode::bagMinus)) 
				*ec << "[QE] BAG_MINUS operation";
			final = new QueryBagResult();
			QueryBagResult *firstSorted;
			errcode = this->sortBag((QueryBagResult *) leftBag, firstSorted);
			if (errcode != 0) return errcode;
			QueryBagResult *secondSorted;
			errcode = this->sortBag((QueryBagResult *) rightBag, secondSorted);
			if (errcode != 0) return errcode;
			while ((firstSorted->size()) != 0) {
				QueryResult *first_elem;
				QueryResult *second_elem;
				if ((secondSorted->size()) != 0) {
					errcode = firstSorted->at(0,first_elem);
					if (errcode != 0) return errcode;
					errcode = secondSorted->at(0,second_elem);
					if (errcode != 0) return errcode;
					if (first_elem->equal(second_elem)) {
						if (op == (AlgOpNode::bagIntersect))
							((QueryBagResult *) final)->addResult(first_elem);
						errcode = firstSorted->getResult(first_elem);
						if (errcode != 0) return errcode;
						errcode = secondSorted->getResult(second_elem);
						if (errcode != 0) return errcode;
					}
					else if (first_elem->sorting_less_eq(second_elem)) {
						if (op == (AlgOpNode::bagMinus))
							((QueryBagResult *) final)->addResult(first_elem);
						errcode = firstSorted->getResult(first_elem);
						if (errcode != 0) return errcode;
					}
					else {
						errcode = secondSorted->getResult(second_elem);
						if (errcode != 0) return errcode;
					}
				}
				else {
					errcode = firstSorted->at(0,first_elem);
					if (errcode != 0) return errcode;
					if (op == (AlgOpNode::bagMinus))
						((QueryBagResult *) final)->addResult(first_elem);
					errcode = firstSorted->getResult(first_elem);
					if (errcode != 0) return errcode;
				}
			}
			return 0;
		}
	}
	else if (op == AlgOpNode::comma) {
		*ec << "[QE] COMMA operation";
		final = new QueryBagResult();
		errcode = lArg->comma(rArg,final);
		if (errcode != 0) return errcode;
		return 0;
	}
	else if (op == AlgOpNode::semicolon) {
		*ec << "[QE] SEMICOLON operation";
		final = rArg;
		return 0;
	}
	else if (op == AlgOpNode::insert) {
		*ec << "[QE] INSERT operation";
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			*ec << "[QE] ERROR! Left argument of insert operation must have size 1";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		QueryResult* outer;
		errcode = ((QueryBagResult *) lBag)->at(0, outer);
		if (errcode != 0) return errcode;
		int leftResultType = outer->type();
		if (leftResultType != QueryResult::QREFERENCE) {
			*ec << "[QE] ERROR! Left argument of insert operation must consist of QREFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		LogicalID *lidOut = ((QueryReferenceResult *) outer)->getValue();
		ObjectPointer *optrOut;
		errcode = tr->getObjectPointer (lidOut, Store::Write, optrOut);
		if (errcode != 0) {
			*ec << "[QE] insert operation - Error in getObjectPointer.";
			tr->abort();
			tr = NULL;
			return errcode;
		}
		*ec << "[QE] insert operation - getObjectPointer on leftArg done";
		DataValue* db_value = optrOut->getValue();
		*ec << "[QE] insert operation - Value taken";
		int vType = db_value->getType();
		if (vType != Store::Vector) {
			*ec << "[QE] insert operation - ERROR! the Value has to be a Vector";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
		vector<LogicalID*> *insVector = db_value->getVector();
		if (insVector == NULL) {
			*ec << "[QE] insert operation - insVector == NULL";
			*ec << (ErrQExecutor | EQEUnexpectedErr);
			return ErrQExecutor | EQEUnexpectedErr;
		}
		*ec << "[QE] Vector taken";
		ec->printf("[QE] Vec.size = %d\n", insVector->size());
		QueryResult *rBag = new QueryBagResult();
		rBag->addResult(rArg);
		for (unsigned int i = 0; i < rBag->size(); i++) {
			ec->printf("[QE] trying to INSERT %d element of rightArg into leftArg\n", i);
			QueryResult *toInsert;
			errcode = ((QueryBagResult *) rBag)->at(i, toInsert);
			if (errcode != 0) return errcode;
			ObjectPointer *optrIn;
			LogicalID *lidIn;
			int rightResultType = toInsert->type();
			switch (rightResultType) {
				case QueryResult::QREFERENCE: {
					lidIn = ((QueryReferenceResult *) toInsert)->getValue();
					errcode = tr->getObjectPointer (lidIn, Store::Write, optrIn);
					if (errcode != 0) {
						*ec << "[QE] insert operation - Error in getObjectPointer.";
						tr->abort();
						tr = NULL;
						return errcode;
					}
					ec->printf("[QE] getObjectPointer on %d element of rightArg done\n", i);
					if (optrIn->isRoot()) {
						errcode = tr->removeRoot(optrIn);
						if (errcode != 0) {
							*ec << "[QE] insert operation - Error in removeRoot.";
							tr->abort();
							tr = NULL;
							return errcode;
						}
						*ec << "[QE] insert operation - Root removed";
					}
					else {
						*ec << "[QE] ERROR in insert operation - right argument must be Root";
						*ec << (ErrQExecutor | EQEUnexpectedErr);
						return ErrQExecutor | EQEUnexpectedErr;
					}
					break;
				}
				case QueryResult::QBINDER: {
					errcode = this->objectFromBinder(toInsert, optrIn);
					if (errcode != 0) return errcode;
					lidIn = (optrIn->getLogicalID());
					break;
				}
				default: {
					*ec << "[QE] Right argument of insert operation must consist of QREFERENCE or QBINDER";
					*ec << (ErrQExecutor | ERefExpected);
					return ErrQExecutor | ERefExpected;
				}
			}
			*ec << "[QE] Inserting the object";
			insVector->push_back(lidIn);
			ec->printf("[QE] New Vec.size = %d\n", insVector->size());
		}
		DBDataValue *dbDataVal = new DBDataValue();
		dbDataVal->setVector(insVector);
		*ec << "[QE] dataValue: setVector done";
		db_value = dbDataVal;
		errcode = tr->modifyObject(optrOut, db_value);
		if (errcode != 0) {
			*ec << "[QE] insert operation - Error in modifyObject.";
			tr->abort();
			tr = NULL;
			return errcode;
		}
		*ec << "[QE] INSERT operation Done";
		final = new QueryNothingResult();
		return 0;
	}
	else if (op == AlgOpNode::assign) {
		*ec << "[QE] ASSIGN operation";
		QueryResult *lBag = new QueryBagResult();
		lBag->addResult(lArg);
		if (lBag->size() != 1) {
			*ec << "[QE] ERROR! operator <assign> expects that left argument is single REFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		errcode = ((QueryBagResult *) lBag)->at(0, lArg);
		if (errcode != 0) return errcode;
		int leftResultType = lArg->type();
		if (leftResultType != QueryResult::QREFERENCE) {
			*ec << "[QE] ERROR! operator <assign> expects that left argument is REFERENCE";
			*ec << (ErrQExecutor | ERefExpected);
			return ErrQExecutor | ERefExpected;
		}
		LogicalID *old_lid = ((QueryReferenceResult *) lArg)->getValue();
		ObjectPointer *old_optr;
		errcode = tr->getObjectPointer (old_lid, Store::Write, old_optr);
		if (errcode != 0) {
			*ec << "[QE] operator <assign>: error in getObjectPointer()";
			tr->abort();
			tr = NULL;
			return errcode;
		}
		*ec << "[QE] operator <assign>: getObjectPointer on left argument done";
		DBDataValue *dbValue = new DBDataValue();
		QueryResult *derefed;
		errcode = this->derefQuery(rArg, derefed);
		if (errcode != 0) return errcode;
		int derefed_type = derefed->type();
		switch (derefed_type) {
			case QueryResult::QINT: {
				int intValue = ((QueryIntResult *) derefed)->getValue();
				dbValue->setInt(intValue);
				*ec << "[QE] < query := ('integer'); > operation";
				break;
			}
			case QueryResult::QDOUBLE: {
				double doubleValue = ((QueryDoubleResult *) derefed)->getValue();
				dbValue->setDouble(doubleValue);
				*ec << "[QE] < query := ('double'); > operation";
				break;
			}
			case QueryResult::QSTRING: {
				string stringValue = ((QueryStringResult *) derefed)->getValue();
				dbValue->setString(stringValue);
				*ec << "[QE] < query := ('string'); > operation";
				break;
			}
			case QueryResult::QREFERENCE: {
				LogicalID* refValue = ((QueryReferenceResult *) derefed)->getValue();
				dbValue->setPointer(refValue);
				*ec << "[QE] < query := ('reference'); > operation";
				break;
			}
			case QueryResult::QSTRUCT: {
				vector<LogicalID*> vectorValue;
				for (unsigned int i=0; i < (derefed->size()); i++) {
					QueryResult *tmp_res;
					errcode = ((QueryStructResult *) derefed)->at(i, tmp_res);
					if (errcode != 0) return errcode;
					ObjectPointer *optr_tmp;
					errcode = this->objectFromBinder(tmp_res, optr_tmp);
					if (errcode != 0) return errcode;
					LogicalID *lid_tmp = optr_tmp->getLogicalID();
					vectorValue.push_back(lid_tmp);
				}
				dbValue->setVector(&vectorValue);
				*ec << "[QE] < query := ('struct'); > operation";
				break;
			}
			case QueryResult::QBINDER: {
				vector<LogicalID*> vectorValue;
				ObjectPointer *optr_tmp;
				errcode = this->objectFromBinder(derefed, optr_tmp);
				if (errcode != 0) return errcode;
				LogicalID *lid_tmp = optr_tmp->getLogicalID();
				vectorValue.push_back(lid_tmp);
				dbValue->setVector(&vectorValue);
				*ec << "[QE] < query := ('binder'); > operation";
				break;
			}
			default: {
				*ec << "[QE] operator <assign>: error, bad type right argument evaluated by executeRecQuery";
				*ec << (ErrQExecutor | EOtherResExp);
				return ErrQExecutor | EOtherResExp;
			}
		}
		DataValue* value = dbValue;
		errcode = tr->modifyObject(old_optr, value);
		if (errcode != 0) {
			*ec << "[QE] operator <assign>: error in modifyObject()";
			tr->abort();
			tr = NULL;
			return errcode;
		}
		*ec << "[QE] operator <assign>: value of the object was changed";
		*ec << "[QE] ASSIGN operation Done";
		final = new QueryNothingResult();
		return 0;
	}
	else { // algOperation type not known
		*ec << "[QE] ERROR! Algebraic operation type not known";
		*ec << (ErrQExecutor | EUnknownNode);
		return ErrQExecutor | EUnknownNode;
	}
	return 0;
}

int QueryExecutor::combine(NonAlgOpNode::nonAlgOp op, QueryResult *curr, QueryResult *rRes, QueryResult *&partial) {
	*ec << "[QE] combine() function applied to the partial results";
	int errcode;
	switch (op) {
		case NonAlgOpNode::dot: {
			*ec << "[QE] combine(): NonAlgebraic operator <dot>";
			partial->addResult(rRes);
			break;
		}
		case NonAlgOpNode::join: {
			*ec << "[QE] combine(): NonAlgebraic operator <join>";
			errcode = curr->comma(rRes,partial);
			if (errcode != 0) return errcode;
			break;
		}
		case NonAlgOpNode::where: {
			*ec << "[QE] combine(): NonAlgebraic operator <where>";
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				if (tmp_bool_value) 
					partial->addResult(curr);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <where> is not boolean!";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case NonAlgOpNode::orderBy: {
			*ec << "[QE] combine(): NonAlgebraic operator <orderBy>";
			QueryResult *derefR;
			errcode = this->derefQuery(rRes,derefR);
			if (errcode != 0) return errcode;
			QueryResult *currRow = new QueryStructResult(curr, derefR);
			partial->addResult(currRow);
			break;
		}
		case NonAlgOpNode::exists: {
			*ec << "[QE] combine(): NonAlgebraic operator <exists>";
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <exists> is not boolean!";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case NonAlgOpNode::forAll: {
			*ec << "[QE] combine(): NonAlgebraic operator <forAll>";
			if (rRes->isBool()) {
				bool tmp_bool_value;
				errcode = (rRes->getBoolValue(tmp_bool_value));
				if (errcode != 0) return errcode;
				QueryResult *tmp_result = new QueryBoolResult(tmp_bool_value); 
				partial->addResult(tmp_result);
			}
			else {
				*ec << "[QE] combine(): ERROR! Right argument of operator <forAll> is not boolean!";
				*ec << (ErrQExecutor | EBoolExpected);
				return ErrQExecutor | EBoolExpected;
			}
			break;
		}
		case NonAlgOpNode::forEach: {
			*ec << "[QE] combine(): NonAlgebraic operator <forEach>";
			break;
		}
		default: {
			*ec << "[QE] combine(): unknown NonAlgebraic operator!";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
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
		case NonAlgOpNode::orderBy: {
			*ec << "[QE] merge(): NonAlgebraic operator <orderBy>";
			QueryResult *tmp_seq = new QuerySequenceResult();
			errcode = ((QuerySequenceResult *) tmp_seq)->sortCollection(partial);
			if (errcode != 0) return errcode;
			errcode = ((QuerySequenceResult *) tmp_seq)->postSort(final);
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
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
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
					*ec << (ErrQExecutor | EBoolExpected);
					return ErrQExecutor | EBoolExpected;
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
			*ec << "[QE] merge(): NonAlgebraic operator <forEach>";
			final = partial;
			break;
		}
		default: {
			*ec << "[QE] merge(): unknown NonAlgebraic operator!";
			*ec << (ErrQExecutor | EUnknownNode);
			return ErrQExecutor | EUnknownNode;
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
		*ec << (ErrQExecutor | EOtherResExp);
		return ErrQExecutor | EOtherResExp;
	}
	bool tmp_bool = false;
	for (unsigned int i = 0; i < set->size(); i++) {
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

int QueryExecutor::sortBag(QueryBagResult *inBag, QueryBagResult *&outBag) {
	int errcode;
	outBag = new QueryBagResult();
	if ((inBag->size()) == 1) { 
		outBag->addResult(inBag);
	}
	else if ((inBag->size()) == 2) {
		QueryResult *first;
		QueryResult *second;
		errcode = inBag->at(0, first);
		if (errcode != 0) return errcode;
		errcode = inBag->at(1, second);
		if (errcode != 0) return errcode;
		if (first->sorting_less_eq(second)) {
			outBag->addResult(first);
			outBag->addResult(second);
		}
		else {
			outBag->addResult(second);
			outBag->addResult(first);
		}
	}
	else { //inBag size = 2+
		QueryResult *firstHalf;
		QueryResult *secondHalf;
		errcode = inBag->divideBag(firstHalf, secondHalf);
		if (errcode != 0) return errcode;
		QueryBagResult *firstHalfSorted;
		QueryBagResult *secondHalfSorted;
		errcode = this->sortBag((QueryBagResult *)firstHalf, firstHalfSorted);
		if (errcode != 0) return errcode;
		errcode = this->sortBag((QueryBagResult *)secondHalf, secondHalfSorted);
		if (errcode != 0) return errcode;
		unsigned int total_size = (firstHalfSorted->size()) + (secondHalfSorted->size());
		if (total_size != (inBag->size())) {
			*ec << "[QE] sortCollection() ERROR! - i lost some elements somewhere...";
			*ec << (ErrQExecutor | EQEUnexpectedErr);
			return ErrQExecutor | EQEUnexpectedErr; // something is wrong, those sizes should be equal
		}
		for (unsigned int i = 0; i < total_size; i++) {
			QueryResult *first_elem;
			QueryResult *second_elem;
			if ((firstHalfSorted->size()) == 0) { // first half ended
				errcode = secondHalfSorted->getResult(second_elem);
				if (errcode != 0) return errcode;
				outBag->addResult(second_elem);
			} 
			else if ((secondHalfSorted->size()) == 0) { // second sequence ended
				errcode = firstHalfSorted->getResult(first_elem);
				if (errcode != 0) return errcode;
				outBag->addResult(first_elem);
			} 
			else {
				errcode = firstHalfSorted->at(0,first_elem);
				if (errcode != 0) return errcode;
				errcode = secondHalfSorted->at(0,second_elem);
				if (errcode != 0) return errcode;
				if (first_elem->sorting_less_eq(second_elem)) {
					outBag->addResult(first_elem);
					errcode = firstHalfSorted->getResult(first_elem);
					if (errcode != 0) return errcode;
				}
				else {
					outBag->addResult(second_elem);
					errcode = secondHalfSorted->getResult(second_elem);
					if (errcode != 0) return errcode;
				}
			}
		}
	}
	return 0;
}

int QueryExecutor::abort() {
	*ec << "[QE] abort()";
	if (tr != NULL) {
		int errcode = tr->abort();
		tr = NULL;
		if (errcode != 0) {
			*ec << "[QE] Error in abort()";
			return errcode;
		}
		*ec << "[QE] Transaction aborted succesfully. Done!";
	}
	else
		*ec << "[QE] Transaction not opened. Closing nevertheless. Done!\n";
	return 0;
}

int QueryExecutor::objectFromBinder(QueryResult *res, ObjectPointer *&newObject) {
	int errcode;
	*ec << "[QE] objectFromBinder()";
	if ((res->type()) != QueryResult::QBINDER) {
		*ec << "[QE] objectFromBinder() expected a binder, got something else";
		*ec << (ErrQExecutor | EOtherResExp);
		return ErrQExecutor | EOtherResExp;
	}
	string binderName = ((QueryBinderResult *) res)->getName();
	QueryResult *binderItem = ((QueryBinderResult *) res)->getItem();
	DBDataValue *dbValue = new DBDataValue();
	int binderItemType = binderItem->type();
	switch (binderItemType) {
		case QueryResult::QINT: {
			int intValue = ((QueryIntResult *) binderItem)->getValue();
			dbValue->setInt(intValue);
			*ec << "[QE] objectFromBinder(): value is INT type";
			break;
		}
		case QueryResult::QDOUBLE: {
			double doubleValue = ((QueryDoubleResult *) binderItem)->getValue();
			dbValue->setDouble(doubleValue);
			*ec << "[QE] objectFromBinder(): value is DOUBLE type";
			break;
		}
		case QueryResult::QSTRING: {
			string stringValue = ((QueryStringResult *) binderItem)->getValue();
			dbValue->setString(stringValue);
			*ec << "[QE] objectFromBinder(): value is STRING type";
			break;
		}
		case QueryResult::QREFERENCE: {
			LogicalID* refValue = ((QueryReferenceResult *) binderItem)->getValue();
			dbValue->setPointer(refValue);
			*ec << "[QE] objectFromBinder(): value is REFERENCE type";
			break;
		}
		case QueryResult::QBINDER: {
			vector<LogicalID*> vectorValue;
			ObjectPointer *optr;
			errcode = this->objectFromBinder(binderItem, optr);
			if (errcode != 0) return errcode;
			LogicalID *lid = optr->getLogicalID();
			*ec << "[QE] objectFromBinder(): logical ID received";
			vectorValue.push_back(lid);
			*ec << "[QE] objectFromBinder(): added to a vector";
			dbValue->setVector(&vectorValue);
			*ec << "[QE] objectFromBinder(): value is BINDER type";
			break;
		}
		case QueryResult::QSTRUCT: {
			vector<LogicalID*> vectorValue;
			for (unsigned int i=0; i < (binderItem->size()); i++) {
				QueryResult *curr;
				errcode = ((QueryStructResult *) binderItem)->at(i, curr);
				if (errcode != 0) return errcode;
				ObjectPointer *optr;
				errcode = this->objectFromBinder(curr, optr);
				if (errcode != 0) return errcode;
				LogicalID *lid = optr->getLogicalID();
				*ec << "[QE] objectFromBinder(): logical ID received";
				vectorValue.push_back(lid);
				*ec << "[QE] objectFromBinder(): added to a vector";
			}
			dbValue->setVector(&vectorValue);
			*ec << "[QE] objectFromBinder(): value is STRUCT type";
			break;
		}
		default: {
			*ec << "[QE] objectFromBinder(): bad type item in a binder";
			*ec << (ErrQExecutor | EOtherResExp);
			return ErrQExecutor | EOtherResExp;
		}
	}
	DataValue* value;
	value = dbValue;
	if ((errcode = tr->createObject(binderName, value, newObject)) != 0) {
		*ec << "[QE] Error in createObject";
		tr->abort();
		tr = NULL;
		return errcode;
	}
	return 0;
}


QueryExecutor::~QueryExecutor() {
	if (tr != NULL) tr->abort();
	tr = NULL;
	delete envs;
	delete qres;
	*ec << "[QE] QueryExecutor shutting down\n";
	delete ec;
}



/* RESULT STACK */


ResultStack::ResultStack() { ec = new ErrorConsole("QueryExecutor"); }
ResultStack::~ResultStack() { this->deleteAll(); if (ec != NULL) delete ec; }

int ResultStack::push(QueryResult *r) {
	rs.push_back(r);
	*ec << "[QE] Result Stack pushed";
	return 0;
}

int ResultStack::pop(QueryResult *&r){ 
	if (rs.empty()) { 
		*ec << (ErrQExecutor | EQEmptySet);
		return ErrQExecutor | EQEmptySet;
	} 
	else {
		r=(rs.back());
		rs.pop_back();
	};
	*ec << "[QE] Result Stack popped"; 
	return 0;
}

bool ResultStack::empty() { return rs.empty(); }
int ResultStack::size() { return rs.size(); }

void ResultStack::deleteAll() {
	for (unsigned int i = 0; i < (rs.size()); i++) {
		delete rs.at(i);
	};
}

}
