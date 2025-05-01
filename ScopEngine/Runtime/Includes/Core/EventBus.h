#ifndef __SCOPE_CORE_EVENT_BUS__
#define __SCOPE_CORE_EVENT_BUS__

#include <Core/EventBase.h>
#include <Core/EventListener.h>
#include <string>
#include <vector>

namespace Scop
{
	class EventBus
	{
		public:
			EventBus() = delete;
			static void Send(const std::string& listener_name, const EventBase& event);
			static void SendBroadcast(const EventBase& event);
			inline static void RegisterListener(const EventListener& listener) { s_listeners.push_back(listener); }
			~EventBus() = delete;

		private:
			inline static std::vector<EventListener> s_listeners;
	};
}

#endif
