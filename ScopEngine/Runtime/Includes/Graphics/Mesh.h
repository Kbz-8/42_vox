#ifndef __SCOPE_RENDERER_MESH__
#define __SCOPE_RENDERER_MESH__

#include <vector>
#include <cstdint>
#include <cstring>

#include <Renderer/Vertex.h>
#include <Renderer/Buffer.h>
#include <Utils/Buffer.h>

namespace Scop
{
	class Mesh
	{
		public:
			struct SubMesh
			{
				MeshBuffer buffer;
				std::size_t index_size;
				std::size_t triangle_count = 0;

				inline SubMesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
				{
					CPUBuffer data(vertices.size() * sizeof(Vertex) + indices.size() * sizeof(std::uint32_t));
					std::memcpy(data.GetData(), vertices.data(), vertices.size() * sizeof(Vertex));
					std::memcpy(data.GetData() + vertices.size() * sizeof(Vertex), indices.data(), indices.size() * sizeof(std::uint32_t));
					buffer.Init(vertices.size() * sizeof(Vertex), indices.size() * sizeof(std::uint32_t), 0, std::move(data));
					triangle_count = vertices.size() / 3;
					index_size = indices.size();
				}
			};

		public:
			Mesh() = default;

			void Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t& polygondrawn) const noexcept;
			void Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t& polygondrawn, std::size_t submesh_index) const noexcept;

			inline std::size_t GetSubMeshCount() const { return m_sub_meshes.size(); }

			inline void AddSubMesh(SubMesh mesh) { m_sub_meshes.emplace_back(std::move(mesh)); }
			[[nodiscard]] inline SubMesh& GetSubMesh(std::size_t index) { return m_sub_meshes.at(index); }

			~Mesh();

		private:
			std::vector<SubMesh> m_sub_meshes;
	};
}

#endif
