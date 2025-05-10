#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>

#include <ScopGraphics.h>

#include <Chunk.h>
#include <Utils.h>
#include <MeshPool.h>

constexpr std::uint8_t RENDER_DISTANCE = 2;

class World
{
	public:
		World(Scop::Scene& scene);

		[[nodiscard]] inline Scop::Scene& GetScene() noexcept { return m_scene; }
		[[nodiscard]] inline std::shared_ptr<Scop::Material> GetBlockMaterial() const { return p_block_material; }
		[[nodiscard]] Scop::NonOwningPtr<Chunk> GetChunk(Scop::Vec2i position);
		[[nodiscard]] inline MeshPool& GetMeshPool() { return m_mesh_pool; }

		~World() = default;

	private:
		void GenerateWorld();

	private:
		std::unordered_map<Scop::Vec2i, Chunk> m_chunks;
		MeshPool m_mesh_pool;
		std::shared_ptr<Scop::Material> p_block_material;
		Scop::Narrator& m_narrator;
		Scop::Scene& m_scene;
		Scop::Vec2i m_previous_chunk_position;
};

#endif
