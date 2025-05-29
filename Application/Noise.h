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
		[[nodiscard]] const int Perlin2D(int x, int y) noexcept;
		[[nodiscard]] const int Perlin3D(int x, int y, int z) noexcept;
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

		[[nodiscard]] const float Perlin2D(float x, float y) noexcept;
		[[nodiscard]] const int ApplyPerlin2DParameters(float x, float y) noexcept;

		[[nodiscard]] const float Perlin3D(float x, float y, float z) noexcept;
		[[nodiscard]] const int ApplyPerlin3DParameters(float x, float y, float z) noexcept;

		[[nodiscard]] const float fade(float t) noexcept;
		[[nodiscard]] const float lerp(float a, float b, float t) noexcept;
		[[nodiscard]] const float grad2D(int hash, float x, float y) noexcept;
		[[nodiscard]] const float grad(int hash, float x, float y, float z) noexcept;
};

#endif
