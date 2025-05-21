#ifndef NOISE_H
#define NOISE_H

#include <ScopMaths.h>

#include <Chunk.h>

class Noise
{
	public:
		Noise() = default;

		[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> GetHeight(Scop::Vec2i pos);

		~Noise() = default;

	private:
};

#endif
