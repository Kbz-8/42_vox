#include "Block.h"
#include "Chunk.h"
#include "Maths/Constants.h"
#include "Maths/Vec2.h"
#include <NoiseCollection.h>
#include <Noise.h>
#include <algorithm>
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

const std::array<std::uint32_t, CHUNK_SIZE.y> NoiseCollection::CarveCaves(std::array<std::uint32_t, CHUNK_SIZE.y> &data, std::uint32_t threshold, std::uint32_t cave_radius, std::uint32_t coef)
{	
	if (threshold > 90)
	{
		for(std::uint32_t y = coef; y < coef + cave_radius / 2; y++)
		{
			data[y] = static_cast<std::uint32_t>(BlockType::Air);
		}
	}
	return data;
}
const std::array<std::uint32_t, CHUNK_SIZE.y> NoiseCollection::GetBlocks(Scop::Vec2i pos)
{
	const std::uint32_t height = m_collection["terrain"]->Perlin2D(pos.x, pos.y) + ARTIFICIAL_ELEVATION;
	const std::uint32_t temperature = m_collection["temperature"]->Perlin2D(pos.x, pos.y);
	const std::uint32_t humidity = m_collection["humidity"]->Perlin2D(pos.x, pos.y);
	const std::uint32_t cave_value = m_collection["caves_depth"]->Perlin2D(pos.x, pos.y);
	const std::uint32_t cave_radius = m_collection["caves_radius"]->Perlin2D(pos.x, pos.y);
	std::array<std::uint32_t, CHUNK_SIZE.y> data;

	if (temperature < 95 && humidity <= 110)
		data = c_biomecollection.GetBiomeBlocks("tundra", height, pos);
	else if (temperature >= 95 && temperature <= 130 && humidity >= 85)
		data = c_biomecollection.GetBiomeBlocks("grassland", height, pos);
	else if (temperature >= 130)
		data = c_biomecollection.GetBiomeBlocks("desert", height, pos);
	else
		data = c_biomecollection.GetBiomeBlocks("grassland", height, pos);
	data = CarveCaves(data, cave_value, cave_radius, std::clamp(static_cast<std::uint32_t>((humidity + temperature) / 5), static_cast<std::uint32_t>(0), static_cast<std::uint32_t>(255)));
	return data;
}
