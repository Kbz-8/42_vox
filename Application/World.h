#ifndef WORLD_H
#define WORLD_H

#include <atomic>
#include <unordered_map>
#include <shared_mutex>

#include <ScopGraphics.h>

#include <Chunk.h>
#include <Utils.h>
#include <FpsCounter.h>
#include <NoiseCollection.h>

constexpr std::uint8_t RENDER_DISTANCE = 15;
constexpr std::uint8_t CHUNKS_UPLOAD_PER_FRAME = 3;

enum class GenerationState: std::uint8_t
{
	Ready,
	Working,
	Quitting,
	Finished,
};

struct PostProcessData
{
	Scop::Vec2f inv_res;
	std::int32_t underwater;
};

class World
{
	public:
		World(Scop::Scene& scene);

		[[nodiscard]] inline Scop::Scene& GetScene() noexcept { return m_scene; }
		[[nodiscard]] inline std::shared_ptr<Scop::Material> GetBlockMaterial() const { return p_block_material; }
		[[nodiscard]] inline std::shared_ptr<Scop::GraphicPipeline> GetWaterPipeline() const { return p_water_pipeline; }

		[[nodiscard]] Scop::NonOwningPtr<const Chunk> GetChunk(Scop::Vec2i position) const;
		[[nodiscard]] NoiseCollection& GetNoiseGenerator() noexcept { return m_noisecollection; }

		~World();

	private:
		void UnloadChunks();
		void GenerateWorld();
		void Upload();
		void SetupLoading();

	private:
		NoiseCollection m_noisecollection;
		FpsCounter m_fps_counter;
		std::unordered_map<Scop::Vec2i, Chunk> m_chunks;
		ThreadSafeQueue<std::reference_wrapper<Chunk>> m_chunks_to_upload;
		std::shared_ptr<Scop::Material> p_block_material;
		std::shared_ptr<Scop::GraphicPipeline> p_water_pipeline;
		std::shared_ptr<Scop::Shader> p_water_vertex_shader;
		std::shared_ptr<Scop::Shader> p_water_fragment_shader;
		mutable std::shared_mutex m_chunk_mutex;
		Scop::Scene& m_scene;
		Scop::Vec2i m_previous_chunk_position;
		std::atomic<Scop::Vec2i> m_current_chunk_position;
		std::atomic<GenerationState> m_generation_status = GenerationState::Ready;
		Scop::NonOwningPtr<Scop::Text> p_fps_text;
		Scop::NonOwningPtr<Scop::Text> p_loading_text;
		std::atomic<float> m_loading_progress = 0.0f;
		std::uint32_t m_last_fps_count = 0;
		bool m_show_loading_screen = true;
};

#endif
