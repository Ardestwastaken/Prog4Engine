#pragma once
#include "CoilyState.h"
#include <glm/vec3.hpp>

namespace dae
{
	class CoilyEggState final : public CoilyState
	{
	public:
		void OnEnter(CoilyComponent& coily) override;
		void OnExit(CoilyComponent& coily)  override;
		CoilyState* Update(CoilyComponent& coily, float dt) override;

	private:
		CoilyState* StartNextJump(CoilyComponent& coily);

		float     m_elapsed{ 0.f };
		glm::vec3 m_start{};
		glm::vec3 m_dest{};
		int       m_destRow{};
		int       m_destCol{};
		bool      m_jumping{ false };
		bool      m_spawning{ true };
		float     m_landPause{ 0.f };
	};

	class CoilySnakeState final : public CoilyState
	{
	public:
		void OnEnter(CoilyComponent& coily) override;
		void OnExit(CoilyComponent& coily)  override;
		CoilyState* Update(CoilyComponent& coily, float dt) override;

	private:
		void PickNextJump(CoilyComponent& coily);

		float     m_elapsed{ 0.f };
		glm::vec3 m_start{};
		glm::vec3 m_dest{};
		int       m_destRow{};
		int       m_destCol{};
		int       m_dRow{};
		int       m_dCol{};
		bool      m_jumping{ false };
		float     m_landPause{ 0.f };
		bool      m_offGrid{ false };
	};

	class CoilyPlayerControlledState final : public CoilyState
	{
	public:
		void OnEnter(CoilyComponent& coily) override;
		void OnExit(CoilyComponent& coily)  override;
		CoilyState* Update(CoilyComponent& coily, float dt) override;

		void InputJump(int dRow, int dCol);

	private:
		float     m_elapsed{ 0.f };
		glm::vec3 m_start{};
		glm::vec3 m_dest{};
		int       m_destRow{};
		int       m_destCol{};
		int       m_dRow{};
		int       m_dCol{};
		bool      m_jumping{ false };
		float     m_landPause{ 0.f };
		bool      m_hasInput{ false };
		int       m_inputDRow{};
		int       m_inputDCol{};
	};
}
