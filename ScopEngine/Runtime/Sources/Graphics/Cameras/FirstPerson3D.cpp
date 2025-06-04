#include <Graphics/Cameras/FirstPerson3D.h>
#include <Platform/Inputs.h>

namespace Scop
{
	FirstPerson3D::FirstPerson3D() : BaseCamera(), c_up(0, 1, 0), m_position(0.0, 0.0, 0.0)
	{}

	FirstPerson3D::FirstPerson3D(Vec3f position, float fov, float speed) : BaseCamera(), m_position(std::move(position)), c_up(0, 1, 0), c_speed(speed), m_fov(fov)
	{}

	void FirstPerson3D::Update(class Inputs& input, float aspect, float timestep)
	{
		UpdateView();
		m_target = m_position + m_direction;
		m_view = Mat4f::LookAt(m_position, m_target, c_up);
		m_proj = Mat4f::Perspective(RadianAnglef(m_fov), aspect, 0.1f, 100'000.f);

		if(m_inputs_blocked)
			return;

		if(input.IsMouseGrabbed())
		{
			m_theta -= input.GetXRel() * c_sensivity;
			m_phi -= input.GetYRel() * c_sensivity;
		}

		if(input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
			input.ReleaseMouse();
		else if(input.IsMouseButtonPressed(SDL_BUTTON_LEFT))
			input.GrabMouse();

		m_mov = Vec3f(0.0);

		if(input.IsKeyPressed(SDL_SCANCODE_W) || input.IsKeyPressed(SDL_SCANCODE_UP))
			m_mov -= m_forward;
		if(input.IsKeyPressed(SDL_SCANCODE_S) || input.IsKeyPressed(SDL_SCANCODE_DOWN))
			m_mov += m_forward;
		if(input.IsKeyPressed(SDL_SCANCODE_D) || input.IsKeyPressed(SDL_SCANCODE_RIGHT))
			m_mov -= m_left;
		if(input.IsKeyPressed(SDL_SCANCODE_A) || input.IsKeyPressed(SDL_SCANCODE_LEFT))
			m_mov += m_left;
		if(input.IsKeyPressed(SDL_SCANCODE_LSHIFT) || input.IsKeyPressed(SDL_SCANCODE_RSHIFT))
			m_mov -= c_up;
		if(input.IsKeyPressed(SDL_SCANCODE_SPACE))
			m_mov += c_up;

		/*
		if(input.IsMouseWheelUp())
			m_speed_factor *= 1.5f;
		if(input.IsMouseWheelDown())
			m_speed_factor /= 1.5f;
		*/
		if(input.IsKeyPressed(SDL_SCANCODE_Q))
			m_speed_factor = 20.0f;
		else
			m_speed_factor = 1.0f;
		m_position += m_mov * c_speed * m_speed_factor * timestep;
	}

	void FirstPerson3D::UpdateView()
	{
		m_phi = m_phi > 89 ? 89 : m_phi;
		m_phi = m_phi < -89 ? -89 : m_phi;

		// Spherical coordinate system
		m_direction.x = std::cos(m_phi * Pi<float>() / 180) * std::cos(m_theta * Pi<float>() / 180);
		m_direction.y = std::sin(m_phi * Pi<float>() / 180);
		m_direction.z = std::cos(m_phi * Pi<float>() / 180) * std::sin(-m_theta * Pi<float>() / 180);

		m_left = c_up.CrossProduct(m_direction);
		m_left.Normalize();

		m_forward = c_up.CrossProduct(m_left);
		m_forward.Normalize();
	}
}
