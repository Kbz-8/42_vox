#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <filesystem>

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

#endif
