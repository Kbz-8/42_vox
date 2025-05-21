#include <Chunk.h>
#include <Block.h>
#include <World.h>

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
	if(p_actor)
		return;
	for(auto& z: m_data)
	{
		for(auto& y: z)
			std::fill(y.begin(), y.end(), 0);
	}

	for(std::uint32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::uint32_t z = 0; z < CHUNK_SIZE.z; z++)
		{
			// Implement noise here
			std::uint32_t height = std::abs(std::sin((float)m_offset.x / 20.0f) * std::cos((float)m_offset.y / 20.0f) * 60.0f) + 1;
			for(std::uint32_t y = 0; y < height; y++)
				m_data[x][z][y] = 1;
		}
	}
}

void Chunk::GenerateMesh()
{
	if(!m_mesh_data.empty())
		return;

	std::size_t offset = 0;

	for(std::uint32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::uint32_t z = 0; z < CHUNK_SIZE.z; z++)
		{
			for(std::uint32_t y = 0; y < CHUNK_SIZE.y; y++)
			{
				if(!GetBlock(Scop::Vec3i(x, y, z)))
					continue;

				if(!GetBlock(Scop::Vec3i(x, y, z + 1)))
				{
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[0].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[0].normal,  BLOCK_MESH[0].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[1].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[1].normal,  BLOCK_MESH[1].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[2].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[2].normal,  BLOCK_MESH[2].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[3].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[3].normal,  BLOCK_MESH[3].uv));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x, y, static_cast<std::int32_t>(z) - 1)))
				{
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[4].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[4].normal,  BLOCK_MESH[4].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[5].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[5].normal,  BLOCK_MESH[5].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[6].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[6].normal,  BLOCK_MESH[6].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[7].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[7].normal,  BLOCK_MESH[7].uv));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x, y + 1, z)))
				{
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[8].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[0].normal,  BLOCK_MESH[8].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[9].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[9].normal,  BLOCK_MESH[9].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[10].position + Scop::Vec3f(x, y, z), BLOCK_MESH[10].normal, BLOCK_MESH[10].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[11].position + Scop::Vec3f(x, y, z), BLOCK_MESH[11].normal, BLOCK_MESH[11].uv));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x, static_cast<std::int32_t>(y) - 1, z)))
				{
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[12].position + Scop::Vec3f(x, y, z), BLOCK_MESH[12].normal, BLOCK_MESH[12].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[13].position + Scop::Vec3f(x, y, z), BLOCK_MESH[13].normal, BLOCK_MESH[13].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[14].position + Scop::Vec3f(x, y, z), BLOCK_MESH[14].normal, BLOCK_MESH[14].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[15].position + Scop::Vec3f(x, y, z), BLOCK_MESH[15].normal, BLOCK_MESH[15].uv));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x + 1, y, z)))
				{
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[16].position + Scop::Vec3f(x, y, z), BLOCK_MESH[16].normal, BLOCK_MESH[16].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[17].position + Scop::Vec3f(x, y, z), BLOCK_MESH[17].normal, BLOCK_MESH[17].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[18].position + Scop::Vec3f(x, y, z), BLOCK_MESH[18].normal, BLOCK_MESH[18].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[19].position + Scop::Vec3f(x, y, z), BLOCK_MESH[19].normal, BLOCK_MESH[19].uv));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(static_cast<std::int32_t>(x) - 1, y, z)))
				{
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[20].position + Scop::Vec3f(x, y, z), BLOCK_MESH[20].normal, BLOCK_MESH[20].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[21].position + Scop::Vec3f(x, y, z), BLOCK_MESH[21].normal, BLOCK_MESH[21].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[22].position + Scop::Vec3f(x, y, z), BLOCK_MESH[22].normal, BLOCK_MESH[22].uv));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[23].position + Scop::Vec3f(x, y, z), BLOCK_MESH[23].normal, BLOCK_MESH[23].uv));

					offset += 4;
				}
			}
		}
	}
}

void Chunk::UploadMesh()
{
	if(p_actor || m_mesh_data.empty() || m_mesh_index_data.empty())
		return;
	std::shared_ptr<Scop::Mesh> mesh = std::make_shared<Scop::Mesh>();
	mesh->AddSubMesh({ std::move(m_mesh_data), std::move(m_mesh_index_data) });

	Scop::Actor& actor = m_world.GetScene().CreateActor(mesh);
	actor.GetModelRef().SetMaterial(m_world.GetBlockMaterial(), 0);
	actor.SetScale(Scop::Vec3f{ 2.0f });
	actor.SetPosition(Scop::Vec3f(m_position.x, 0.0f, m_position.y));
	p_actor = &actor;
}

std::uint32_t Chunk::GetBlock(Scop::Vec3i position) const noexcept
{
	if(position.x < 0 || position.x >= m_data.size() || position.z < 0 || position.z >= m_data[position.x >= m_data.size() ? m_data.size() - 1 : position.x].size() || position.y < 0)
		return 1;
	if(position.x < m_data.size() && position.z < m_data[position.x].size() && position.y < m_data[position.x][position.z].size())
		return m_data[position.x][position.z][position.y];
	return 0;
}
