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
		Noise(const std::uint32_t seed = 42, float frequency = 0.05f, float amplitude = 0.9f, int octaves = 4, float lacunarity = 2.0f, float persistance = 0.5f);

		[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> GetHeight(Scop::Vec2i pos);
		const float perlin2D(float x, float y);
		const int perlin2D(int x, int y);
		const int perlin2D2(float x, float y);
		const int perlin3D(int x, int y, int z);
		const float perlin3D(float x, float y, float z);
		const int perlin3D2(float x, float y, float z);
		~Noise() = default;

	private:
		std::mt19937 seed;
		std::array<int, NOISE_SIZE> perms;
		const float frequency;
		const float amplitude;
		const int octaves;
		const float lacunarity;
		const float persistance;
		void InitPermutation(void);
		[[nodiscard]] const float fade(float t);
		[[nodiscard]] const float lerp(float a, float b, float t);
		[[nodiscard]] const float grad2D(int hash, float x, float y);
		[[nodiscard]] const float grad(int hash, float x, float y, float z);
};

#endif
