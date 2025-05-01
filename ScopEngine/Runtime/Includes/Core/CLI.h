#ifndef __SCOP_CLI__
#define __SCOP_CLI__

#include <string>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace Scop
{
	class CommandLineInterface
	{
		public:
			inline CommandLineInterface();

			inline std::optional<std::string> GetOption(std::string_view option) const;
			inline std::optional<std::string> GetArgument(std::string_view arg) const;
			inline bool HasFlag(std::string_view flag) const;
			inline const std::string& GetCommand() const;

			inline void Feed(int ac, char** av);

			inline static CommandLineInterface& Get() noexcept;

			inline ~CommandLineInterface();

		private:
			static inline CommandLineInterface* s_instance = nullptr;

			std::unordered_map<std::string, std::string> m_options;
			std::unordered_set<std::string> m_flags;
			std::unordered_set<std::string> m_args;
			std::string m_cmd;
	};
}

#include <Core/CLI.inl>

#endif
