#include <BiomeCollection.h>

const std::array<std::uint32_t, CHUNK_SIZE.y> BiomeCollection::GetBiomeBlocks(std::string biome, std::uint32_t height, Scop::Vec2i pos) const
{
	return m_collection.at(biome)->GetBiomeBlocks(height, pos);
}
