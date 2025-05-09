#ifndef __SCOP_CORE_UUID__
#define __SCOP_CORE_UUID__

#include <cstdint>

namespace Scop
{
	class UUID
	{
		public:
			UUID();
			inline operator std::uint64_t() const noexcept { return m_uuid; }

		private:
			std::uint64_t m_uuid;
	};
}

#endif
