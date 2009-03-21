#include "ExecutorViews.h"
#include "QueryResult.h"
#include "QueryExecutor.h"
#include "BindNames.h"
#include "Store/DBLogicalID.h"
#include "Errors/ErrorConsole.h"
#include "TransactionManager/Transaction.h"

namespace QExecutor
{
	ExecutorViews::ExecutorViews()
	{
		ec = &ErrorConsole::get_instance(EC_QUERY_EXECUTOR);
	}

	int ExecutorViews::createNewSections(QueryVirtualResult *qvirt, QueryBinderResult *param, LogicalID *viewdef, vector<QueryBagResult*> &sections, Transaction *tr, QueryExecutor *qe) 
	{
		int errcode;
		vector<DataValue*> _tmp_dataVal_vec;

		if ((qvirt != NULL) && (qvirt->view_parent != NULL)) {
			QueryResult *tmp_result_1 = new QueryReferenceResult(qvirt->view_parent);
			QueryResult *nested_tmp_result_1;
			errcode = tmp_result_1->nested(qe, tr, nested_tmp_result_1, _tmp_dataVal_vec);
			if(errcode != 0) return errcode;
			sections.push_back((QueryBagResult *) nested_tmp_result_1);
		}

		if (qvirt != NULL) {
			for (int i = ((qvirt->view_defs.size()) - 1); i >= 0; i-- ) {
				QueryResult *tmp_result_2 = new QueryReferenceResult(qvirt->view_defs.at(i));
				QueryResult *nested_tmp_result_2;
				errcode = tmp_result_2->nested(qe, tr, nested_tmp_result_2, _tmp_dataVal_vec);
				if(errcode != 0) return errcode;
				sections.push_back((QueryBagResult *) nested_tmp_result_2);
			}
		}

		if (viewdef != NULL) {
			QueryResult *tmp_result_3 = new QueryReferenceResult(viewdef);
			QueryResult *nested_tmp_result_3;
			errcode = tmp_result_3->nested(qe, tr, nested_tmp_result_3, _tmp_dataVal_vec);
			if(errcode != 0) return errcode;
			sections.push_back((QueryBagResult *) nested_tmp_result_3);
		}

		if (qvirt != NULL) {
			for (int i = ((qvirt->seeds.size()) - 1); i >= 0; i-- ) {
				QueryResult *tmp_result_4 = qvirt->seeds.at(i);
				QueryResult *nested_tmp_result_4;
				errcode = tmp_result_4->nested(qe, tr, nested_tmp_result_4, _tmp_dataVal_vec);
				if(errcode != 0) return errcode;
				sections.push_back((QueryBagResult *) nested_tmp_result_4);
			}
		}

		if (param != NULL) {
			QueryBagResult *param_bag = new QueryBagResult();
			param_bag->addResult(param);
			sections.push_back(param_bag);
		}

		return 0;
	}

