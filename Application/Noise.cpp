#include <Noise.h>
#include <Block.h>

[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> Noise::GetHeight(Scop::Vec2i pos)
{
	std::array<std::uint32_t, CHUNK_SIZE.y> data;
	std::memset(data.data(), static_cast<std::uint32_t>(BlockType::Air), data.size() * sizeof(std::uint32_t));

	std::uint32_t height = std::abs(std::sin((float)pos.x / 20.0f) * std::cos((float)pos.y / 20.0f) * 60.0f) + 1;

	// Must not exceed CHUNK_SIZE.y
	for(std::uint32_t y = 0; y < std::min(height, CHUNK_SIZE.y); y++)
	{
		if(y > std::min(height, CHUNK_SIZE.y) - 2)
			data[y] = static_cast<std::uint32_t>(BlockType::Grass);
		else
			data[y] = static_cast<std::uint32_t>(BlockType::Stone);
	}

	return data;
}
