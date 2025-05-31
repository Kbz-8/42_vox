#ifndef NOISECOLLECTION_H
# define NOISECOLLECTION_H

#include <Noise.h>
#include <cstdint>
#include <memory>
#include <unordered_map>

class NoiseCollection
{
	public:
		inline NoiseCollection(const std::uint32_t seed)
		{
			m_collection.emplace("terrain", std::make_unique<Noise>(seed, // seed
																	0.045f,
																	0.8f,
																	4,
																	2.0f,
																	0.7f,
																	4,
																	1.2f
																	));
			//m_collection.emplace("caves", std::make_unique<Noise>(seed)); // TODO !!!!!!
		}

		Noise* GetNoise(const std::string& key) const;

		inline void AddNoise(std::string key, std::unique_ptr<Noise> noise) { m_collection.emplace(std::move(key), std::move(noise)); }
		[[nodiscard]] inline std::array<std::uint32_t, CHUNK_SIZE.y> GetBlocks(Scop::Vec2i pos) const { return m_collection.at("terrain")->GetHeight(pos); }
		const std::array<std::uint32_t, CHUNK_SIZE.y> GetBlocks(Scop::Vec2i pos);
		~NoiseCollection() = default;
	
	private:
		std::unordered_map<std::string, std::unique_ptr<Noise>> m_collection;
};

#endif
