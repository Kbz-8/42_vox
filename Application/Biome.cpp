
#include <Biome.h>
#include "Block.h"
#include "Chunk.h"
#include "Maths/Vec3.h"
#include <utility>

Biome::Biome(std::uint32_t filler, std::uint32_t water_level, std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>> blocks): filler(filler), water_level(water_level), c_blockmap(blocks)
{
	for (const auto& [height, BlockPlacement] : blocks)
	{
		const auto& [PlacementType, Blocks] = BlockPlacement;
		if (PlacementType == BlockPlacementType::PseudoRandom && Blocks.size() < 2)
			Scop::FatalError("A biome have a Multiple block placement but less than 2 blocks was given");
	}
}

const std::array<std::uint32_t, CHUNK_SIZE.y> Biome::GetBiomeBlocks(const std::uint32_t height, Scop::Vec2i pos)	
{
	std::array<std::uint32_t, CHUNK_SIZE.y> data;

	auto it = c_blockmap.lower_bound(height);

	if (it == c_blockmap.end())
		Scop::FatalError("Biome declaration does not have a value for a certain generated height");
	for(std::uint32_t y = 0; y < std::min(height, CHUNK_SIZE.y); y++)
	{
		if(y > std::min(height, CHUNK_SIZE.y) - 2)
		{
			data[y] = static_cast<std::uint32_t>(BlockType::Dirt);
			continue;
			const auto& [placementType, blockTypes] = it->second;
			switch (static_cast<std::uint8_t>(placementType))
			{
				case(static_cast<std::uint8_t>(BlockPlacementType::Simple)):
				{
					data[y] = static_cast<std::uint32_t>(blockTypes.front());
					break;
				}
				case(static_cast<std::uint8_t>(BlockPlacementType::PseudoRandom)):
				{
					float weight = sin(2 * (pos.x * pos.y)) + sin(Scop::Pi<float>() * (pos.x * pos.y));
					if (weight > 0.0f)
						data[y] = static_cast<std::uint32_t>(blockTypes.at(0));
					else
						data[y] = static_cast<std::uint32_t>(blockTypes.at(1));
					break;
				}
				default:
					data[y] = static_cast<std::uint32_t>(BlockType::Stone);
			}
		}
		else
		{
			data[y] = filler;
		}
	}
	for(std::uint32_t y = 0; y < water_level; y++)
	{
		if(data[y] == static_cast<std::uint32_t>(BlockType::Air))
			data[y] = static_cast<std::uint32_t>(BlockType::Water);
	}
	return data;
}
