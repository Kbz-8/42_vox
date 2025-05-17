#ifndef UTILS_H
#define UTILS_H

#include <mutex>
#include <queue>
#include <unistd.h>
#include <filesystem>
#include <condition_variable>

#include <ScopMaths.h>

inline std::filesystem::path GetExecutablePath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

inline std::filesystem::path GetResourcesPath()
{
	return GetExecutablePath().parent_path().parent_path() / "Resources";
}

inline void HashCombine([[maybe_unused]] std::size_t& seed) noexcept {}

template <typename T, typename... Rest>
inline void HashCombine(std::size_t& seed, const T& v, Rest... rest)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	HashCombine(seed, rest...);
}

namespace std
{
	template <>
	struct hash<Scop::Vec2i>
	{
		std::size_t operator()(const Scop::Vec2i& v) const noexcept
		{
			std::size_t hash = 0;
			HashCombine(hash, v.x, v.y);
			return hash;
		}
	};
}

template <typename T>
class ThreadSafeQueue
{
	public:
		inline void Push(T item)
		{
			const std::unique_lock<std::mutex> lock(m_mutex);
			m_queue.push(item);
			m_cond.notify_one();
		}

		inline T Pop()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cond.wait(lock, [this]() { return !m_queue.empty(); });
			T item = m_queue.front();
			m_queue.pop();
			return item;
		}

		bool IsEmpty() const
		{
			const std::unique_lock<std::mutex> lock(m_mutex);
			return m_queue.empty();
		}

	private:
		std::queue<T> m_queue;
		mutable std::mutex m_mutex;
		std::condition_variable m_cond;
};

#endif
