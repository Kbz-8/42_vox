#ifndef __SCOP_GRAPHICS_ENUMS__
#define __SCOP_GRAPHICS_ENUMS__

#include <cstddef>

namespace Scop
{
	enum class CullMode
	{
		None = 0,
		Back,
		Front,
		FrontAndBack,

		EndEnum
	};
	constexpr std::size_t CullModeCount = static_cast<std::size_t>(CullMode::EndEnum);
}

#endif
