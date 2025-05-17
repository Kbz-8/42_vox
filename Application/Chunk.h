#ifndef CHUNK_H
#define CHUNK_H

#include <vector>

#include <ScopGraphics.h>
#include <ScopMaths.h>

constexpr Scop::Vec3ui CHUNK_SIZE = Scop::Vec3ui{ 32, 32, 32 };
constexpr std::uint32_t CHUNK_VOLUME = CHUNK_SIZE.x * CHUNK_SIZE.y * CHUNK_SIZE.z;

class Chunk
{
	public:
		Chunk(class World& world, Scop::Vec2i offset);

		void GenerateChunk();
		void GenerateMesh();
		void UploadMesh();
		[[nodiscard]] std::uint32_t GetBlock(Scop::Vec3i position) const noexcept;
		[[nodiscard]] inline Scop::NonOwningPtr<Scop::Actor> GetActor() const noexcept { return p_actor; }

		~Chunk() = default;

	private:
		std::vector<Scop::Vertex> m_mesh_data;
		std::vector<std::uint32_t> m_mesh_index_data;
		std::vector<std::uint32_t> m_data;
		Scop::Vec2i m_offset; // In chunks
		Scop::Vec2i m_position; // In blocks
		class World& m_world;
		Scop::NonOwningPtr<Scop::Actor> p_actor = nullptr;
};

#endif
