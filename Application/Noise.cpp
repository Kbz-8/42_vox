#include <Noise.h>
#include <Block.h>
#include <cstdint>
#include <random>

Noise::Noise(const std::uint32_t seed, float frequency, float amplitude, int octaves, float lacunarity, float persistance, int redistribution, float compensatory_factor): m_seed(std::mt19937(seed)), c_frequency(frequency), c_amplitude(amplitude), c_octaves(octaves), c_lacunarity(lacunarity), c_persistance(persistance), c_redistribution(redistribution), c_compensatory_factor(compensatory_factor)
{
	if(c_amplitude > 1.0f || c_amplitude < -1.0f)
		Scop::FatalError("Amplitude value must be in [-1;1]");
	if(c_redistribution <= 0)
		Scop::FatalError("Redistribution cannot be a negative integer");
	InitPermutation();
}

void Noise::InitPermutation()
{
	std::array<int, 256> permutations;

	for(int i = 0; i < 256; ++i)
		permutations[i] = i;
	std::shuffle(permutations.begin(), permutations.end(), m_seed);
	for(int i = 0; i < 256; ++i) 
	{
		m_perms[i] = permutations[i];
		m_perms[i + 256] = permutations[i];
	}
}

[[nodiscard]] const float Noise::Fade(float t) noexcept
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

const float Noise::Perlin2D(float x, float y) noexcept 
{
	int xi = static_cast<std::int32_t>(std::floor(x)) & 255;
	int yi = static_cast<std::int32_t>(std::floor(y)) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	float u = Fade(xf);
	float v = Fade(yf);

	int aa = m_perms[m_perms[xi] + yi];
	int ab = m_perms[m_perms[xi] + yi + 1];
	int ba = m_perms[m_perms[xi + 1] + yi];
	int bb = m_perms[m_perms[xi + 1] + yi + 1];

	float x1 = std::lerp(Grad2D(aa, xf,     yf),
					Grad2D(ba, xf - 1, yf), u);
	float x2 = std::lerp(Grad2D(ab, xf,     yf - 1),
					Grad2D(bb, xf - 1, yf - 1), u);

	return ((std::lerp(x1, x2, v) + 1.0f) / 2.0f);
}

const int Noise::ApplyPerlin2DParameters(float x, float y) noexcept // Wrapper to apply various mumbo jumbo to get a very worldlike generation
{
	float total = 0.0f;
	float tmp_freq = c_frequency;
	float tmp_amp = c_amplitude;
	float maxValue = 0.0f;

	 for(int i = 0; i < this->c_octaves; ++i)
	 {
		total += Perlin2D(x * tmp_freq, y * tmp_freq) * tmp_amp;
		maxValue += tmp_amp;
		tmp_amp *= c_persistance;
		tmp_freq *= c_lacunarity;
	}
	float normalized = total / maxValue;
	normalized = std::clamp(normalized, 0.0f, 1.0f);
	normalized = std::pow(normalized * c_compensatory_factor, c_redistribution);
    return static_cast<int>(normalized * HEIGHT_COEFF);
}

[[nodiscard]] const int Noise::Perlin2D(int x, int y) noexcept
{ // Wrapper to unnormalise input and output
	float scaledX = static_cast<float>(x) * c_frequency;
	float scaledY = static_cast<float>(y) * c_frequency;
	return std::floor(ApplyPerlin2DParameters(scaledX, scaledY));
}

[[nodiscard]] const float Noise::Grad2D(int hash, float x, float y) noexcept
{
	int h = hash & 7; // 8 directions
	float u = h < 4 ? x : y;
	float v = h < 4 ? y : x;

	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

[[nodiscard]] const float Noise::Grad(int hash, float x, float y, float z) noexcept 
{
	int h = hash & 15;  // 16 directions possibles
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}


[[nodiscard]] const int Noise::Perlin3D(int x, int y, int z) noexcept
{
	float scaledX = static_cast<float>(x) * c_frequency;
    float scaledY = static_cast<float>(y) * c_frequency;
	float scaledZ = static_cast<float>(z) * c_frequency;
    return static_cast<int>(ApplyPerlin3DParameters(scaledX, scaledY, scaledZ));
}

[[nodiscard]] const float Noise::Perlin3D(float x, float y, float z) noexcept 
{
	int xi = static_cast<int>(x) & 255;
	int yi = static_cast<int>(y) & 255;
	int zi = static_cast<int>(z) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);
	float zf = z - std::floor(z);

	float u = Fade(xf);
	float v = Fade(yf);
	float w = Fade(zf);

	int aaa = m_perms[m_perms[m_perms[xi] + yi] + zi];
	int aba = m_perms[m_perms[m_perms[xi] + yi + 1] + zi];
	int aab = m_perms[m_perms[m_perms[xi] + yi] + zi + 1];
	int abb = m_perms[m_perms[m_perms[xi] + yi + 1] + zi + 1];
	int baa = m_perms[m_perms[m_perms[xi + 1] + yi] + zi];
	int bba = m_perms[m_perms[m_perms[xi + 1] + yi + 1] + zi];
	int bab = m_perms[m_perms[m_perms[xi + 1] + yi] + zi + 1];
	int bbb = m_perms[m_perms[m_perms[xi + 1] + yi + 1] + zi + 1];

	float x1, x2, y1, y2;
	x1 = std::lerp(Grad(aaa, xf, yf, zf),
			  Grad(baa, xf - 1, yf, zf), u);
	x2 = std::lerp(Grad(aba, xf, yf - 1, zf),
			  Grad(bba, xf - 1, yf - 1, zf), u);
	y1 = std::lerp(x1, x2, v);

	x1 = std::lerp(Grad(aab, xf, yf, zf - 1),
			  Grad(bab, xf - 1, yf, zf - 1), u);
	x2 = std::lerp(Grad(abb, xf, yf - 1, zf - 1),
			  Grad(bbb, xf - 1, yf - 1, zf - 1), u);
	y2 = std::lerp(x1, x2, v);

	return ((std::lerp(y1, y2, w) + 1.0f) / 2.0f) * c_amplitude;
}

[[nodiscard]] const int Noise::ApplyPerlin3DParameters(float x, float y, float z) noexcept
{
	float total = 0.0f;
	float tmp_freq = c_frequency;
	float tmp_amp = c_amplitude;
	float maxValue = 0.0f;

	for(int i = 0; i < this->c_octaves; ++i)
	{
		total += Perlin3D(x * tmp_freq, y * tmp_freq, z * tmp_freq) * tmp_amp;
		maxValue += tmp_amp;
		tmp_amp *= c_persistance;
		tmp_freq *= c_lacunarity;
	}
	float normalized = total / maxValue;
	normalized = std::clamp(normalized, 0.0f, 1.0f);
	return static_cast<int>(normalized * HEIGHT_COEFF);
}

