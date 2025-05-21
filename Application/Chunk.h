#ifndef CHUNK_H
#define CHUNK_H

#include <vector>

#include <ScopGraphics.h>
#include <ScopMaths.h>

constexpr Scop::Vec3ui CHUNK_SIZE = Scop::Vec3ui{ 32, 256, 32 };
constexpr Scop::Vec3ui CHUNK_SIZE_HALF = CHUNK_SIZE / 2 ;
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
		[[nodiscard]] inline Scop::Vec2i GetPosition() const noexcept { return m_position; }

		~Chunk() = default;

	private:
		std::vector<Scop::Vertex> m_mesh_data;
		std::vector<std::uint32_t> m_mesh_index_data;
		std::array<std::array<std::uint32_t, CHUNK_SIZE.y>, CHUNK_SIZE.x * CHUNK_SIZE.z> m_data;
		Scop::Vec2i m_offset; // In chunks
		Scop::Vec2i m_position; // In blocks
		class World& m_world;
		Scop::NonOwningPtr<Scop::Actor> p_actor = nullptr;
};

#endif
