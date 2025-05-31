#include <Graphics/Scene.h>
#include <Renderer/Renderer.h>
#include <Renderer/RenderCore.h>
#include <Platform/Inputs.h>
#include <Core/Logs.h>
#include <Renderer/ViewerData.h>
#include <Core/EventBus.h>
#include <Core/Engine.h>
#include <Graphics/DogicaTTF.h>

#include <cstring>

namespace Scop
{
	Scene::Scene(std::string_view name, SceneDescriptor desc)
	: m_name(name), m_descriptor(std::move(desc)), p_parent(nullptr)
	{
		LoadFont("default", 6.0f);
	}

	Scene::Scene(std::string_view name, SceneDescriptor desc, NonOwningPtr<Scene> parent)
	: m_name(name), m_descriptor(std::move(desc)), p_parent(parent)
	{
		LoadFont("default", 6.0f);
	}

	Actor& Scene::CreateActor(Model model) noexcept
	{
		UUID uuid = UUID();
		return m_actors.try_emplace(uuid, uuid, std::move(model)).first->second;
	}

	Actor& Scene::CreateActor(std::string_view name, Model model)
	{
		UUID uuid = UUID();
		return m_actors.try_emplace(uuid, uuid, std::move(model)).first->second;
	}

	Narrator& Scene::CreateNarrator() noexcept
	{
		UUID uuid = UUID();
		return m_narrators.try_emplace(uuid, uuid).first->second;
	}

	Narrator& Scene::CreateNarrator(std::string_view name)
	{
		UUID uuid = UUID();
		return m_narrators.try_emplace(uuid, uuid).first->second;
	}

	Sprite& Scene::CreateSprite(std::shared_ptr<Texture> texture) noexcept
	{
		UUID uuid = UUID();
		return m_sprites.try_emplace(uuid, uuid, texture).first->second;
	}

	Sprite& Scene::CreateSprite(std::string_view name, std::shared_ptr<Texture> texture)
	{
		UUID uuid = UUID();
		return m_sprites.try_emplace(uuid, uuid, texture).first->second;
	}

	Text& Scene::CreateText(std::string text) noexcept
	{
		UUID uuid = UUID();
		return m_texts.try_emplace(uuid, uuid, std::move(text), p_bound_font).first->second;
	}

	Text& Scene::CreateText(std::string_view name, std::string text)
	{
		UUID uuid = UUID();
		return m_texts.try_emplace(uuid, uuid, std::move(text), p_bound_font).first->second;
	}

	void Scene::LoadFont(std::filesystem::path path, float scale)
	{
		std::shared_ptr<Font> font = m_fonts_registry.GetFont(path, scale);
		if(!font)
		{
			if(path.string() == "default")
				font = std::make_shared<Font>("default", dogica_ttf, scale);
			else
				font = std::make_shared<Font>(std::move(path), scale);
			font->BuildFont();
			m_fonts_registry.RegisterFont(font);
		}
		p_bound_font = font;
	}

	void Scene::RemoveActor(Actor& actor) noexcept
	{
		auto it = m_actors.find(actor.GetUUID());
		if(it == m_actors.end())
		{
			Error("Actor not found");
			return;
		}
		m_actors.erase(it);
	}

	void Scene::RemoveNarrator(Narrator& narrator) noexcept
	{
		auto it = m_narrators.find(narrator.GetUUID());
		if(it == m_narrators.end())
		{
			Error("Narrator not found");
			return;
		}
		m_narrators.erase(it);
	}

	void Scene::RemoveSprite(Sprite& sprite) noexcept
	{
		auto it = m_sprites.find(sprite.GetUUID());
		if(it == m_sprites.end())
		{
			Error("Sprite not found");
			return;
		}
		m_sprites.erase(it);
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
				m_depth.Init(renderer->GetSwapchain().GetSwapchainImages().back().GetWidth(), renderer->GetSwapchain().GetSwapchainImages().back().GetHeight(), false, m_name + "_depth");
				m_depth.CreateSampler();
			}

			if(event.What() == Event::ResizeEventCode || event.What() == Event::SceneHasChangedEventCode)
				m_pipeline.Destroy(); // Ugly but f*ck off
		};
		EventBus::RegisterListener({ functor, m_name + std::to_string(reinterpret_cast<std::uintptr_t>(this)) });

		if(m_descriptor.post_process_shader)
		{
			m_post_process.set = std::make_shared<DescriptorSet>(m_descriptor.post_process_shader->GetShaderLayout().set_layouts[0].second, m_descriptor.post_process_shader->GetPipelineLayout().set_layouts[0], ShaderType::Fragment);
			m_post_process.data_buffer = std::make_shared<UniformBuffer>();
			m_post_process.data_buffer->Init(m_descriptor.post_process_data_size, m_name + "_post_process_data_buffer");
		}

		m_post_process.data.Allocate(m_descriptor.post_process_data_size);

		auto vertex_shader = RenderCore::Get().GetDefaultVertexShader();
		m_depth.Init(renderer->GetSwapchain().GetSwapchainImages().back().GetWidth(), renderer->GetSwapchain().GetSwapchainImages().back().GetHeight(), false, m_name + "_depth");
		m_depth.CreateSampler();
		m_forward.matrices_buffer = std::make_shared<UniformBuffer>();
		m_forward.matrices_buffer->Init(sizeof(ViewerData), m_name + "_matrice_buffer");

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
		for(auto& [_, actor] : m_actors)
			actor.Update(this, input, timestep);
		for(auto& [_, narrator] : m_narrators)
			narrator.Update(this, input, timestep);
		for(auto& [_, sprite] : m_sprites)
			sprite.Update(this, input, timestep);
		if(m_descriptor.camera)
			m_descriptor.camera->Update(input, aspect, timestep);
	}

	void Scene::Destroy()
	{
		RenderCore::Get().WaitDeviceIdle();
		p_skybox.reset();
		m_depth.Destroy();
		m_actors.clear();
		m_narrators.clear();
		m_sprites.clear();
		m_pipeline.Destroy();
		m_descriptor.fragment_shader.reset();
		m_descriptor.post_process_shader.reset();
		m_forward.matrices_buffer->Destroy();
		if(m_post_process.data_buffer)
			m_post_process.data_buffer->Destroy();
		m_fonts_registry.Reset();
		for(auto& child : m_scene_children)
			child.Destroy();
	}
}
