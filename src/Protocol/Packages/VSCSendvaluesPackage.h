#ifndef PROTOCOL_V_SC_SENDVALUES_PACKAGE_H
#define PROTOCOL_V_SC_SENDVALUES_PACKAGE_H

/* This file is generated by lw_protogen. DO NOT EDIT.*/
#include <string>
#include <memory>
#include <Protocol/Packages/Package.h>
#include <Protocol/ByteBuffer.h>

namespace Protocol {
	class VSCSendvaluesPackage : public Package {
		private:
			VarUint root_value_id;
			VarUint o_bundles_count;
			VarUint o_bid_count;
			VarUint p_vid_count;
		public:
			VSCSendvaluesPackage(const sigset_t &mask, const bool &cancel, size_t &length, DataStream &stream);
			VSCSendvaluesPackage(VarUint root_value_id, VarUint o_bundles_count, VarUint o_bid_count, VarUint p_vid_count);

			void serialize(const sigset_t &mask, const bool& cancel, DataStream &stream, bool with_header = true) const;
			uint8_t get_type() const;
			std::string to_string() const;
			size_t get_ser_size() const;
			VarUint get_val_root_value_id() const;
			VarUint get_val_o_bundles_count() const;
			VarUint get_val_o_bid_count() const;
			VarUint get_val_p_vid_count() const;
	};
}
#endif
