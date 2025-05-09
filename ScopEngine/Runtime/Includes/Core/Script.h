#ifndef __SCOP_SCRIPT__
#define __SCOP_SCRIPT__

#include <Utils/NonOwningPtr.h>

namespace Scop
{
	class ActorScript
	{
		public:
			ActorScript() = default;

			virtual void OnInit(NonOwningPtr<class Actor> actor) = 0;
			virtual void OnUpdate(NonOwningPtr<class Scene> scene, NonOwningPtr<class Actor> actor, class Inputs& input, float delta) = 0;
			virtual void OnQuit(NonOwningPtr<class Actor> actor) = 0;

			virtual ~ActorScript() = default;
	};

	class SpriteScript
	{
		public:
			SpriteScript() = default;

			virtual void OnInit(NonOwningPtr<class Sprite> sprite) = 0;
			virtual void OnUpdate(NonOwningPtr<class Scene> scene, NonOwningPtr<class Sprite> sprite, class Inputs& input, float delta) = 0;
			virtual void OnQuit(NonOwningPtr<class Sprite> sprite) = 0;

			virtual ~SpriteScript() = default;
	};

	class NarratorScript
	{
		public:
			NarratorScript() = default;

			virtual void OnInit() = 0;
			virtual void OnUpdate(NonOwningPtr<class Scene> scene, class Inputs& input, float delta) = 0;
			virtual void OnQuit() = 0;

			virtual ~NarratorScript() = default;
	};
}

#endif
