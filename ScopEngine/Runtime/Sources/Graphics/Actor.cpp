#include <Graphics/Actor.h>
#include <Platform/Inputs.h>

namespace Scop
{
	Actor::Actor()
	{
		if(p_script)
			p_script->OnInit(this);
	}

	Actor::Actor(Model model) : m_model(std::move(model))
	{
		if(p_script)
			p_script->OnInit(this);
	}

	void Actor::Update(NonOwningPtr<Scene> scene, Inputs& input, float delta)
	{
		if(p_script)
			p_script->OnUpdate(scene, this, input, delta);
	}

	Actor::~Actor()
	{
		if(p_script)
			p_script->OnQuit(this);
	}
}
