#include "Block.h"
#include "Chunk.h"
#include "Maths/Constants.h"
#include "Maths/Vec2.h"
#include <NoiseCollection.h>
#include <Noise.h>
#include <cstdint>
#include <memory>

Noise* NoiseCollection::GetNoise(const std::string& key) const
{
	auto it = m_collection.find(key);
	if(it != m_collection.end())
		return it->second.get();
	Scop::FatalError("A non existant Noise has been requested");
	return nullptr;
}

const std::array<std::uint32_t, CHUNK_SIZE.y> NoiseCollection::GetBlocks(Scop::Vec2i pos)
{
	const std::uint32_t height = m_collection["terrain"]->Perlin2D(pos.x, pos.y) + ARTIFICIAL_ELEVATION;
	std::array<std::uint32_t, CHUNK_SIZE.y> data;
	const std::uint32_t biome_value = m_collection["biomes"]->Perlin2D(pos.x, pos.y);
	const std::uint32_t temperature = m_collection["temperature"]->Perlin2D(pos.x, pos.y);
	const std::uint32_t humidity = m_collection["humidity"]->Perlin2D(pos.x, pos.y);
	if (temperature < 85 && humidity <= 110)
		return c_biomecollection.GetBiomeBlocks("tundra", height, pos);
	if (temperature >= 85 && temperature <= 145 && humidity >= 85)
		return c_biomecollection.GetBiomeBlocks("grassland", height, pos);
	if (temperature >= 145)
		return c_biomecollection.GetBiomeBlocks("desert", height, pos);
	return c_biomecollection.GetBiomeBlocks("grassland", height, pos);
}
