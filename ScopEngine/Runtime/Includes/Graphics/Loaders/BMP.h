#ifndef __SCOP_BMP_LOADER__
#define __SCOP_BMP_LOADER__

#include <filesystem>

#include <Maths/Vec2.h>
#include <Utils/Buffer.h>

namespace Scop
{
	CPUBuffer LoadBMPFile(const std::filesystem::path& path, Vec2ui32& dimensions);
}

#endif
