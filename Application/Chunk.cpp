#include <Chunk.h>
#include <Block.h>
#include <World.h>

#define POS_TO_INDEX(posx, posz) (posx * CHUNK_SIZE.x + posz)

constexpr Scop::Vec2ui SPRITE_SIZE = { 16, 16 };
constexpr Scop::Vec2ui ATLAS_SIZE = { 64, 64 };
constexpr Scop::Vec2f SPRITE_UNIT = Scop::Vec2f(SPRITE_SIZE) / Scop::Vec2f(ATLAS_SIZE);

constexpr std::array<std::array<Scop::Vec2ui, 3>, BlocksCount> BLOCKS_TO_ATLAS = {
	std::array<Scop::Vec2ui, 3>{ Scop::Vec2ui{ 0, 0 } }, // Air
	std::array<Scop::Vec2ui, 3>{ Scop::Vec2ui{ 0, 0 } }, // Dirt
	std::array<Scop::Vec2ui, 3>{ Scop::Vec2ui{ 1, 0 } }, // Stone
	std::array<Scop::Vec2ui, 3>{ Scop::Vec2ui{ 2, 0 }, Scop::Vec2ui{ 0, 0 }, Scop::Vec2ui{ 3, 0 } }, // Grass
};

enum class Side : std::uint8_t
{
	Top = 0,
	Bottom,
	Side
};

Scop::Vec2f GetAtlasOffset(BlockType type, Side side)
{
	Scop::Vec2ui pos = BLOCKS_TO_ATLAS[static_cast<std::uint32_t>(type)][static_cast<std::uint8_t>(side)];
	return Scop::Vec2f(Scop::Vec2f(pos.x, SPRITE_SIZE.y - pos.y - 1) * SPRITE_UNIT);
}

Chunk::Chunk(World& world, Scop::Vec2i offset) : m_offset(offset), m_position(std::move(offset) * Scop::Vec2i{ CHUNK_SIZE.x, CHUNK_SIZE.z }), m_world(world)
{
}

void Chunk::GenerateChunk()
{
	if(p_actor)
		return;
	for(auto& y: m_data)
		std::memset(y.data(), 0, y.size() * sizeof(std::uint32_t));

	for(std::uint32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::uint32_t z = 0; z < CHUNK_SIZE.z; z++)
			std::memcpy(m_data[POS_TO_INDEX(x, z)].data(), m_world.GetNoiseGenerator().GetHeight(m_position + Scop::Vec2i(x, z)).data(), CHUNK_SIZE.y * sizeof(std::uint32_t));
	}
}

