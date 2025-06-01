
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
                        static_cast<std::uint32_t>(BlockType::Stone), // underground block
                        20, // water level
						static_cast<std::uint32_t>(BlockType::Water), // Block to place at water level
						static_cast<std::uint32_t>(BlockType::Dirt), // intermediary block
                        std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>>{
                            {17, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Dirt}}}, // place a dirt block at surface is height <= 17
                            {23, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Sand}}}, // place a sand block at surface if height > 17 && <= 23
							{125, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Grass}}},
                            {132, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::SnowyGrass, BlockType::Grass}}}, // Place randomly one of the two blocks at the surface
                            {140, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::Snow, BlockType::SnowyGrass}}},
                            {255, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Snow}}} // ALWAYS put a block to 255
                        }
                        ));   
			m_collection.emplace("tundra", std::make_unique<Biome>(
                        static_cast<std::uint32_t>(BlockType::Stone),
                        20,
						static_cast<std::uint32_t>(BlockType::Water),
						static_cast<std::uint32_t>(BlockType::Stone),
                        std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>>{
                            {17, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Dirt}}},
                            {20, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Ice}}},
                            {23, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Snow}}},
                            {120, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::SnowyGrass}}},
                            {140, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::Snow, BlockType::SnowyGrass}}},
                            {255, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Snow}}}
                        }
                        ));   
			m_collection.emplace("desert", std::make_unique<Biome>(
                        static_cast<std::uint32_t>(BlockType::Stone),
                        20,
						static_cast<std::uint32_t>(BlockType::Water),
						static_cast<std::uint32_t>(BlockType::SandStone),
                        std::map<std::uint32_t, std::pair<BlockPlacementType, std::vector<BlockType>>>{
                            {120, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::Simple, std::vector<BlockType>{BlockType::Sand}}},
                            {140, std::pair<BlockPlacementType, std::vector<BlockType>>{BlockPlacementType::PseudoRandom, std::vector<BlockType>{BlockType::Sand, BlockType::SandStone}}},
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
