#include <ThreadPool.h>
#include <ScopCore.h>
#include <thread>

ThreadPool::ThreadPool() : m_concurency(std::max(std::thread::hardware_concurrency(), DEFAULT_CONCURENCY)), m_waiting_count(m_concurency)
{
	for(std::size_t i = 0; i < m_concurency; i++)
	{
		m_pool.emplace_back([this]
		{
			using namespace std::chrono_literals;

			for(;;)
			{
				if(m_stop)
					return;
				if(!m_tasks.IsEmpty())
				{
					std::function<void()> task = std::move(m_tasks.Pop());
					m_waiting_count--;
					task();
					m_waiting_count++;
				}
				else
					std::this_thread::sleep_for(10ms);
			}
		});
	}
	Scop::Message("ThreadPool started with a capacity of %", m_concurency);
}

ThreadPool::~ThreadPool()
{
	m_stop = true;
	for(auto& thread : m_pool)
		thread.join();
}
