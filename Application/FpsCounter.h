#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

#include <cstdint>
#include <chrono>

class FpsCounter
{
	public:
		FpsCounter();

		void Update();
		[[nodiscard]] inline std::uint32_t GetFPSCount() const noexcept { return m_current_fps; }

		~FpsCounter() = default;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_now;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_past;
		std::uint32_t m_counter = 0;
		std::uint32_t m_current_fps = 0;
};

#endif
