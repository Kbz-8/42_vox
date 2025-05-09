#include <Graphics/Sprite.h>
#include <Core/Script.h>
#include <Renderer/Image.h>
#include <Graphics/MeshFactory.h>
#include <Core/Logs.h>
#include <Core/UUID.h>

namespace Scop
{
	Sprite::Sprite(std::shared_ptr<Texture> texture)
	{
		Verify((bool)texture, "Sprite: invalid texture");
		m_uuid = UUID();
		p_mesh = CreateQuad(0, 0, texture->GetWidth(), texture->GetHeight());
		p_texture = texture;
		if(p_script)
			p_script->OnInit(this);
	}

	void Sprite::Update(NonOwningPtr<Scene> scene, Inputs& input, float delta)
	{
		if(p_script)
			p_script->OnUpdate(scene, this, input, delta);
	}

	Sprite::~Sprite()
	{
		if(p_script)
			p_script->OnQuit(this);
	}
}
