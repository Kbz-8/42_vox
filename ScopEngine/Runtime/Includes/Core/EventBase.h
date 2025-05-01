#ifndef __SCOPE_CORE_BASE_EVENT__
#define __SCOPE_CORE_BASE_EVENT__

#include <Core/Enums.h>

namespace Scop
{
	struct EventBase
	{
		virtual Event What() const = 0;
	};
}

#endif
