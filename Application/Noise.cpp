#include <Noise.h>

[[nodiscard]] std::uint32_t Noise::GetHeight(Scop::Vec2i pos)
{
	return std::abs(std::sin((float)pos.x / 20.0f) * std::cos((float)pos.y / 20.0f) * 60.0f) + 1;
}
