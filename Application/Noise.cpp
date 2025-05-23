#include <Noise.h>
#include <Block.h>
#include <cstdint>
#include <random>
#include <stdexcept>

Noise::Noise(const std::uint32_t seed, float frequency, float amplitude, int octaves, float lacunarity, float persistance): seed(std::mt19937(seed)), frequency(frequency), amplitude(amplitude), octaves(octaves), lacunarity(lacunarity), persistance(persistance)
{
	if (amplitude > 1.0f || amplitude < -1.0f)
	{
		throw std::invalid_argument("Amplitude value must be in [-1;1]");
	}
	InitPermutation();
}

void Noise::InitPermutation(void)
{
	std::array<int, 256> permutations;

	for (int i = 0; i < 256; ++i)
		permutations[i] = i;
	std::shuffle(permutations.begin(), permutations.end(), seed);
	for (int i = 0; i < 256; ++i) 
	{
		this->perms[i] = permutations[i];
		this->perms[i + 256] = permutations[i];
	}
	for (int val: this->perms)
	{
		std::cout << val << " ";
	}
	std::cout << std::endl;
	std::cout << perlin3D2(0, 0, 0) << std::endl;
	std::cout << perlin3D2(20, 20, 20) << std::endl;
	std::cout << perlin3D(150, 150, 150) << std::endl;
	std::cout << perlin3D(0.078f, 0.4f, 0.2f) << std::endl;

}

[[nodiscard]] const float Noise::fade(float t)
{

	return t * t * t * (t * (t * 6 - 15) + 10);
}

[[nodiscard]] const float Noise::lerp(float a, float b, float t)
{
	return (a + t * (b - a));
}

const float Noise::perlin2D(float x, float y) {
    int xi = (int)floor(x) & 255;
    int yi = (int)floor(y) & 255;

    float xf = x - floor(x);
    float yf = y - floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = perms[perms[xi] + yi];
    int ab = perms[perms[xi] + yi + 1];
    int ba = perms[perms[xi + 1] + yi];
    int bb = perms[perms[xi + 1] + yi + 1];

    float x1 = lerp(grad2D(aa, xf,     yf),
                    grad2D(ba, xf - 1, yf), u);
    float x2 = lerp(grad2D(ab, xf,     yf - 1),
                    grad2D(bb, xf - 1, yf - 1), u);

    return ((lerp(x1, x2, v) + 1.0f) / 2.0f);
}

const int Noise::perlin2D2(float x, float y) // Wrapper to apply various mumbo jumbo to get a very worldlike generation
{
	float total = 0.0f;
	float tmp_freq = frequency;
	float tmp_amp = amplitude;
	float maxValue = 0.0f;

	 for (int i = 0; i < this->octaves; ++i) {
        total += perlin2D(x * tmp_freq, y * tmp_freq) * tmp_amp;
        maxValue += tmp_amp;
		tmp_amp *= persistance;
        tmp_freq *= lacunarity;
    }
	float normalized = total / maxValue;
	normalized = std::clamp(normalized, 0.0f, 1.0f);
    return static_cast<int>(normalized * 255.0f);
}

const int Noise::perlin2D(int x, int y) { // Wrapper to unnormalise input and output
    float scaledX = static_cast<float>(x) * frequency;
    float scaledY = static_cast<float>(y) * frequency;
    return floor(perlin2D2(scaledX, scaledY));
}



[[nodiscard]] const float Noise::grad2D(int hash, float x, float y)
{
    int h = hash & 7; // 8 directions
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

[[nodiscard]] const float Noise::grad(int hash, float x, float y, float z) {
    int h = hash & 15;  // 16 directions possibles
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}


const int Noise::perlin3D(int x, int y, int z)
{
	float scaledX = static_cast<float>(x) * frequency;
    float scaledY = static_cast<float>(y) * frequency;
	float scaledZ = static_cast<float>(z) * frequency;
    return floor(perlin3D2(scaledX, scaledY, scaledZ));
}

const float Noise::perlin3D(float x, float y, float z) {
    int xi = (int)floor(x) & 255;
    int yi = (int)floor(y) & 255;
    int zi = (int)floor(z) & 255;

    float xf = x - floor(x);
    float yf = y - floor(y);
    float zf = z - floor(z);

    float u = fade(xf);
    float v = fade(yf);
    float w = fade(zf);

    int aaa = perms[perms[perms[xi] + yi] + zi];
    int aba = perms[perms[perms[xi] + yi + 1] + zi];
    int aab = perms[perms[perms[xi] + yi] + zi + 1];
    int abb = perms[perms[perms[xi] + yi + 1] + zi + 1];
    int baa = perms[perms[perms[xi + 1] + yi] + zi];
    int bba = perms[perms[perms[xi + 1] + yi + 1] + zi];
    int bab = perms[perms[perms[xi + 1] + yi] + zi + 1];
    int bbb = perms[perms[perms[xi + 1] + yi + 1] + zi + 1];

    float x1, x2, y1, y2;
    x1 = lerp(grad(aaa, xf, yf, zf),
              grad(baa, xf - 1, yf, zf), u);
    x2 = lerp(grad(aba, xf, yf - 1, zf),
              grad(bba, xf - 1, yf - 1, zf), u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, xf, yf, zf - 1),
              grad(bab, xf - 1, yf, zf - 1), u);
    x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
              grad(bbb, xf - 1, yf - 1, zf - 1), u);
    y2 = lerp(x1, x2, v);

    return ((lerp(y1, y2, w) + 1.0f) / 2.0f) * amplitude;
}

const int Noise::perlin3D2(float x, float y, float z)
{
	float total = 0.0f;
	float tmp_freq = frequency;
	float tmp_amp = amplitude;
	float maxValue = 0.0f;

	 for (int i = 0; i < this->octaves; ++i) {
        total += perlin3D(x * tmp_freq, y * tmp_freq, z * tmp_freq) * tmp_amp;
        maxValue += tmp_amp;
		tmp_amp *= persistance;
        tmp_freq *= lacunarity;
    }
	float normalized = total / maxValue;
	normalized = std::clamp(normalized, 0.0f, 1.0f);
    return static_cast<int>(normalized * 255.0f);
}


[[nodiscard]] std::array<std::uint32_t, CHUNK_SIZE.y> Noise::GetHeight(Scop::Vec2i pos)
{
	std::array<std::uint32_t, CHUNK_SIZE.y> data;
	std::memset(data.data(), static_cast<std::uint32_t>(BlockType::Air), data.size() * sizeof(std::uint32_t));

	
	//std::uint32_t height = std::abs(std::sin((float)pos.x / 20.0f) * std::cos((float)pos.y / 20.0f) * 60.0f) + 1;
	
	std::uint32_t height = perlin2D(pos.x, pos.y); 
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
