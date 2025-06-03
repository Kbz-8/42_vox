#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <vector>
#include <functional>
#include <Utils.h>

constexpr std::uint32_t DEFAULT_CONCURENCY = 6;

class ThreadPool
{
	public:
		ThreadPool();
		inline void EnqueueTask(std::function<void()> task) { m_tasks.Push(std::move(task)); }
		inline void WaitForAllTasks() const
		{
			using namespace std::chrono_literals;
			for(; m_waiting_count != m_concurency;)
				std::this_thread::sleep_for(10ms);
		}
		~ThreadPool();

	private:
		ThreadSafeQueue<std::function<void()>> m_tasks;
		std::vector<std::thread> m_pool;
		std::uint32_t m_concurency;
		std::atomic_uint32_t m_waiting_count;
		std::atomic_bool m_stop = false;
};

#endif
