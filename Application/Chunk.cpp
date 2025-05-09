#include <Chunk.h>
#include <algorithm>

Chunk::Chunk(Scop::Scene& world, Scop::Vec2ui offset) : m_data(CHUNK_VOLUME), m_offset(offset), m_position(std::move(offset) * Scop::Vec2ui{ CHUNK_SIZE })
{
}

void Chunk::GenerateChunk()
{
	std::fill(m_data.begin(), m_data.end(), 0);

	for(std::uint32_t x = 0; x < CHUNK_SIZE.x; x++)
	{
		for(std::uint32_t z = 0; z < CHUNK_SIZE.z; z++)
		{
			std::uint32_t pos_x = m_position.x + x;
			std::uint32_t pos_z = m_position.y + z;

			for(std::uint32_t y = 0; y < CHUNK_SIZE.y; y++)
			{
				std::uint32_t index = (z * CHUNK_SIZE.x * CHUNK_SIZE.y) + (y * CHUNK_SIZE.x) + x;

				// Implement noise here

				m_data[index] = y < 10 ? 1 : 0;
			}
		}
	}
}
