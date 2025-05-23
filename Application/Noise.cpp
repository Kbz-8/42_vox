#include <Noise.h>
#include <Block.h>
#include <cstdint>
#include <random>

Noise::Noise(const std::uint32_t seed): seed(std::mt19937(seed))
{
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
	std::cout << perlin2D(0, 0) << std::endl;
	std::cout << perlin2D(20, 20) << std::endl;
	std::cout << perlin2D(150, 150) << std::endl;
	std::cout << perlin2D(0.078f, 0.4f) << std::endl;

}

float Noise::fade(float t)
{

	return t * t * t * (t * (t * 6 - 15) + 10);
}

float Noise::lerp(float a, float b, float t)
{
	return (a + t * (b - a));
}

float Noise::grad(int hash, float x, float y) 
{
    int h = hash & 7; 
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

int Noise::perlin2D(int x, int y) {
    float scaledX = static_cast<float>(x) * 0.01f;
    float scaledY = static_cast<float>(y) * 0.01f;
    return floor(perlin2D(scaledX, scaledY) * 255.0f);
}

float Noise::perlin2D(float x, float y) {
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

    float x1 = lerp(grad(aa, xf,     yf),
                    grad(ba, xf - 1, yf), u);
    float x2 = lerp(grad(ab, xf,     yf - 1),
                    grad(bb, xf - 1, yf - 1), u);

    return (lerp(x1, x2, v) + 1.0f) / 2.0f; // Normalize to [0, 1]
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
