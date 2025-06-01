#include <FpsCounter.h>
#include <chrono>

FpsCounter::FpsCounter()
{
	m_past = std::chrono::high_resolution_clock::now();
	m_now = m_past;
}

void FpsCounter::Update()
{
	using namespace std::chrono_literals;
	m_now = std::chrono::high_resolution_clock::now();

	if(std::chrono::duration_cast<std::chrono::seconds>(m_now - m_past) >= 1s)
	{
		m_past = m_now;
		m_current_fps = m_counter;
		m_counter = 0;
	}
	m_counter++;
}
