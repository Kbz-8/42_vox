#ifndef NOISECOLLECTION_H
# define NOISECOLLECTION_H

#include <Noise.h>
#include <cstdint>
#include <memory>
#include <unordered_map>

class NoiseCollection
{
	public:
		NoiseCollection(const std::uint32_t seed);
		~NoiseCollection(void) = default;
		void AddNoise(const std::string& key, std::unique_ptr<Noise> noise);
		Noise* GetNoise(const std::string key) const;
		[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> GetBlocks(Scop::Vec2i pos);
	
	private:
		std::unordered_map<std::string, std::unique_ptr<Noise>> m_collection;
};

#endif