void Chunk::GenerateMesh()
{
	if(!m_mesh_data.empty())
		return;

	std::size_t offset = 0;
	m_mesh_data.reserve(CHUNK_VOLUME);
	m_mesh_index_data.reserve(CHUNK_VOLUME * 4);

	for(std::int32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::int32_t z = 0; z < CHUNK_SIZE.z; z++)
		{
			for(std::int32_t y = 0; y < CHUNK_SIZE.y; y++)
			{
				BlockType type = static_cast<BlockType>(GetBlock(Scop::Vec3i(x, y, z)));
				if(type == BlockType::Air)
					continue;

				if(!GetBlock(Scop::Vec3i(x, y, z + 1)))
				{
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[0].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[0].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[0].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[1].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[1].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[1].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[2].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[2].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[2].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[3].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[3].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[3].uv)));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x, y, z - 1)))
				{
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[4].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[4].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[4].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[5].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[5].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[5].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[6].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[6].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[6].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[7].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[7].normal,  GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[7].uv)));

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

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[8].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[0].normal,  GetAtlasOffset(type, Side::Top) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[8].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[9].position  + Scop::Vec3f(x, y, z), BLOCK_MESH[9].normal,  GetAtlasOffset(type, Side::Top) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[9].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[10].position + Scop::Vec3f(x, y, z), BLOCK_MESH[10].normal, GetAtlasOffset(type, Side::Top) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[10].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[11].position + Scop::Vec3f(x, y, z), BLOCK_MESH[11].normal, GetAtlasOffset(type, Side::Top) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[11].uv)));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x, y - 1, z)))
				{
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[12].position + Scop::Vec3f(x, y, z), BLOCK_MESH[12].normal, GetAtlasOffset(type, Side::Bottom) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[12].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[13].position + Scop::Vec3f(x, y, z), BLOCK_MESH[13].normal, GetAtlasOffset(type, Side::Bottom) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[13].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[14].position + Scop::Vec3f(x, y, z), BLOCK_MESH[14].normal, GetAtlasOffset(type, Side::Bottom) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[14].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[15].position + Scop::Vec3f(x, y, z), BLOCK_MESH[15].normal, GetAtlasOffset(type, Side::Bottom) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[15].uv)));

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

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[16].position + Scop::Vec3f(x, y, z), BLOCK_MESH[16].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[16].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[17].position + Scop::Vec3f(x, y, z), BLOCK_MESH[17].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[17].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[18].position + Scop::Vec3f(x, y, z), BLOCK_MESH[18].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[18].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[19].position + Scop::Vec3f(x, y, z), BLOCK_MESH[19].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[19].uv)));

					offset += 4;
				}

				if(!GetBlock(Scop::Vec3i(x - 1, y, z)))
				{
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 2);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset);
					m_mesh_index_data.push_back(offset + 3);
					m_mesh_index_data.push_back(offset + 1);

					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[20].position + Scop::Vec3f(x, y, z), BLOCK_MESH[20].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[20].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[21].position + Scop::Vec3f(x, y, z), BLOCK_MESH[21].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[21].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[22].position + Scop::Vec3f(x, y, z), BLOCK_MESH[22].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[22].uv)));
					m_mesh_data.push_back(Scop::Vertex(BLOCK_MESH[23].position + Scop::Vec3f(x, y, z), BLOCK_MESH[23].normal, GetAtlasOffset(type, Side::Side) + (Scop::Vec2f(SPRITE_UNIT) * BLOCK_MESH[23].uv)));

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
	if(position.y < 0 || position.y >= CHUNK_SIZE.y) [[unlikely]] // No chunk under or above
		return static_cast<std::uint32_t>(BlockType::Dirt);
	if(position.x < 0) [[unlikely]]
	{
		Scop::NonOwningPtr<Chunk> neighbour = m_world.GetChunk(Scop::Vec2i{ m_offset.x - 1, m_offset.y });
		return neighbour ? neighbour->GetBlock(Scop::Vec3i(CHUNK_SIZE.x - 1, position.y, position.z)) : static_cast<std::uint32_t>(BlockType::Dirt);
	}
	if(position.x >= CHUNK_SIZE.x) [[unlikely]]
	{
		Scop::NonOwningPtr<Chunk> neighbour = m_world.GetChunk(Scop::Vec2i{ m_offset.x + 1, m_offset.y });
		return neighbour ? neighbour->GetBlock(Scop::Vec3i(0, position.y, position.z)) : static_cast<std::uint32_t>(BlockType::Dirt);
	}
	if(position.z < 0) [[unlikely]]
	{
		Scop::NonOwningPtr<Chunk> neighbour = m_world.GetChunk(Scop::Vec2i{ m_offset.x, m_offset.y - 1 });
		return neighbour ? neighbour->GetBlock(Scop::Vec3i(position.x, position.y, CHUNK_SIZE.x - 1)) : static_cast<std::uint32_t>(BlockType::Dirt);
	}
	if(position.z >= CHUNK_SIZE.z) [[unlikely]]
	{
		Scop::NonOwningPtr<Chunk> neighbour = m_world.GetChunk(Scop::Vec2i{ m_offset.x, m_offset.y + 1 });
		return neighbour ? neighbour->GetBlock(Scop::Vec3i(position.x, position.y, 0)) : static_cast<std::uint32_t>(BlockType::Dirt);
	}
	return m_data[POS_TO_INDEX(position.x, position.z)][position.y];
}
