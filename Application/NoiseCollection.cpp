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

	std::memset(data.data(), static_cast<std::uint32_t>(BlockType::Air), data.size() * sizeof(std::uint32_t));

	for(std::uint32_t y = 0; y < std::min(height, CHUNK_SIZE.y); y++)
	{
		// const std::uint32_t value = Perlin3D(pos.x, y, pos.y);
		if(y > std::min(height, CHUNK_SIZE.y) - 2)
		{
			if(height <= 23 + ARTIFICIAL_ELEVATION)
				data[y] = static_cast<std::uint32_t>(BlockType::Sand);
			else if(height < 125 + ARTIFICIAL_ELEVATION)
				data[y] = static_cast<std::uint32_t>(BlockType::Grass);
			else if (height < 132 + ARTIFICIAL_ELEVATION)
			{
				float weight = sin(2 * (pos.x * pos.y)) + sin(Scop::Pi<float>() * (pos.x * pos.y));
				if (weight > 0.0f)
					data[y] = static_cast<std::uint32_t>(BlockType::Grass);
				else
					data[y] = static_cast<std::uint32_t>(BlockType::SnowyGrass);
			}
			else if (height < 140 + ARTIFICIAL_ELEVATION)
			{
				float weight = sin(2 * (pos.x * pos.y)) + sin(Scop::Pi<float>() * (pos.x * pos.y));
				if (weight > 0.0f)
					data[y] = static_cast<std::uint32_t>(BlockType::Snow);
				else
					data[y] = static_cast<std::uint32_t>(BlockType::SnowyGrass);
			}
			else
				data[y] = static_cast<std::uint32_t>(BlockType::Snow);
		}
		else
			data[y] = static_cast<std::uint32_t>(BlockType::Stone);
	}
	for(std::uint32_t y = 0; y < ARTIFICIAL_ELEVATION + WATER_LEVEL; y++)
	{
		if(data[y] == static_cast<std::uint32_t>(BlockType::Air))
			data[y] = static_cast<std::uint32_t>(BlockType::Water);
	}
	return data;
}
