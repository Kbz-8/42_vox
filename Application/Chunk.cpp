#include <Chunk.h>
#include <Block.h>
#include <World.h>

#include <cmath>

Chunk::Chunk(World& world, Scop::Vec2i offset) : m_offset(offset), m_position(std::move(offset) * Scop::Vec2i{ CHUNK_SIZE.x, CHUNK_SIZE.z }), m_world(world)
{
	m_data.resize(CHUNK_SIZE.x);
	for(auto& z: m_data)
	{
		z.resize(CHUNK_SIZE.z);
		for(auto& y: z)
			y.resize(CHUNK_SIZE.y);
	}
}

void Chunk::GenerateChunk()
{
	for(auto& z: m_data)
	{
		for(auto& y: z)
			std::fill(y.begin(), y.end(), 0);
	}

	std::vector<Scop::Vertex> mesh_data;
	std::vector<std::uint32_t> indices;

	for(std::uint32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::uint32_t z = 0; z < CHUNK_SIZE.z; z++)
		{
			// Implement noise here
			std::uint32_t height = 4 + std::sin(x) + std::cos(z);
			for(std::uint32_t y = 0; y < height; y++)
				m_data[x][z][y] = 1;
		}
	}

	for(std::uint32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::uint32_t z = 0; z < CHUNK_SIZE.z; z++)
		{
			for(std::uint32_t y = 0; y < CHUNK_SIZE.y; y++)
			{
				if(!GetBlock(Scop::Vec3i(x, y, z)))
					continue;
				if(!GetBlock(Scop::Vec3i(x, y, static_cast<std::int32_t>(z) - 1)))
				{
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[0].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[0].normal,  BLOCK_MESH[0].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[1].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[1].normal,  BLOCK_MESH[1].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[2].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[2].normal,  BLOCK_MESH[2].uv));

					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[3].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[3].normal,  BLOCK_MESH[3].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[4].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[4].normal,  BLOCK_MESH[4].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[5].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[5].normal,  BLOCK_MESH[5].uv));
				}

				if(!GetBlock(Scop::Vec3i(x, y, z + 1)))
				{
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[6].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[6].normal,  BLOCK_MESH[6].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[7].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[7].normal,  BLOCK_MESH[7].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[8].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[8].normal,  BLOCK_MESH[8].uv));

					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[9].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[9].normal,  BLOCK_MESH[9].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[10].position + Scop::Vec3f(x, y, z), BLOCK_MESH[10].normal, BLOCK_MESH[10].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[11].position + Scop::Vec3f(x, y, z), BLOCK_MESH[11].normal, BLOCK_MESH[11].uv));
				}

				if(!GetBlock(Scop::Vec3i(static_cast<std::int32_t>(x) - 1, y, z)))
				{
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[12].position + Scop::Vec3f(x, y, z), BLOCK_MESH[12].normal, BLOCK_MESH[12].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[13].position + Scop::Vec3f(x, y, z), BLOCK_MESH[13].normal, BLOCK_MESH[13].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[14].position + Scop::Vec3f(x, y, z), BLOCK_MESH[14].normal, BLOCK_MESH[14].uv));

					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[15].position + Scop::Vec3f(x, y, z), BLOCK_MESH[15].normal, BLOCK_MESH[15].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[16].position + Scop::Vec3f(x, y, z), BLOCK_MESH[16].normal, BLOCK_MESH[16].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[17].position + Scop::Vec3f(x, y, z), BLOCK_MESH[17].normal, BLOCK_MESH[17].uv));
				}

				if(!GetBlock(Scop::Vec3i(x + 1, y, z)))
				{
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[18].position + Scop::Vec3f(x, y, z), BLOCK_MESH[18].normal, BLOCK_MESH[18].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[19].position + Scop::Vec3f(x, y, z), BLOCK_MESH[19].normal, BLOCK_MESH[19].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[20].position + Scop::Vec3f(x, y, z), BLOCK_MESH[20].normal, BLOCK_MESH[20].uv));

					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[21].position + Scop::Vec3f(x, y, z), BLOCK_MESH[21].normal, BLOCK_MESH[21].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[22].position + Scop::Vec3f(x, y, z), BLOCK_MESH[22].normal, BLOCK_MESH[22].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[23].position + Scop::Vec3f(x, y, z), BLOCK_MESH[23].normal, BLOCK_MESH[23].uv));
				}

				if(!GetBlock(Scop::Vec3i(x, static_cast<std::int32_t>(y) - 1, z)))
				{
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[24].position + Scop::Vec3f(x, y, z), BLOCK_MESH[24].normal, BLOCK_MESH[24].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[25].position + Scop::Vec3f(x, y, z), BLOCK_MESH[25].normal, BLOCK_MESH[25].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[26].position + Scop::Vec3f(x, y, z), BLOCK_MESH[26].normal, BLOCK_MESH[26].uv));

					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[27].position + Scop::Vec3f(x, y, z), BLOCK_MESH[27].normal, BLOCK_MESH[27].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[28].position + Scop::Vec3f(x, y, z), BLOCK_MESH[28].normal, BLOCK_MESH[28].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[29].position + Scop::Vec3f(x, y, z), BLOCK_MESH[29].normal, BLOCK_MESH[29].uv));
				}

				if(!GetBlock(Scop::Vec3i(x, y + 1, z)))
				{
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[30].position + Scop::Vec3f(x, y, z), BLOCK_MESH[30].normal, BLOCK_MESH[30].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[31].position + Scop::Vec3f(x, y, z), BLOCK_MESH[31].normal, BLOCK_MESH[31].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[32].position + Scop::Vec3f(x, y, z), BLOCK_MESH[32].normal, BLOCK_MESH[32].uv));

					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[33].position + Scop::Vec3f(x, y, z), BLOCK_MESH[33].normal, BLOCK_MESH[33].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[34].position + Scop::Vec3f(x, y, z), BLOCK_MESH[34].normal, BLOCK_MESH[34].uv));
					mesh_data.push_back(Scop::Vertex(BLOCK_MESH[35].position + Scop::Vec3f(x, y, z), BLOCK_MESH[35].normal, BLOCK_MESH[35].uv));
				}
			}
		}
	}

	for(std::uint32_t i = 0; i < mesh_data.size(); i++)
		indices.push_back(i);

	std::shared_ptr<Scop::Mesh> mesh = std::make_shared<Scop::Mesh>();
	mesh->AddSubMesh({ std::move(mesh_data), std::move(indices) });

	Scop::Actor& actor = m_world.GetScene().CreateActor(mesh);
	actor.GetModelRef().SetMaterial(m_world.GetBlockMaterial(), 0);
	actor.SetScale(Scop::Vec3f{ 2.0f });
	actor.SetPosition(Scop::Vec3f(m_position.x, 0.0f, m_position.y));
	p_actor = &actor;
}

std::uint32_t Chunk::GetBlock(Scop::Vec3i position) const noexcept
{
	//if(position.x < 0 || position.x >= m_data.size() || position.z < 0 || position.z >= m_data[position.x >= m_data.size() ? m_data.size() - 1 : position.x].size() || position.y < 0)
	//	return 1;
	if(position.x < m_data.size() && position.z < m_data[position.x].size() && position.y < m_data[position.x][position.z].size())
		return m_data[position.x][position.z][position.y];
	return 0;
}
