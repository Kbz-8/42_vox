#ifndef WORLD_H
#define WORLD_H

#include <atomic>
#include <unordered_map>

#include <ScopGraphics.h>

#include <Chunk.h>
#include <Utils.h>

constexpr std::uint8_t RENDER_DISTANCE = 10;
constexpr std::uint8_t CHUNKS_UPLOAD_PER_FRAME = 1;

enum class GenerationState: std::uint8_t
{
	Ready,
	Working,
	Finished,
};

class World
{
	public:
		World(Scop::Scene& scene);

		[[nodiscard]] inline Scop::Scene& GetScene() noexcept { return m_scene; }
		[[nodiscard]] inline std::shared_ptr<Scop::Material> GetBlockMaterial() const { return p_block_material; }
		[[nodiscard]] Scop::NonOwningPtr<Chunk> GetChunk(Scop::Vec2i position);

		~World() = default;

	private:
		void UnloadChunks(Scop::Vec2i current_chunk_position);
		void GenerateWorld(Scop::Vec2i current_chunk_position);
		void Upload();

	private:
		std::unordered_map<Scop::Vec2i, Chunk> m_chunks;
		ThreadSafeQueue<std::reference_wrapper<Chunk>> m_chunks_to_upload;
		std::shared_ptr<Scop::Material> p_block_material;
		Scop::Scene& m_scene;
		Scop::Vec2i m_previous_chunk_position;
		std::atomic<GenerationState> m_generation_status = GenerationState::Ready;
};

#endif
