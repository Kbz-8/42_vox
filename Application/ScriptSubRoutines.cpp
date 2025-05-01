#include <ScriptSubRoutines.h>

constexpr const float SPEED = 40.0f;

void WireframeHandler(Scop::NonOwningPtr<Scop::Scene> scene, Scop::Inputs& inputs)
{
	static bool key_pressed_last_frame = false;
	if(inputs.IsKeyPressed(SDL_SCANCODE_F))
		key_pressed_last_frame = true;
	else if(key_pressed_last_frame)
	{
		scene->GetForwardData().wireframe = !scene->GetForwardData().wireframe;
		Scop::RenderCore::Get().WaitDeviceIdle();
		scene->GetPipeline().Destroy();
		key_pressed_last_frame = false;
	}
	static Scop::Vec3f rotations{ 0.0f, 0.0f, 0.0f };
}

void MovementHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta)
{
	static Scop::Vec3f position{ 0.0f, 0.0f, 0.0f };
	if(inputs.IsKeyPressed(SDL_SCANCODE_I))
		position.x += SPEED * delta;
	if(inputs.IsKeyPressed(SDL_SCANCODE_K))
		position.x -= SPEED * delta;
	if(inputs.IsKeyPressed(SDL_SCANCODE_L))
		position.z += SPEED * delta;
	if(inputs.IsKeyPressed(SDL_SCANCODE_J))
		position.z -= SPEED * delta;
	if(inputs.IsKeyPressed(SDL_SCANCODE_U))
		position.y += SPEED * delta;
	if(inputs.IsKeyPressed(SDL_SCANCODE_O))
		position.y -= SPEED * delta;
	actor->SetPosition(position);
}

void ColorsTransitionHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta, Scop::MaterialData& data)
{
	static bool colors_transition = false;
	static bool colors_key_pressed_last_frame = false;
	static std::uint8_t colors_transition_way = 0;
	if(inputs.IsKeyPressed(SDL_SCANCODE_C) && !colors_transition)
		colors_key_pressed_last_frame = true;
	else if(colors_key_pressed_last_frame)
	{
		colors_key_pressed_last_frame = false;
		colors_transition = true;
		colors_transition_way = (data.dissolve_black_white_colors_factor >= 0.5f);
	}
	if(colors_transition)
	{
		if(colors_transition_way == 1)
		{
			data.dissolve_black_white_colors_factor -= 1.0f * delta;
			colors_transition = (data.dissolve_black_white_colors_factor > 0.0f);
		}
		else
		{
			data.dissolve_black_white_colors_factor += 1.0f * delta;
			colors_transition = (data.dissolve_black_white_colors_factor < 1.0f);
		}
	}

	static bool normals_transition = false;
	static bool normals_key_pressed_last_frame = false;
	static std::uint8_t normals_transition_way = 0;
	if(inputs.IsKeyPressed(SDL_SCANCODE_N) && !normals_transition)
		normals_key_pressed_last_frame = true;
	else if(normals_key_pressed_last_frame)
	{
		normals_key_pressed_last_frame = false;
		normals_transition = true;
		normals_transition_way = (data.dissolve_normals_colors_factor >= 0.5f);
	}
	if(normals_transition)
	{
		if(normals_transition_way == 1)
		{
			data.dissolve_normals_colors_factor -= 1.0f * delta;
			normals_transition = (data.dissolve_normals_colors_factor > 0.0f);
		}
		else
		{
			data.dissolve_normals_colors_factor += 1.0f * delta;
			normals_transition = (data.dissolve_normals_colors_factor < 1.0f);
		}
	}
}

void TextureTransitionHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta, Scop::MaterialData& data)
{
	static bool texture_transition = false;
	static bool key_pressed_last_frame = false;
	static std::uint8_t transition_way = 0;

	if(inputs.IsKeyPressed(SDL_SCANCODE_T) && !texture_transition)
		key_pressed_last_frame = true;
	else if(key_pressed_last_frame)
	{
		texture_transition = true;
		key_pressed_last_frame = false;
		transition_way = (data.dissolve_texture_factor >= 0.5f);
	}
	if(texture_transition)
	{
		if(transition_way == 1)
		{
			data.dissolve_texture_factor -= 1.0f * delta;
			texture_transition = (data.dissolve_texture_factor > 0.0f);
		}
		else
		{
			data.dissolve_texture_factor += 1.0f * delta;
			texture_transition = (data.dissolve_texture_factor < 1.0f);
		}
	}
}
