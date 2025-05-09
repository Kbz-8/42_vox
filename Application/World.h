#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include <ScopGraphics.h>

#include <Chunk.h>

class World
{
	public:
		World(Scop::Scene& scene);

		[[nodiscard]] inline Scop::Scene& GetScene() noexcept { return m_scene; }
		[[nodiscard]] inline std::shared_ptr<Scop::Material> GetBlockMaterial() const { return p_block_material; }
		[[nodiscard]] Chunk& GetChunk(Scop::Vec2i position);

		~World() = default;

	private:
		void GenerateWorld();

	private:
		static inline constexpr std::size_t CHUNKS_SIZE = 16;

		std::vector<Chunk> m_chunks;
		std::shared_ptr<Scop::Material> p_block_material;
		Scop::Narrator& m_narrator;
		Scop::Scene& m_scene;
};

#endif
