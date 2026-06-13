#include "PlayerStates.h"
#include "PlayerComponent.h"
#include "TextureAtlasComponent.h"
#include "GameObject.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include <glm/glm.hpp>
#include <cmath>

enum SoundIds : dae::sound_id
{
	SND_JUMP = 0,
	SND_SWEAR = 1,
};

namespace dae
{
	static int FacingFrame(int dRow, int dCol)
	{
		if (dRow < 0 && dCol < 0)  return 1;
		if (dRow < 0 && dCol == 0) return 0;
		if (dRow > 0 && dCol == 0) return 3;
		if (dRow > 0 && dCol > 0)  return 2;
		return 0;
	}

	static PlayerState* StartJump(PlayerComponent& p, int dRow, int dCol)
	{
		if (!p.TryJump(dRow, dCol)) return nullptr;
		p.SetFacingFrame(FacingFrame(dRow, dCol));
		ServiceLocator::GetSoundSystem().Play(SND_JUMP, 1.f);
		auto* s = new JumpingState{};
		s->m_dRow = dRow; s->m_dCol = dCol;
		return s;
	}

	void IdleState::OnEnter(PlayerComponent& p)
	{
		if (!p.HasBufferedJump()) return;
		auto buf = p.GetInputBuffer();
		p.ConsumeBuffer();
		auto* next = StartJump(p, buf.dRow, buf.dCol);
		if (next) p.TransitionTo(next);
	}

	void IdleState::OnExit(PlayerComponent&) {}

	PlayerState* IdleState::Update(PlayerComponent&, float)
	{
		return nullptr;
	}

	PlayerState* IdleState::OnJumpUpLeft(PlayerComponent& p) { return StartJump(p, -1, -1); }
	PlayerState* IdleState::OnJumpUpRight(PlayerComponent& p) { return StartJump(p, -1, 0); }
	PlayerState* IdleState::OnJumpDownLeft(PlayerComponent& p) { return StartJump(p, +1, 0); }
	PlayerState* IdleState::OnJumpDownRight(PlayerComponent& p) { return StartJump(p, +1, +1); }

	PlayerState* IdleState::OnDie(PlayerComponent&)
	{
		return new DeadState{};
	}

	void JumpingState::OnEnter(PlayerComponent& p)
	{
		m_elapsed = 0.f;
		m_start = {
			p.GetGO()->GetWorldPosition().x,
			p.GetGO()->GetWorldPosition().y,
			0.f
		};
		m_dest = p.GetJumpTarget();
		m_pendingRow = p.GetRow() + m_dRow;
		m_pendingCol = p.GetCol() + m_dCol;
	}

	void JumpingState::OnExit(PlayerComponent& p)
	{
		p.GetGO()->SetLocalPosition(m_dest);

		if (p.IsJumpOffGrid())
			p.CommitOffGridJump();
		else
			p.CommitJump(m_pendingRow, m_pendingCol);
	}

	PlayerState* JumpingState::Update(PlayerComponent& p, float dt)
	{
		m_elapsed += dt;
		const float t = glm::min(m_elapsed / PlayerComponent::JumpDuration, 1.f);

		glm::vec3 pos = m_start + (m_dest - m_start) * t;
		pos.y -= 24.f * std::sin(t * 3.14159265f);
		p.GetGO()->SetLocalPosition(pos);

		if (t >= 1.f)
		{
			if (p.IsJumpOffGrid())
				return new DeadState{};

			return new LandingState{};
		}
		return nullptr;
	}

	PlayerState* JumpingState::OnJumpUpLeft(PlayerComponent& p) { p.BufferJump(-1, -1); return nullptr; }
	PlayerState* JumpingState::OnJumpUpRight(PlayerComponent& p) { p.BufferJump(-1, 0); return nullptr; }
	PlayerState* JumpingState::OnJumpDownLeft(PlayerComponent& p) { p.BufferJump(+1, 0); return nullptr; }
	PlayerState* JumpingState::OnJumpDownRight(PlayerComponent& p) { p.BufferJump(+1, +1); return nullptr; }

	PlayerState* JumpingState::OnDie(PlayerComponent&)
	{
		return new DeadState{};
	}

	void LandingState::OnEnter(PlayerComponent&)
	{
		m_timer = 0.12f;
	}

	void LandingState::OnExit(PlayerComponent&) {}

	PlayerState* LandingState::Update(PlayerComponent&, float dt)
	{
		m_timer -= dt;
		return (m_timer <= 0.f) ? new IdleState{} : nullptr;
	}

	PlayerState* LandingState::OnJumpUpLeft(PlayerComponent& p) { p.BufferJump(-1, -1); return nullptr; }
	PlayerState* LandingState::OnJumpUpRight(PlayerComponent& p) { p.BufferJump(-1, 0); return nullptr; }
	PlayerState* LandingState::OnJumpDownLeft(PlayerComponent& p) { p.BufferJump(+1, 0); return nullptr; }
	PlayerState* LandingState::OnJumpDownRight(PlayerComponent& p) { p.BufferJump(+1, +1); return nullptr; }

	PlayerState* LandingState::OnDie(PlayerComponent&)
	{
		return new DeadState{};
	}

	void DeadState::OnEnter(PlayerComponent& p)
	{
		m_timer = 2.0f;
		m_killed = false;

		ServiceLocator::GetSoundSystem().Play(SND_SWEAR, 1.f);

		const glm::vec2 deathPos{
			p.GetGO()->GetWorldPosition().x,
			p.GetGO()->GetWorldPosition().y
		};

		p.GetGO()->SetLocalPosition(-9999.f, -9999.f);

		p.SpawnCurses(deathPos);
	}

	void DeadState::OnExit(PlayerComponent& p)
	{
		p.DespawnCurses();
	}

	PlayerState* DeadState::Update(PlayerComponent& p, float dt)
	{
		m_timer -= dt;

		if (!m_killed && m_timer <= 1.0f)
		{
			m_killed = true;
			p.Kill();
		}

		if (m_timer <= 0.f)
		{
			p.Respawn();
			return new IdleState{};
		}
		return nullptr;
	}
}