#include <Graphics/Model.h>
#include <Graphics/Loaders/OBJ.h>
#include <Renderer/Pipelines/Graphics.h>
#include <Maths/Angles.h>

#include <unordered_map>

namespace Scop
{
	Model::Model(std::shared_ptr<Mesh> mesh) : p_mesh(mesh)
	{
		if(p_mesh)
			m_materials.resize(p_mesh->GetSubMeshCount() + 1);

		if(!s_default_material)
		{
			CPUBuffer default_pixels{ kvfFormatSize(VK_FORMAT_R8G8B8A8_SRGB) };
			default_pixels.GetDataAs<std::uint32_t>()[0] = 0xFFFFFFFF;
			std::shared_ptr<Texture> default_texture = std::make_shared<Texture>(std::move(default_pixels), 1, 1, VK_FORMAT_R8G8B8A8_SRGB);

			MaterialTextures textures;
			textures.albedo = default_texture;
			s_default_material = std::make_shared<Material>(textures);
		}
		m_materials.back() = s_default_material;
	}

	void Model::Draw(VkCommandBuffer cmd, std::shared_ptr<DescriptorSet> matrices_set, const GraphicPipeline& pipeline, std::shared_ptr<DescriptorSet> set, std::size_t& drawcalls, std::size_t& polygondrawn, std::size_t frame_index) const
	{
		if(!p_mesh)
			return;
		for(std::size_t i = 0; i < GetSubMeshCount(); i++)
		{
			std::shared_ptr<Material> material;
			if(!m_materials[i])
				material = m_materials.back();
			else
				material = m_materials[i];
			if(!material->IsSetInit())
				material->UpdateDescriptorSet(set);
			material->Bind(frame_index, cmd);
			std::array<VkDescriptorSet, 2> sets = { matrices_set->GetSet(frame_index), material->GetSet(frame_index) };
			RenderCore::Get().vkCmdBindDescriptorSets(cmd, pipeline.GetPipelineBindPoint(), pipeline.GetPipelineLayout(), 0, sets.size(), sets.data(), 0, nullptr);
			p_mesh->Draw(cmd, drawcalls, polygondrawn, i);
		}
	}

	RadianAnglef GetAngleBetweenVectors(const Vec3f& a, const Vec3f& b) noexcept
	{
		float cosine_theta = (a.DotProduct(b)) / (a.GetLength() * b.GetLength());
		RadianAnglef angle(std::acos(cosine_theta));
		return angle;
	}

	Model LoadModelFromObjFile(std::filesystem::path path) noexcept
	{
		auto obj_data = LoadObjFromFile(path);
		if(!obj_data)
			return { nullptr };
		TesselateObjData(*obj_data);
		ObjModel obj_model = ConvertObjDataToObjModel(*obj_data);
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		float min_x = std::numeric_limits<float>::max(), max_x = std::numeric_limits<float>::lowest();
		float min_y = std::numeric_limits<float>::max(), max_y = std::numeric_limits<float>::lowest();
		float min_z = std::numeric_limits<float>::max(), max_z = std::numeric_limits<float>::lowest();

		bool needs_to_generate_normals = obj_model.normal.empty();
		std::unordered_map<std::string, std::vector<Vec3f>> generated_normals;
		if(needs_to_generate_normals)
		{
			for(auto& [group, faces] : obj_model.faces)
			{
				generated_normals[group] = std::vector<Vec3f>(faces.size(), Vec3f{});
				for(std::size_t i = 0; i < faces.size(); i += 3)
				{
					Vec3f vec_a{ obj_model.vertex[faces[i + 1]] - obj_model.vertex[faces[i]] };
					Vec3f vec_b{ obj_model.vertex[faces[i + 2]] - obj_model.vertex[faces[i]] };
					Vec3f normal = vec_a.CrossProduct(vec_b).Normalize();
					generated_normals[group][i + 0] = normal;
					generated_normals[group][i + 1] = normal;
					generated_normals[group][i + 2] = normal;
				}
			}
		}

		for(auto& [group, faces] : obj_model.faces)
		{
			std::vector<Vertex> vertices;
			std::vector<std::uint32_t> indices;
			for(std::size_t i = 0; i < faces.size(); i++)
			{
				min_x = std::min(obj_model.vertex[faces[i]].x, min_x);
				min_y = std::min(obj_model.vertex[faces[i]].y, min_y);
				min_z = std::min(obj_model.vertex[faces[i]].z, min_z);
				max_x = std::max(obj_model.vertex[faces[i]].x, max_x);
				max_y = std::max(obj_model.vertex[faces[i]].y, max_y);
				max_z = std::max(obj_model.vertex[faces[i]].z, max_z);

				Vec4f color{};
				switch(i % 10)
				{
					case 0:  color = Vec4f{ 1.0f, 0.0f, 1.0f, 1.0f }; break;
					case 1:  color = Vec4f{ 1.0f, 1.0f, 0.0f, 1.0f }; break;
					case 2:  color = Vec4f{ 1.0f, 0.5f, 0.0f, 1.0f }; break;
					case 3:  color = Vec4f{ 1.0f, 0.0f, 0.0f, 1.0f }; break;
					case 4:  color = Vec4f{ 0.2f, 0.0f, 0.8f, 1.0f }; break;
					case 5:  color = Vec4f{ 0.0f, 1.0f, 1.0f, 1.0f }; break;
					case 6:  color = Vec4f{ 0.0f, 1.0f, 0.0f, 1.0f }; break;
					case 7:  color = Vec4f{ 0.0f, 0.0f, 1.0f, 1.0f }; break;
					case 8:  color = Vec4f{ 0.3f, 0.0f, 0.4f, 1.0f }; break;
					default: color = Vec4f{ 1.0f, 1.0f, 1.0f, 1.0f }; break;
				}

				Vec3f normal{};
				if(needs_to_generate_normals)
				{
					normal = generated_normals[group][i];
					for(std::size_t j = 0; j < faces.size(); j++)
					{
						if(faces[j] == faces[i] && i != j)
						{
							RadianAnglef angle = GetAngleBetweenVectors(generated_normals[group][i], generated_normals[group][j]);
							if(angle.ToDegrees() < 89.0f)
								normal += generated_normals[group][j];
						}
					}
				}
				else
					normal = obj_model.normal[faces[i]];

				Vertex v(
					Vec4f{
						obj_model.vertex[faces[i]],
						1.0f
					},
					color,
					Vec4f{
						normal.Normalize(),
						1.0f
					},
					(obj_model.tex_coord.empty() ?
						Vec2f{ (obj_model.vertex[faces[i]].x - min_x) / (max_x - min_x), 1.0f - ((obj_model.vertex[faces[i]].y - min_y) / (max_y - min_y)) }
						:
						obj_model.tex_coord[faces[i]]
					)
				);
				indices.push_back(vertices.size());
				vertices.push_back(std::move(v));
			}

			mesh->AddSubMesh({ vertices, indices });
		}
		Model model(mesh);
		model.m_center = Vec3f{ (min_x + max_x) / 2.0f, (min_y + max_y) / 2.0f, (min_z + max_z) / 2.0f };
		return model;
	}
}
