#include "CoilyComponent.h"
#include "CoilyStates.h"
#include "GridComponent.h"
#include "PlayerComponent.h"
#include "TextureAtlasComponent.h"
#include "GameObject.h"
#include "Timer.h"

namespace dae
{
	CoilyComponent::CoilyComponent(GameObject* go,
		GridComponent*   grid,
		PlayerComponent* player,
		float spriteW, float spriteH)
		: Component(go)
		, m_grid(grid)
		, m_player(player)
		, m_spriteW(spriteW)
		, m_spriteH(spriteH)
	{
		m_state = std::make_unique<CoilyEggState>();
		m_state->OnEnter(*this);
	}

	void CoilyComponent::Update()
	{
		// Freeze Coily completely during the celebration animation
		if (m_grid && m_grid->IsCelebrating()) return;

		if (!m_state) return;
		CoilyState* next = m_state->Update(*this, dae::Time::GetDeltaTime());
		if (next) TransitionTo(next);

		UpdateAtlasDestRect();

		if (m_collisionEnabled &&
			m_player &&
			m_player->GetRow() == m_row &&
			m_player->GetCol() == m_col)
		{
			m_player->Die();
		}
	}

	void CoilyComponent::SetPlayerControlled()
	{
		m_collisionEnabled = false;
		TransitionTo(new CoilyPlayerControlledState{});
	}

	void CoilyComponent::Respawn()
	{
		m_collisionEnabled = false;
		m_row = 0;
		m_col = 0;
		TransitionTo(new CoilyEggState{});
	}

	void CoilyComponent::TransitionTo(CoilyState* next)
	{
		m_state->OnExit(*this);
		m_state.reset(next);
		m_state->OnEnter(*this);
	}

	void CoilyComponent::SetGridPos(int row, int col)
	{
		m_row = row;
		m_col = col;
	}

	glm::vec3 CoilyComponent::GetWorldPos() const
	{
		const auto& p = GetGameObject()->GetWorldPosition();
		return { p.x, p.y, 0.f };
	}

	void CoilyComponent::SetWorldPos(glm::vec3 pos)
	{
		GetGameObject()->SetLocalPosition(pos);
	}

	void CoilyComponent::SetAtlasFrame(int col)
	{
		if (auto* atlas = GetGameObject()->GetComponent<TextureAtlasComponent>())
			atlas->SetFrame(col, 0);
	}

	void CoilyComponent::UpdateAtlasDestRect()
	{
		if (auto* atlas = GetGameObject()->GetComponent<TextureAtlasComponent>())
		{
			const auto& wp = GetGameObject()->GetWorldPosition();
			atlas->SetDestRect(wp.x - m_spriteW * 0.5f, wp.y - m_spriteH, m_spriteW, m_spriteH);
		}
	}
}
