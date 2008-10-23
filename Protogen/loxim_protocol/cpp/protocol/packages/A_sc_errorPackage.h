#ifndef A_SC_ERRORPACKAGE_H_
#define A_SC_ERRORPACKAGE_H_

#include <stdint.h>
#include "../ptools/Package.h"
#include "../ptools/CharArray.h"
#include "../enums/enums.h"

namespace protocol{

#define ID_A_sc_errorPackage 2

class A_sc_errorPackage : public Package
{
	private:
		enum ErrorsEnum error_code;
		int64_t error_object_id;
		bool bnull_error_object_id;
		CharArray* description;
		uint32_t line;
		uint32_t col;
	public:
		A_sc_errorPackage();
		~A_sc_errorPackage();
		A_sc_errorPackage(
			enum ErrorsEnum a_error_code,\
			int64_t a_error_object_id,\
			CharArray* a_description,\
			uint32_t a_line,\
			uint32_t a_col
			,bool a_bnull_error_object_id=false
			);

		virtual bool equals(Package* _package);

		enum ErrorsEnum getError_code(){return error_code;};
		void setError_code(enum ErrorsEnum a_error_code){error_code=a_error_code;};

		int64_t getError_object_id(){return error_object_id;};
		bool getBnull_error_object_id(){return bnull_error_object_id;};
		void setError_object_id(int64_t a_error_object_id){error_object_id=a_error_object_id;};
		void setBnull_error_object_id(bool a_bnull_error_object_id){bnull_error_object_id=a_bnull_error_object_id;};

		CharArray* getDescription(){return description;};
		void setDescription(CharArray* a_description){description=a_description;};

		uint32_t getLine(){return line;};
		void setLine(uint32_t a_line){line=a_line;};

		uint32_t getCol(){return col;};
		void setCol(uint32_t a_col){col=a_col;};


		uint8_t getPackageType(){return ID_A_sc_errorPackage;};
	protected:
		void serializeW(PackageBufferWriter *writter);
		bool deserializeR(PackageBufferReader *reader);
};//class

}//namespace
#endif /*define A_SC_ERRORPACKAGE_H_*/
