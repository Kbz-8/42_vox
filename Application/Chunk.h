#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <ScopGraphics.h>
#include <ScopMaths.h>

constexpr Scop::Vec3ui CHUNK_SIZE = Scop::Vec3ui{ 16, 256, 16 };
constexpr std::uint32_t CHUNK_VOLUME = CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z;

class Chunk
{
	public:
		Chunk(Scop::Scene& world, Scop::Vec2ui offset);

		void GenerateChunk();

		~Chunk() = default;

	private:
		std::vector<std::uint32_t> m_data;
		Scop::Vec2ui m_offset; // In chunks
		Scop::Vec2ui m_position; // In blocks
		Scop::NonOwningPtr<Scop::Actor> p_actor = nullptr;
};

#endif
