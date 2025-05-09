#ifndef __SCOP_NATIVE_SCRIPT__
#define __SCOP_NATIVE_SCRIPT__

#include <functional>

#include <Core/Script.h>

namespace Scop
{
	class NativeActorScript : public ActorScript
	{
		public:
			NativeActorScript(std::function<void(NonOwningPtr<class Actor>)> on_init, std::function<void(NonOwningPtr<class Scene>, NonOwningPtr<class Actor>, class Inputs&, float)> on_update, std::function<void(NonOwningPtr<class Actor>)> on_quit)
			: f_on_init(std::move(on_init)), f_on_update(std::move(on_update)), f_on_quit(std::move(on_quit))
			{}

			inline void OnInit(NonOwningPtr<class Actor> actor) override { if(f_on_init) f_on_init(actor); }
			inline void OnUpdate(NonOwningPtr<class Scene> scene, NonOwningPtr<class Actor> actor, class Inputs& input, float delta) override { if(f_on_update) f_on_update(scene, actor, input, delta); }
			inline void OnQuit(NonOwningPtr<class Actor> actor) override { if(f_on_quit) f_on_quit(actor); }

			~NativeActorScript() = default;

		private:
			std::function<void(NonOwningPtr<class Actor>)> f_on_init;
			std::function<void(NonOwningPtr<class Scene>, NonOwningPtr<class Actor>, class Inputs&, float)> f_on_update;
			std::function<void(NonOwningPtr<class Actor>)> f_on_quit;
	};

	class NativeSpriteScript : public SpriteScript
	{
		public:
			NativeSpriteScript(std::function<void(NonOwningPtr<class Sprite>)> on_init, std::function<void(NonOwningPtr<class Scene>, NonOwningPtr<class Sprite>, class Inputs&, float)> on_update, std::function<void(NonOwningPtr<class Sprite>)> on_quit)
			: f_on_init(std::move(on_init)), f_on_update(std::move(on_update)), f_on_quit(std::move(on_quit))
			{}

			inline void OnInit(NonOwningPtr<class Sprite> sprite) override { if(f_on_init) f_on_init(sprite); }
			inline void OnUpdate(NonOwningPtr<class Scene> scene, NonOwningPtr<class Sprite> sprite, class Inputs& input, float delta) override { if(f_on_update) f_on_update(scene, sprite, input, delta); }
			inline void OnQuit(NonOwningPtr<class Sprite> sprite) override { if(f_on_quit) f_on_quit(sprite); }

			~NativeSpriteScript() = default;

		private:
			std::function<void(NonOwningPtr<class Sprite>)> f_on_init;
			std::function<void(NonOwningPtr<class Scene>, NonOwningPtr<class Sprite>, class Inputs&, float)> f_on_update;
			std::function<void(NonOwningPtr<class Sprite>)> f_on_quit;
	};

	class NativeNarratorScript : public NarratorScript
	{
		public:
			NativeNarratorScript(std::function<void()> on_init, std::function<void(NonOwningPtr<class Scene>, class Inputs&, float)> on_update, std::function<void()> on_quit)
			: f_on_init(std::move(on_init)), f_on_update(std::move(on_update)), f_on_quit(std::move(on_quit))
			{}

			inline void OnInit() override { if(f_on_init) f_on_init(); }
			inline void OnUpdate(NonOwningPtr<class Scene> scene, class Inputs& input, float delta) override { if(f_on_update) f_on_update(scene, input, delta); }
			inline void OnQuit() override { if(f_on_quit) f_on_quit(); }

			~NativeNarratorScript() = default;

		private:
			std::function<void()> f_on_init;
			std::function<void(NonOwningPtr<class Scene>, class Inputs&, float)> f_on_update;
			std::function<void()> f_on_quit;
	};
}

#endif
