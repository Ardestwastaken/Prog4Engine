#pragma once
#include "Component.h"
#include <glm/vec2.hpp>

namespace dae
{
	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent(GameObject* gameObject, glm::vec2 centre, float radius, float speed);
		~RotatorComponent() override = default;

		RotatorComponent(const RotatorComponent&) = delete;
		RotatorComponent(RotatorComponent&&) = delete;
		RotatorComponent& operator=(const RotatorComponent&) = delete;
		RotatorComponent& operator=(RotatorComponent&&) = delete;

		void Update() override;

	private:
		glm::vec2 m_centre{ 0, 0 };
		float m_radius{ 50.f };
		float m_speed{ 1.f };
		float m_angle{ 0.f };
	};
}