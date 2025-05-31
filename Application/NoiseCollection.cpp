#include "Chunk.h"
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
	return m_collection["terrain"]->GetHeight(pos);	
}
