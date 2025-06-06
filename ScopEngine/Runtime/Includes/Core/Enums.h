#ifndef __SCOPE_CORE_ENUMS__
#define __SCOPE_CORE_ENUMS__

#include <cstddef>

namespace Scop
{
	enum class LogType
	{
		Debug = 0,
		Message,
		Warning,
		Error,
		FatalError,

		EndEnum
	};

	constexpr std::size_t LogTypeCount = static_cast<std::size_t>(LogType::EndEnum);

	enum class Event
	{
		SceneHasChangedEventCode,
		ResizeEventCode,
		FrameBeginEventCode,
		FatalErrorEventCode,
		QuitEventCode,
		MemoryChunkAllocationFailed,

		EndEnum
	};

	constexpr std::size_t EventCount = static_cast<std::size_t>(Event::EndEnum);
}

#endif
