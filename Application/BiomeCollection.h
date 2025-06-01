
#ifndef BIOMECOLLECTION_H
#define BIOMECOLLECTION_H

#include <memory>
#include <unordered_map>
#include <string>
#include "Biome.h"
#include "Chunk.h"
#include "Maths/Vec2.h"

class BiomeCollection
{
	public:
		inline BiomeCollection()
		{
			m_collection.emplace("grassland", std::make_unique<Biome>(Biome(
									static_cast<std::uint32_t>(BlockType::Stone),
									0,
									{
            							{20, {BlockPlacementType::Simple, {BlockType::Sand}}},
										{255, {BlockPlacementType::Simple, {BlockType::Snow}}}
        							})
									));	
		};
		const std::array<std::uint32_t, CHUNK_SIZE.y> GetBiomeBlocks(std::string biome, std::uint32_t height, Scop::Vec2i pos) const;
		~BiomeCollection() = default;
	
	private:
		std::unordered_map<std::string, std::unique_ptr<Biome>> m_collection;
};

#endif
