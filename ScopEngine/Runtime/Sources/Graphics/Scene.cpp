#include <Graphics/Scene.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>
#include <Platform/Inputs.h>
#include <Core/Logs.h>
#include <Renderer/ViewerData.h>
#include <Core/EventBus.h>
#include <Core/Engine.h>

#include <cstring>

namespace Scop
{
	Scene::Scene(std::string_view name, SceneDescriptor desc)
	: m_name(name), m_descriptor(std::move(desc)), p_parent(nullptr)
	{
	}

	Scene::Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent)
	: m_name(name), m_descriptor(std::move(desc)), p_parent(parent)
	{
	}

	Actor& Scene::CreateActor(Model model) noexcept
	{
		std::shared_ptr<Actor> actor = std::make_shared<Actor>(std::move(model));
		m_actors.push_back(actor);
		return *actor;
	}

	Actor& Scene::CreateActor(std::string_view name, Model model)
	{
		std::shared_ptr<Actor> actor = std::make_shared<Actor>(std::move(model));
		m_actors.push_back(actor);
		return *actor;
	}

	Sprite& Scene::CreateSprite(std::shared_ptr<Texture> texture) noexcept
	{
		std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(texture);
		m_sprites.push_back(sprite);
		return *sprite;
	}

	Sprite& Scene::CreateSprite(std::string_view name, std::shared_ptr<Texture> texture)
	{
		std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(texture);
		m_sprites.push_back(sprite);
		return *sprite;
	}

	void Scene::SwitchToChild(std::string_view name) const noexcept
	{
		auto it = std::find_if(m_scene_children.begin(), m_scene_children.end(), [name](const Scene& scene){ return name == scene.GetName(); });
		if(it == m_scene_children.end())
		{
			Error("Cannot switch to scene '%', scene not found in children of '%'", name, m_name);
			return;
		}
		ScopEngine::Get().SwitchToScene(const_cast<Scene*>(&(*it)));
	}

	void Scene::SwitchToParent() const noexcept
	{
		ScopEngine::Get().SwitchToScene(p_parent);
	}

	void Scene::Init(NonOwningPtr<Renderer> renderer)
	{
		std::function<void(const EventBase&)> functor = [this, renderer](const EventBase& event)
		{
			if(event.What() == Event::ResizeEventCode)
			{
				m_depth.Destroy();
				m_depth.Init(renderer->GetSwapchain().GetSwapchainImages().back().GetWidth(), renderer->GetSwapchain().GetSwapchainImages().back().GetHeight());
			}

			if(event.What() == Event::ResizeEventCode || event.What() == Event::SceneHasChangedEventCode)
				m_pipeline.Destroy(); // Ugly but f*ck off
		};
		EventBus::RegisterListener({ functor, m_name + std::to_string(reinterpret_cast<std::uintptr_t>(this)) });

		auto vertex_shader = RenderCore::Get().GetDefaultVertexShader();
		m_depth.Init(renderer->GetSwapchain().GetSwapchainImages().back().GetWidth(), renderer->GetSwapchain().GetSwapchainImages().back().GetHeight());
		m_forward.matrices_buffer = std::make_shared<UniformBuffer>();
		m_forward.matrices_buffer->Init(sizeof(ViewerData));

		m_forward.matrices_set = std::make_shared<DescriptorSet>(vertex_shader->GetShaderLayout().set_layouts[0].second, vertex_shader->GetPipelineLayout().set_layouts[0], ShaderType::Vertex);
		for(std::size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_forward.matrices_set->SetUniformBuffer(i, 0, m_forward.matrices_buffer->Get(i));
			m_forward.matrices_set->Update(i);
		}
		m_forward.albedo_set = std::make_shared<DescriptorSet>(m_descriptor.fragment_shader->GetShaderLayout().set_layouts[0].second, m_descriptor.fragment_shader->GetPipelineLayout().set_layouts[0], ShaderType::Fragment);
		for(auto& child : m_scene_children)
			child.Init(renderer);
	}

	void Scene::Update(Inputs& input, float timestep, float aspect)
	{
		for(auto actor : m_actors)
			actor->Update(this, input, timestep);
		for(auto sprite : m_sprites)
			sprite->Update(this, input, timestep);
		if(m_descriptor.camera)
			m_descriptor.camera->Update(input, aspect, timestep);
	}

	void Scene::Destroy()
	{
		p_skybox.reset();
		m_depth.Destroy();
		m_actors.clear();
		m_sprites.clear();
		m_pipeline.Destroy();
		m_descriptor.fragment_shader.reset();
		m_forward.matrices_buffer->Destroy();
		for(auto& child : m_scene_children)
			child.Destroy();
	}
}
