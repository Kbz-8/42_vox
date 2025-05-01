#include <Graphics/Loaders/OBJ.h>
#include <Core/Logs.h>

#include <set>
#include <fstream>
#include <algorithm>

namespace Scop
{
	std::optional<ObjData> LoadObjFromFile(const std::filesystem::path& path)
	{
		if(!std::filesystem::exists(path))
		{
			Error("OBJ loader : OBJ file does not exists; %", path);
			return std::nullopt;
		}
		char line[1024];
		std::string op;
		std::istringstream line_in;
		std::set<std::string> groups;
		groups.insert("default");

		std::ifstream in(path);

		ObjData data;

		while(in.good())
		{
			in.getline(line, 1023);
			line_in.clear();
			line_in.str(line);

			if(!(line_in >> op))
				continue;

			if(op == "v")
			{
				Vec3f v;
				line_in >> v.x;
				line_in >> v.y;
				line_in >> v.z;
				data.vertex.push_back(std::move(v));
			}
			else if(op == "vt")
			{
				Vec2f v;
				line_in >> v.x;
				line_in >> v.y;
				data.tex_coord.push_back(std::move(v));
			}
			else if(op == "vn")
			{
				Vec3f v;
				line_in >> v.x;
				line_in >> v.y;
				line_in >> v.z;
				data.normal.push_back(std::move(v));
			}
			else if(op == "vc")
			{
				Vec4f v;
				line_in >> v.x;
				line_in >> v.y;
				line_in >> v.z;
				line_in >> v.w;
				data.color.push_back(std::move(v));
			}
			else if(op == "g")
			{
				groups.clear();
				while(line_in >> groups);
				groups.insert("default");
			}
			else if(op == "f")
			{
				std::vector<ObjData::FaceVertex> list;
				while(line_in >> list);
				for(const auto& group : groups)
				{
					ObjData::FaceList& fl = data.faces[group];
					fl.second.push_back(fl.first.size());
					fl.first.insert(fl.first.end(), list.begin(), list.end());
				}
			}
		}
		for(auto& [_, face] : data.faces)
		{
			ObjData::FaceList& fl = face;
			fl.second.push_back(fl.first.size());
		}
		Message("OBJ Loader : loaded %", path);
		return data;
	}

	static void TesselateObjData(std::vector<ObjData::FaceVertex>& input, std::vector<std::uint32_t>& input_start) noexcept
	{
		std::vector<ObjData::FaceVertex> output;
		std::vector<std::uint32_t> output_start;
		output.reserve(input.size());
		output_start.reserve(input_start.size());
		for(auto s = input_start.begin(); s != input_start.end() - 1; ++s)
		{
			const std::uint32_t size = *(s + 1) - *s;
			if(size > 3)
			{
				const ObjData::FaceVertex & start_vertex = input[*s];
				for(std::size_t i = 1; i < size - 1; i++)
				{
					output_start.push_back(output.size());
					output.push_back(start_vertex);
					output.push_back(input[*s + i]);
					output.push_back(input[*s + i + 1]);
				}
			}
			else
			{
				output_start.push_back(output.size());
				output.insert(output.end(), input.begin() + *s, input.begin() + *(s + 1));
			}
		}
		output_start.push_back(output.size());
		input.swap(output);
		input_start.swap(output_start);
	}

	void TesselateObjData(ObjData& obj)
	{
		for(auto& face : obj.faces)
		{
			ObjData::FaceList& fl = face.second;
			TesselateObjData(fl.first, fl.second);
		}
		Message("OBJ Loader : object data tesselated");
	}

	ObjModel ConvertObjDataToObjModel(const ObjData& data)
	{
		std::vector<ObjData::FaceVertex> unique(data.faces.find("default")->second.first);
		std::sort(unique.begin(), unique.end());
		//unique.erase(std::unique(unique.begin(), unique.end()), unique.end());

		ObjModel model;
		for(auto& face : unique)
		{
			model.vertex.push_back(data.vertex[face.v]);
			if(!data.tex_coord.empty())
			{
				const int index = (face.t > -1) ? face.t : face.v;
				model.tex_coord.push_back(data.tex_coord[index]);
			}
			if(!data.normal.empty())
			{
				const int index = (face.n > -1) ? face.n : face.v;
				model.normal.push_back(data.normal[index]);
			}
			if(!data.color.empty())
			{
				const int index = face.v;
				model.color.push_back(data.color[index]);
			}
		}
		for(auto& [group, faces] : data.faces)
		{
			std::vector<std::uint32_t>& v = model.faces[group];
			v.reserve(faces.first.size());
			for(auto& face : faces.first)
				v.push_back(std::distance(unique.begin(), std::lower_bound(unique.begin(), unique.end(), face)));
		}
		return model;
	}
}
