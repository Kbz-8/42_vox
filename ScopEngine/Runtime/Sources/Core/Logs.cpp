#include <ctime>
#include <string>
#include <string_view>

#include <Core/Logs.h>
#include <Utils/Ansi.h>
#include <Core/EventBase.h>
#include <Core/EventBus.h>

namespace Scop
{
	namespace Internal
	{
		struct FatalErrorEvent : public EventBase
		{
			Event What() const override { return Event::FatalErrorEventCode; }
		};
	}

	void Logs::Report(LogType type, std::string message)
	{
		Report(type, 0, {}, {}, std::move(message));
	}

	void Logs::Report(LogType type, unsigned int line, std::string_view file, std::string_view function, std::string message)
	{
		using namespace std::literals;

		#ifndef VOX_DEBUG
		if(type == LogType::Debug && std::getenv("VOX_DEBUG_LOGS") == nullptr)
			return;
		#endif

		std::string code_infos;
		if((type == LogType::Error || type == LogType::FatalError) && !file.empty() && !function.empty())
		{
			code_infos += "{in file '"s;
			code_infos += file;
			code_infos += "', line "s + std::to_string(line) + ", in function '"s;
			code_infos += function;
			code_infos += "'} "s;
		}

		switch(type)
		{
			case LogType::Debug:      std::cout << Ansi::blue <<    "[Scop Debug]       "; break;
			case LogType::Message:    std::cout << Ansi::blue <<    "[Scop Message]     "; break;
			case LogType::Warning:    std::cout << Ansi::magenta << "[Scop Warning]     "; break;
			case LogType::Error:      std::cerr << Ansi::red <<     "[Scop Error]       "; break;
			case LogType::FatalError: std::cerr << Ansi::red <<     "[Scop Fatal Error] "; break;
			default: break;
		}

		std::time_t now = time(0);
		std::tm tstruct = *localtime(&now);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), "[%X] ", &tstruct);
		std::cout << Ansi::yellow << buffer << Ansi::def << code_infos << message << std::endl;

		if(type == LogType::FatalError)
		{
			std::cout << Ansi::bg_red << "Fatal Error: emergency exit" << Ansi::bg_def << std::endl;
			EventBus::Send("__ScopEngine", Internal::FatalErrorEvent{});
		}
	}
}
