#ifndef __SCOP_OBJ_LOADER__
#define __SCOP_OBJ_LOADER__

#include <map>
#include <set>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <filesystem>

#include <Maths/Vec2.h>
#include <Maths/Vec3.h>
#include <Maths/Vec4.h>

namespace Scop
{
	struct ObjData
	{
		struct FaceVertex
		{
			FaceVertex() : v(-1), t(-1), n(-1) {}
			std::int32_t v;
			std::int32_t t;
			std::int32_t n;

			inline bool operator<(const FaceVertex& rhs) const
			{
				return (v < rhs.v) || (v == rhs.v && t < rhs.t ) || (v == rhs.v && t == rhs.t && n < rhs.n);
			}
			inline bool operator==(const FaceVertex& rhs) const
			{
				return (v == rhs.v && t == rhs.t && n == rhs.n);
			}
		};

		using FaceList = std::pair<std::vector<FaceVertex>, std::vector<std::uint32_t>>;

		std::vector<Vec4f> color;
		std::vector<Vec3f> vertex;
		std::vector<Vec3f> normal;
		std::vector<Vec2f> tex_coord;

		std::map<std::string, FaceList> faces;
	};

	struct ObjModel
	{
		std::vector<Vec4f> color;
		std::vector<Vec3f> vertex;
		std::vector<Vec3f> normal;
		std::vector<Vec2f> tex_coord;

		std::map<std::string, std::vector<std::uint32_t>> faces;
	};

	std::optional<ObjData> LoadObjFromFile(const std::filesystem::path& path);
	void TesselateObjData(ObjData& data);
	ObjModel ConvertObjDataToObjModel(const ObjData& data);

	template<typename T>
	inline std::istream& operator>>(std::istream& in, std::vector<T>& vec);

	template<typename T>
	inline std::istream& operator>>(std::istream& in, std::set<T>& vec);

	inline std::istream& operator>>(std::istream& in, ObjData::FaceVertex& f);
}

#include <Graphics/Loaders/OBJ.inl>

#endif
