#ifndef NOISE_H
#define NOISE_H

#include <ScopMaths.h>

#include <Chunk.h>
#include <array>
#include <random>

constexpr float HEIGHT_COEFF = 255.0f;
constexpr std::uint32_t NOISE_SIZE = 512;

class Noise
{
	public:
		Noise(const std::uint32_t seed, float frequency, float amplitude, int octaves, float lacunarity, float persistance, int redistribution, float compensatory_factor);

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
