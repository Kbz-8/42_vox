#include <Noise.h>

[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> Noise::GetHeight(Scop::Vec2i pos)
{
	std::array<std::uint32_t, CHUNK_SIZE.y> data;
	std::memset(data.data(), 0, data.size() * sizeof(std::uint32_t));

	std::uint32_t height = std::abs(std::sin((float)pos.x / 20.0f) * std::cos((float)pos.y / 20.0f) * 60.0f) + 1;
	for(std::uint32_t y = 0; y < std::min(height, CHUNK_SIZE.y); y++)
		data[y] = 1;

	return data;
}
