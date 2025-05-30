#include <NoiseCollection.h>
#include <Noise.h>
#include <memory>

NoiseCollection::NoiseCollection(const std::uint32_t seed)
{
	m_collection["terrain"] = std::make_unique<Noise>(seed);
	m_collection["caves"] = std::make_unique<Noise>(seed); // TODO !!!!!!
}

void NoiseCollection::AddNoise(const std::string& key, std::unique_ptr<Noise> noise)
{
	m_collection[key] = std::move(noise);
}

Noise* NoiseCollection::GetNoise(const std::string key) const
{
	auto it = m_collection.find(key);
	if (it != m_collection.end())
		return it->second.get();
	Scop::FatalError("A non existant Noise has been requested");
	return nullptr;
}

[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> NoiseCollection::GetBlocks(Scop::Vec2i pos)
{
	return m_collection["terrain"]->GetHeight(pos);
}
