#include <NoiseCollection.h>
#include <Noise.h>
#include <memory>

Noise* NoiseCollection::GetNoise(const std::string& key) const
{
	auto it = m_collection.find(key);
	if(it != m_collection.end())
		return it->second.get();
	Scop::FatalError("A non existant Noise has been requested");
	return nullptr;
}
