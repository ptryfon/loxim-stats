#ifndef PROTOCOL_VARUINT
#define PROTOCOL_VARUINT

#include <stdint.h>

namespace Protocol {
	class VarUint {
		private:
			uint64_t val;
			bool null;
		public:
			VarUint(uint64_t val, bool null):
				val(val),
				null(null)
			{
			}

			VarUint(const VarUint &v2):
				val(v2.val),
				null(v2.null)
			{
			}

			uint64_t get_val() const
			{
				return val;
			}

			bool is_null() const
			{
				return null;
			}

			VarUint &operator=(const VarUint &v2)
			{
				val = v2.val;
				null = v2.null;
				return *this;
			}



	};
}

#endif /* PROTOCOL_VARUINT */
