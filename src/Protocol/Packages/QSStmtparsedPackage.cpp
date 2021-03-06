/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <sstream>
#include <Protocol/Enums/DataPackages.h>
#include <Protocol/DataPackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/PackageFactory.h>
#include <Protocol/Enums/Packages.h>
#include <Protocol/Packages/QSStmtparsedPackage.h>

using namespace std;

namespace Protocol {
	QSStmtparsedPackage::QSStmtparsedPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream):

		statement_id(stream.read_uint64(mask, cancel, length)),
		params_cnt(stream.read_uint32(mask, cancel, length))
	{
	}

	QSStmtparsedPackage::QSStmtparsedPackage(uint64_t statement_id, uint32_t params_cnt):

		statement_id(statement_id),
		params_cnt(params_cnt)
	{
	}

	void QSStmtparsedPackage::serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header) const
	{
		if (with_header){
			stream.write_uint8(mask, cancel, get_type());
			stream.write_uint32(mask, cancel, get_ser_size());
		}
		stream.write_uint64(mask, cancel, statement_id);
		stream.write_uint32(mask, cancel, params_cnt);
	}

	uint8_t QSStmtparsedPackage::get_type() const
	{
		return Q_S_STMTPARSED_PACKAGE;
	}

	string QSStmtparsedPackage::to_string() const
	{
		stringstream ss;
		ss << "QSStmtparsedPackage:" << endl;
		ss << "  statement_id: " << statement_id << endl;
		ss << "  params_cnt: " << params_cnt << endl;
		return ss.str();
	}

	size_t QSStmtparsedPackage::get_ser_size() const
	{
		return 0 + 8 + 4;
	}

	uint64_t QSStmtparsedPackage::get_val_statement_id() const
	{
		return statement_id;
	}
	uint32_t QSStmtparsedPackage::get_val_params_cnt() const
	{
		return params_cnt;
	}
}
