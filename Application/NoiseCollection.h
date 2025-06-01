#ifndef NOISECOLLECTION_H
#  define NOISECOLLECTION_H

#include <BiomeCollection.h>
#include <Noise.h>
#include <cstdint>
#include <memory>
#include <unordered_map>

constexpr std::uint32_t ARTIFICIAL_ELEVATION = 40;

constexpr std::uint32_t WATER_LEVEL = 20;

class NoiseCollection
{
	public:
		inline NoiseCollection(const std::uint32_t seed): c_biomecollection()
		{
			m_collection.emplace("terrain", std::make_unique<Noise>(seed,
																	0.045f,
																	0.8f,
																	4,
																	2.0f,
																	0.7f,
																	4,
																	1.2f
																	));
			m_collection.emplace("humidity", std::make_unique<Noise>(seed,
																	0.02f,
																	0.3f,
																	1,
																	2.0f,
																	0.6f,
																	2,
																	1.0f
																	));
			m_collection.emplace("caves_depth", std::make_unique<Noise>(seed + 2,
																	0.15f,
																	1.0f,
																	2,
																	2.0f,
																	0.5f,
																	2,
																	1.0f
																	));
			m_collection.emplace("caves_radius", std::make_unique<Noise>(seed + 3,
																	0.15f,
																	1.0f,
																	4,
																	2.0f,
																	0.7f,
																	4,
																	1.0f
																	));
			m_collection.emplace("biomes", std::make_unique<Noise>(seed + 1,
																	0.05f,
																	0.1f,
																	1,
																	2.0f,
																	0.5f,
																	1,
																	1.0f
																	));
			m_collection.emplace("temperature", std::make_unique<Noise>(seed + 1,
																	0.02f,
																	0.3f,
																	2,
																	2.0f,
																	0.55f,
																	2,
																	1.05f
																	));
		}

		Noise* GetNoise(const std::string& key) const;

		inline void AddNoise(std::string key, std::unique_ptr<Noise> noise) { m_collection.emplace(std::move(key), std::move(noise)); }
		const std::array<std::uint32_t, CHUNK_SIZE.y> GetBlocks(Scop::Vec2i pos);
		const std::array<std::uint32_t, CHUNK_SIZE.y> CarveCaves(std::array<std::uint32_t, CHUNK_SIZE.y> &data, std::uint32_t threshold, std::uint32_t cave_radius, std::uint32_t coef);
		~NoiseCollection() = default;
	
	private:
		std::unordered_map<std::string, std::unique_ptr<Noise>> m_collection;
		const BiomeCollection c_biomecollection;
};

#endif
