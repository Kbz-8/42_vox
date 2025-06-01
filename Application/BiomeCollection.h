
#ifndef BIOMECOLLECTION_H
#define BIOMECOLLECTION_H

#include <memory>
#include <unordered_map>
#include <string>
#include "Biome.h"
#include "Block.h"
#include "Chunk.h"
#include "Maths/Vec2.h"

class BiomeCollection
{
	public:
		inline BiomeCollection()
		{
			m_collection.emplace("grassland", std::make_unique<Biome>(
                        static_cast<std::uint32_t>(BlockType::Stone),
                        20,
                        std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>>{
                            {23, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Sand}}},
							{125, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Grass}}},
                            {132, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::SnowyGrass, BlockType::Grass}}},
                            {140, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::Snow, BlockType::SnowyGrass}}},
                            {255, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Snow}}}
                        }
                        ));   
			m_collection.emplace("tundra", std::make_unique<Biome>(
                        static_cast<std::uint32_t>(BlockType::Stone),
                        20,
                        std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>>{
                            {23, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Snow}}},
                            {120, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::SnowyGrass}}},
                            {140, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::Snow, BlockType::SnowyGrass}}},
                            {255, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Snow}}}
                        }
                        ));   
			m_collection.emplace("desert", std::make_unique<Biome>(
                        static_cast<std::uint32_t>(BlockType::Stone),
                        20,
                        std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>>{
                            {255, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Sand}}}
                        }
                        ));   
		};
		const std::array<std::uint32_t, CHUNK_SIZE.y> GetBiomeBlocks(std::string biome, std::uint32_t height, Scop::Vec2i pos) const;
		~BiomeCollection() = default;
	
	private:
		std::unordered_map<std::string, std::unique_ptr<Biome>> m_collection;
};

#endif
