#ifndef NOISE_H
#define NOISE_H

#include <ScopMaths.h>

#include <Chunk.h>
#include <array>
#include <random>

constexpr float HEIGHT_COEFF = 255.0f;
constexpr std::uint32_t NOISE_SIZE = 512;
constexpr std::uint32_t WATER_LEVEL = 20;

class Noise
{
	public:
		Noise(const std::uint32_t seed = 42, float frequency = 0.045f, float amplitude = 0.80f, int octaves = 4, float lacunarity = 2.0f, float persistance = 0.7f, int redistribution = 4, float compensatory_factor = 1.2f);

		[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> GetHeight(Scop::Vec2i pos);
		[[nodiscard]] const int Perlin2D(int x, int y) noexcept;
		[[nodiscard]] const int Perlin3D(int x, int y, int z) noexcept;

		~Noise() = default;
	
	private:
		void InitPermutation(void);

		[[nodiscard]] const float Perlin2D(float x, float y) noexcept;
		[[nodiscard]] const int ApplyPerlin2DParameters(float x, float y) noexcept;

		[[nodiscard]] const float Perlin3D(float x, float y, float z) noexcept;
		[[nodiscard]] const int ApplyPerlin3DParameters(float x, float y, float z) noexcept;

		[[nodiscard]] const float Fade(float t) noexcept;
		[[nodiscard]] const float Grad2D(int hash, float x, float y) noexcept;
		[[nodiscard]] const float Grad(int hash, float x, float y, float z) noexcept;

	private:
		std::mt19937 m_seed;
		std::array<int, NOISE_SIZE> m_perms;
		const float c_frequency;
		const float c_amplitude;
		const int c_octaves;
		const float c_lacunarity;
		const float c_persistance;
		const int c_redistribution;
		const float c_compensatory_factor;
};

#endif
