#include "RotatorComponent.h"
#include "GameObject.h"
#include "Timer.h"
#include <cmath>

namespace dae
{

	RotatorComponent::RotatorComponent(GameObject* gameObject, glm::vec2 centre, float radius, float speed)
		: Component(gameObject)
		, m_centre(centre)
		, m_radius(radius)
		, m_speed(speed)
		, m_angle(0.f)
	{
	}

	void RotatorComponent::Update()
	{
		m_angle += m_speed * dae::Time::GetInstance().GetDeltaTime();

		const float x = m_centre.x + std::cos(m_angle) * m_radius;
		const float y = m_centre.y + std::sin(m_angle) * m_radius;

		GetGameObject()->SetLocalPosition(x, y);
	}
}