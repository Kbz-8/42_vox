#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>
#include <climits>
#include <filesystem>

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

#endif
