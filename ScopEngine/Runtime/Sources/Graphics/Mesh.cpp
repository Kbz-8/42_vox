#include <Graphics/Mesh.h>
#include <Utils/Buffer.h>
#include <cstring>

namespace Scop
{
	void Mesh::Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t& polygondrawn) const noexcept
	{
		for(std::size_t i = 0; i < m_sub_meshes.size(); i++)
			Draw(cmd, drawcalls, polygondrawn, i);
	}

	void Mesh::Draw(VkCommandBuffer cmd, std::size_t& drawcalls, std::size_t& polygondrawn, std::size_t submesh_index) const noexcept
	{
		Verify(submesh_index < m_sub_meshes.size(), "invalid submesh index");
		m_sub_meshes[submesh_index].buffer.BindVertex(cmd);
		m_sub_meshes[submesh_index].buffer.BindIndex(cmd);
		RenderCore::Get().vkCmdDrawIndexed(cmd, m_sub_meshes[submesh_index].index_size, 1, 0, 0, 0);
		polygondrawn += m_sub_meshes[submesh_index].triangle_count;
		drawcalls++;
	}

	Mesh::~Mesh()
	{
		for(auto& mesh : m_sub_meshes)
			mesh.buffer.Destroy();
	}
}
