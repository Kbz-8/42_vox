#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <ScopGraphics.h>
#include <ScopMaths.h>

constexpr Scop::Vec3ui CHUNK_SIZE = Scop::Vec3ui{ 32, 256, 32 };

class Chunk
{
	public:
		Chunk(class World& world, Scop::Vec2i offset);

		void GenerateChunk();
		void GenerateMesh();
		[[nodiscard]] std::uint32_t GetBlock(Scop::Vec3i position) const noexcept;
		[[nodiscard]] inline Scop::NonOwningPtr<Scop::Actor> GetActor() const noexcept { return p_actor; }

		~Chunk();

	private:
		std::vector<std::vector<std::vector<std::uint32_t>>> m_data; // Should be a flat array but I cannot manage to flatten the 3D coordinates correctly
		Scop::Vec2i m_offset; // In chunks
		Scop::Vec2i m_position; // In blocks
		class World& m_world;
		Scop::NonOwningPtr<Scop::Actor> p_actor = nullptr;
};

#endif
