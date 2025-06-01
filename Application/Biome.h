#ifndef BIOME_H
#define BIOME_H

#include "Block.h"
#include "Chunk.h"
#include "Maths/Vec2.h"
#include <map>
#include <utility>
#include <vector>

enum class BlockPlacementType : std::uint8_t
{
    Simple = 0,
    PseudoRandom,
};

class Biome 
{

	public:
		Biome(std::uint32_t filler, std::uint32_t water_level, std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>> blocks);
		~Biome() = default;
		const std::array<std::uint32_t, CHUNK_SIZE.y> GetBiomeBlocks(const std::uint32_t height, Scop::Vec2i pos);	
	private:
		const std::uint32_t filler;
		const std::uint32_t water_level;
		const std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>> c_blockmap;
};

#endif