	int ExecutorViews::checkViewAndGetVirtuals(LogicalID *lid, string &name, string &code, Transaction *tr) {
		int errcode;
		string actual_name = "";
		int actual_name_count = 0;
		ObjectPointer *optr;
		errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
		if (errcode != 0) {
			debug_print(*ec,  "[QE] checkViewAndGetVirtuals - Error in getObjectPointer.");
			return errcode;
		}
		DataValue* data_value = optr->getValue();
		int vType = data_value->getType();
		ExtendedType extType = data_value->getSubtype();
		if ((vType == Store::Vector) && (extType == Store::View)) {
			vector<LogicalID*>* tmp_vec = (data_value->getVector());
			int vec_size = tmp_vec->size();
			for (int i = 0; i < vec_size; i++ ) {
				LogicalID *tmp_logID = tmp_vec->at(i);
				ObjectPointer *tmp_optr;
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
					debug_print(*ec,  "[QE] checkViewAndGetVirtuals - Error in getObjectPointer");
					return errcode;
				}
				string tmp_name = tmp_optr->getName();
				DataValue* tmp_data_value = tmp_optr->getValue();
				int tmp_vType = tmp_data_value->getType();
				ExtendedType tmp_extType = tmp_data_value->getSubtype();
				if ((tmp_name == QE_VIEWDEF_VIRTUALOBJECTS_NAME) && (tmp_vType == Store::String) && (tmp_extType == Store::None)) {
					actual_name = tmp_data_value->getString();
					actual_name_count++;
				}
			}
			if (actual_name_count == 0) {
				debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: \"VirtualObjects\" not found in this view ");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			else if (actual_name_count > 1) {
				debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: multiple \"VirtualObjects\" found in this view");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			else if ((name != "") && (name != actual_name)) {
				debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: \"VirtualObjects\" have wrong value");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			else name = actual_name;
			int proc_code_founded = 0;
			string proc_code = "";
			for (int i = 0; i < vec_size; i++) {
				LogicalID *tmp_logID = tmp_vec->at(i);
				ObjectPointer *tmp_optr;
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
					debug_print(*ec,  "[QE] checkViewAndGetVirtuals - Error in getObjectPointer");
					return errcode;
				}
				string tmp_name = tmp_optr->getName();
				DataValue* tmp_data_value = tmp_optr->getValue();
				int tmp_vType = tmp_data_value->getType();
				ExtendedType tmp_extType = tmp_data_value->getSubtype();
				if ((tmp_vType == Store::Vector) && (tmp_extType == Store::Procedure)) {
					if (tmp_name == name) {
						vector<LogicalID*>* tmp_vec_inner = (tmp_data_value->getVector());
						int vec_size_inner = tmp_vec_inner->size();
						if (vec_size_inner != 1) {
							debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: \"virtual objects\" procedure wrong format");
							debug_print(*ec,  (ErrQExecutor | EBadViewDef));
							return (ErrQExecutor | EBadViewDef);
						}
						LogicalID *proc_code_lid = tmp_vec_inner->at(0);
						ObjectPointer *proc_code_optr;
						if ((errcode = tr->getObjectPointer(proc_code_lid, Store::Read, proc_code_optr, false)) != 0) {
							debug_print(*ec,  "[QE] checkViewAndGetVirtuals - Error in getObjectPointer");
							return errcode;
						}
						string proc_code_name = proc_code_optr->getName();
						DataValue* proc_code_data_value = proc_code_optr->getValue();
						int proc_code_vType = proc_code_data_value->getType();
						ExtendedType proc_code_extType = proc_code_data_value->getSubtype();
						if ((proc_code_vType == Store::String) && (proc_code_extType == Store::None) && (proc_code_name == QE_METHOD_PROCBODY_BIND_NAME)) {
							proc_code = proc_code_data_value->getString();
						}
						else {
							debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: \"virtual objects\" procedure wrong format");
							debug_print(*ec,  (ErrQExecutor | EBadViewDef));
							return (ErrQExecutor | EBadViewDef);
						}
						proc_code_founded++;
					}
				}
			}
			if (proc_code_founded == 0) {
				debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: \"virtual objects\" procedure not found");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			else if (proc_code_founded > 1) {
				debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: multiple \"virtual objects\" procedures found");
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
			else code = proc_code;
		}
		else {
			debug_print(*ec,  "[QE] checkViewAndGetVirtuals error: This is not a view");
			debug_print(*ec,  (ErrQExecutor | EBadViewDef));
			return (ErrQExecutor | EBadViewDef);
		}
		return 0;
	}

	int ExecutorViews::getSubviews(LogicalID *lid, string vo_name, vector<LogicalID *> &subviews, Transaction *tr) {
		int errcode;
		ObjectPointer *optr;
		errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
		if (errcode != 0) {
			debug_print(*ec,  "[QE] getSubviews - Error in getObjectPointer.");
			return errcode;
		}
		DataValue* data_value = optr->getValue();
		int vType = data_value->getType();
		ExtendedType extType = data_value->getSubtype();
		if ((vType == Store::Vector) && (extType == Store::View)) {
			vector<LogicalID*>* tmp_vec = (data_value->getVector());
			int vec_size = tmp_vec->size();
			for (int i = 0; i < vec_size; i++ ) {
				LogicalID *tmp_logID = tmp_vec->at(i);
				ObjectPointer *tmp_optr;
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
					debug_print(*ec,  "[QE] getSubviews - Error in getObjectPointer");
					return errcode;
				}
				string tmp_name = tmp_optr->getName();
				DataValue* tmp_data_value = tmp_optr->getValue();
				int tmp_vType = tmp_data_value->getType();
				ExtendedType tmp_extType = tmp_data_value->getSubtype();
				if ((tmp_vType == Store::Vector) && (tmp_extType == Store::View)) {
					subviews.push_back(tmp_logID);
				}
			}
		}
		else {
			debug_print(*ec,  "[QE] getSubviews error: This is not a view");
			debug_print(*ec,  (ErrQExecutor | EBadViewDef));
			return (ErrQExecutor | EBadViewDef);
		}
		return 0;
	}

	int ExecutorViews::getSubview(LogicalID *lid, string name, LogicalID *&subview_lid, Transaction *tr) {
		int errcode;
		ObjectPointer *optr;
		errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
		if (errcode != 0) {
			debug_print(*ec,  "[QE] getSubview - Error in getObjectPointer.");
			return errcode;
		}
		DataValue* data_value = optr->getValue();
		int vType = data_value->getType();
		ExtendedType extType = data_value->getSubtype();
		if ((vType == Store::Vector) && (extType == Store::View)) {
			vector<LogicalID*>* tmp_vec = (data_value->getVector());
			int vec_size = tmp_vec->size();
			for (int i = 0; i < vec_size; i++ ) {
				LogicalID *tmp_logID = tmp_vec->at(i);
				ObjectPointer *tmp_optr;
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
					debug_print(*ec,  "[QE] getSubview - Error in getObjectPointer");
					return errcode;
				}
				string tmp_name = tmp_optr->getName();
				DataValue* tmp_data_value = tmp_optr->getValue();
				int tmp_vType = tmp_data_value->getType();
				ExtendedType tmp_extType = tmp_data_value->getSubtype();
				if ((tmp_vType == Store::Vector) && (tmp_extType == Store::View)) {
					vector<LogicalID*>* tmp_vec2 = (tmp_data_value->getVector());
					int vec_size2 = tmp_vec2->size();
					for (int j = 0; j < vec_size2; j++) {
						LogicalID *tmp_logID2 = tmp_vec2->at(j);
						ObjectPointer *tmp_optr2;
						if ((errcode = tr->getObjectPointer(tmp_logID2, Store::Read, tmp_optr2, false)) != 0) {
							debug_print(*ec,  "[QE] getSubview - Error in getObjectPointer");
							return errcode;
						}
						string tmp_name2 = tmp_optr2->getName();
						if (tmp_name2 == QE_VIEWDEF_VIRTUALOBJECTS_NAME) {
							DataValue* tmp_data_value2 = tmp_optr2->getValue();
							int tmp_vType2 = tmp_data_value2->getType();
							if (tmp_vType2 == Store::String) {
								string curr_subwiew_name = tmp_data_value2->getString();
								if (curr_subwiew_name == name) {
									subview_lid = tmp_logID;
									return 0;
								}
							}
						}
					}
				}
			}
		}
		else {
			debug_print(*ec,  "[QE] getSubview error: This is not a view");
			debug_print(*ec,  (ErrQExecutor | EBadViewDef));
			return (ErrQExecutor | EBadViewDef);
		}
		return 0;
	}


	int ExecutorViews::getOn_procedure(LogicalID *lid, string procName, string &code, string &param, Transaction *tr) {
		code = "";
		param = "";
		int errcode;
		ObjectPointer *optr;
		errcode = tr->getObjectPointer (lid, Store::Read, optr, false);
		if (errcode != 0) {
			debug_print(*ec,  "[QE] getOn_procedure - Error in getObjectPointer.");
			return errcode;
		}
		DataValue* data_value = optr->getValue();
		int vType = data_value->getType();
		ExtendedType extType = data_value->getSubtype();
		if ((vType == Store::Vector) && (extType == Store::View)) {
			vector<LogicalID*>* tmp_vec = (data_value->getVector());
			int vec_size = tmp_vec->size();
			int founded = 0;
			for (int i = 0; i < vec_size; i++ ) {
				LogicalID *tmp_logID = tmp_vec->at(i);
				ObjectPointer *tmp_optr;
				if ((errcode = tr->getObjectPointer(tmp_logID, Store::Read, tmp_optr, false)) != 0) {
					debug_print(*ec,  "[QE] getOn_procedure - Error in getObjectPointer");
					return errcode;
				}
				string tmp_name = tmp_optr->getName();
				DataValue* tmp_data_value = tmp_optr->getValue();
				int tmp_vType = tmp_data_value->getType();
				ExtendedType tmp_extType = tmp_data_value->getSubtype();
				if ((procName == tmp_name) && (tmp_vType == Store::Vector) && (tmp_extType == Store::Procedure)) {
					founded++;
					vector<LogicalID*>* proc_vec = tmp_data_value->getVector();
					int proc_vec_size = proc_vec->size();
					if ((proc_vec_size == 0) || (proc_vec_size > 2)) {
						debug_printf(*ec, "[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
						debug_print(*ec,  (ErrQExecutor | EBadViewDef));
						return (ErrQExecutor | EBadViewDef);
					}
					LogicalID *first_logID = proc_vec->at(0);
					ObjectPointer *first_optr;
					if ((errcode = tr->getObjectPointer(first_logID, Store::Read, first_optr, false)) != 0) {
						debug_print(*ec,  "[QE] getOn_procedure - Error in getObjectPointer");
						return errcode;
					}
					string first_name = first_optr->getName();
					DataValue* first_data_value = first_optr->getValue();
					int first_vType = first_data_value->getType();
					ExtendedType first_extType = first_data_value->getSubtype();
					if ((first_name == QE_METHOD_PROCBODY_BIND_NAME) && (first_vType == Store::String) && (first_extType == Store::None)) {
						code = first_data_value->getString();
					}
					else if ((first_name == QE_METHOD_PARAM_BIND_NAME) && (first_vType == Store::String) && (first_extType == Store::None)) {
						param = first_data_value->getString();
					}
					else {
						debug_printf(*ec, "[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
						debug_print(*ec,  (ErrQExecutor | EBadViewDef));
						return (ErrQExecutor | EBadViewDef);
					}

					if (proc_vec_size == 2) {
						LogicalID *second_logID = proc_vec->at(1);
						ObjectPointer *second_optr;
						if ((errcode = tr->getObjectPointer(second_logID, Store::Read, second_optr, false)) != 0) {
							debug_print(*ec,  "[QE] getOn_procedure - Error in getObjectPointer");
							return errcode;
						}
						string second_name = second_optr->getName();
						DataValue* second_data_value = second_optr->getValue();
						int second_vType = second_data_value->getType();
						ExtendedType second_extType = second_data_value->getSubtype();
						if ((second_name == QE_METHOD_PROCBODY_BIND_NAME) && (second_vType == Store::String) && (second_extType == Store::None) && code == "") {
							code = second_data_value->getString();
						}
						else if ((second_name == QE_METHOD_PARAM_BIND_NAME) && (second_vType == Store::String) && (second_extType == Store::None) && param == "") {
							param = second_data_value->getString();
						}
						else {
							debug_printf(*ec, "[QE] getOn_procedure error: %s procedure wrong format\n", procName.c_str());
							debug_print(*ec,  (ErrQExecutor | EBadViewDef));
							return (ErrQExecutor | EBadViewDef);
						}
					}

				}
			}
			if (founded > 1) {
				debug_printf(*ec, "[QE] getOn_procedure error: Multiple %s procedures found\n", procName.c_str());
				debug_print(*ec,  (ErrQExecutor | EBadViewDef));
				return (ErrQExecutor | EBadViewDef);
			}
		}
		else {
			debug_print(*ec,  "[QE] getOn_procedure error: This is not a view");
			debug_print(*ec,  (ErrQExecutor | EBadViewDef));
			return (ErrQExecutor | EBadViewDef);
		}
		return 0;
	}

	int ExecutorViews::deVirtualize(QueryResult *arg, QueryResult *&res) {
		//*ec << "[QE] deVirtualize()";
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
					errcode = this->deVirtualize(tmp_item, tmp_res);
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
					errcode = this->deVirtualize(tmp_item, tmp_res);
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
					errcode = this->deVirtualize(tmp_item, tmp_res);
					if (errcode != 0) return errcode;
					res->addResult(tmp_res);
				}
				break;
			}
			case QueryResult::QBINDER: {
				string tmp_name = ((QueryBinderResult *) arg)->getName();
				QueryResult *tmp_item;
				errcode = this->deVirtualize(((QueryBinderResult *) arg)->getItem(), tmp_item);
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
				break;
			}
			case QueryResult::QNOTHING: {
				res = arg;
				break;
			}
			case QueryResult::QVIRTUAL: {
				LogicalID *tmp_lid;
				string virtualToString = arg->toString(0, true);
				if (fakeLid_map.find(virtualToString) != fakeLid_map.end()) {
					tmp_lid = fakeLid_map[virtualToString];
				}
				else {
					unsigned int current_virtual_index = sent_virtuals.size();
					if (current_virtual_index >= QE_VIRTUALS_TO_SEND_MAX_COUNT) {
						debug_print(*ec,  "[QE] No more space left to remember next virtual object.");
						debug_print(*ec,  (ErrQExecutor | EQEUnexpectedErr));
						return (ErrQExecutor | EQEUnexpectedErr);
					}
					sent_virtuals.push_back(arg);
					unsigned int new_id = QE_VIRTUALS_TO_SEND_MIN_ID + current_virtual_index;
					tmp_lid = new DBLogicalID(new_id);
					fakeLid_map[virtualToString] = tmp_lid;
				}
				res = new QueryReferenceResult(new DBLogicalID(tmp_lid->toInteger()));
				break;
			}
			default: {
				debug_print(*ec,  "[QE] ERROR in deVirtualize() - unknown result type");
				debug_print(*ec,  (ErrQExecutor | EOtherResExp));
				return (ErrQExecutor | EOtherResExp);
			}
		}
		return 0;
	}

	int ExecutorViews::reVirtualize(QueryResult *arg, QueryResult *&res) {
		//*ec << "[QE] reVirtualize()";
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
					errcode = this->reVirtualize(tmp_item, tmp_res);
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
					errcode = this->reVirtualize(tmp_item, tmp_res);
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
					errcode = this->reVirtualize(tmp_item, tmp_res);
					if (errcode != 0) return errcode;
					res->addResult(tmp_res);
				}
				break;
			}
			case QueryResult::QBINDER: {
				string tmp_name = ((QueryBinderResult *) arg)->getName();
				QueryResult *tmp_item;
				errcode = this->reVirtualize(((QueryBinderResult *) arg)->getItem(), tmp_item);
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
				unsigned int lid_number = (((QueryReferenceResult *) arg)->getValue())->toInteger();
				if ((lid_number >= QE_VIRTUALS_TO_SEND_MIN_ID) &&
				(lid_number < (QE_VIRTUALS_TO_SEND_MIN_ID + QE_VIRTUALS_TO_SEND_MAX_COUNT))) {
					res = sent_virtuals.at(lid_number - QE_VIRTUALS_TO_SEND_MIN_ID);
				}
				else res = arg;
				break;
			}
			case QueryResult::QNOTHING: {
				res = arg;
				break;
			}
			default: {
				debug_print(*ec,  "[QE] ERROR in reVirtualize() - unknown result type");
				debug_print(*ec,  (ErrQExecutor | EOtherResExp));
				return (ErrQExecutor | EOtherResExp);
			}
		}
		return 0;
	}
}

