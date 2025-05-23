#ifndef NOISE_H
#define NOISE_H

#include <ScopMaths.h>

#include <Chunk.h>
#include <array>
#include <random>

#define NOISE_SIZE 512
class Noise
{
	public:
		Noise(const std::uint32_t seed = 42);

		[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> GetHeight(Scop::Vec2i pos);
		float perlin2D(float x, float y);
		int perlin2D(int x, int y);
		~Noise() = default;

	private:
		std::mt19937 seed;
		std::array<int, NOISE_SIZE> perms;
		void InitPermutation(void);
		float fade(float t);
		float lerp(float a, float b, float t);
		float grad(int hash, float x, float y);
};

#endif
