#include "CoilyStates.h"
#include "CoilyComponent.h"
#include "GridComponent.h"
#include "PlayerComponent.h"
#include "Timer.h"
#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>

namespace dae
{


	static int SnakeIdleFrame(int dRow, int dCol)
	{
		if (dRow < 0 && dCol == 0)  return 2; // NE
		if (dRow < 0 && dCol < 0)   return 4; // NW
		if (dRow > 0 && dCol > 0)   return 6; // SE
		return 8;                              // SW (dRow>0, dCol==0)
	}

	static int SnakeJumpFrame(int dRow, int dCol)
	{
		return SnakeIdleFrame(dRow, dCol) + 1;
	}

	static glm::vec3 JumpArc(glm::vec3 start, glm::vec3 dest, float t)
	{
		glm::vec3 pos = start + (dest - start) * t;
		pos.y -= 20.f * std::sin(t * 3.14159265f);
		return pos;
	}

	void CoilyEggState::OnEnter(CoilyComponent& coily)
	{
		coily.SetGridPos(0, 0);
		// Hide off-screen — will drop in visually when the delay expires
		coily.SetWorldPos({ -9999.f, -9999.f, 0.f });
		coily.UpdateAtlasDestRect();
		coily.SetAtlasFrame(1);
		m_jumping = false;
		m_spawning = true;
		m_landPause = 3.f;
	}

	void CoilyEggState::OnExit(CoilyComponent&) {}

	CoilyState* CoilyEggState::StartNextJump(CoilyComponent& coily)
	{
		int row = coily.GetRow();
		int col = coily.GetCol();

		int dCol = (std::rand() % 2 == 0) ? 0 : 1;
		int nr = row + 1;
		int nc = col + dCol;

		if (!coily.GetGrid()->IsValid(nr, nc))
			dCol = 1 - dCol; // try the other direction
		nc = col + dCol;

		m_destRow = nr;
		m_destCol = nc;

		auto sp = coily.GetGrid()->TileWorldPos(row, col);
		auto dp = coily.GetGrid()->TileWorldPos(nr, nc);
		m_start = { sp.x, sp.y, 0.f };
		m_dest = { dp.x, dp.y, 0.f };
		m_elapsed = 0.f;
		m_jumping = true;

		coily.SetAtlasFrame(1);
		return nullptr;
	}

	CoilyState* CoilyEggState::Update(CoilyComponent& coily, float dt)
	{
		if (m_landPause > 0.f)
		{
			m_landPause -= dt;
			if (m_landPause <= 0.f)
			{
				if (m_spawning)
				{
					// Drop in from above onto the top tile
					m_spawning = false;
					auto tp = coily.GetGrid()->TileWorldPos(0, 0);
					m_start = { tp.x, tp.y - 200.f, 0.f };
					m_dest = { tp.x, tp.y, 0.f };
					m_destRow = 0;
					m_destCol = 0;
					m_elapsed = 0.f;
					m_jumping = true;
					coily.SetAtlasFrame(1);
					return nullptr;
				}

				if (coily.GetRow() >= coily.GetGrid()->NumRows() - 1)
					return new CoilySnakeState{};

				coily.EnableCollision();
				StartNextJump(coily);
			}
			return nullptr;
		}

		if (!m_jumping) return nullptr;

		m_elapsed += dt;
		float t = glm::min(m_elapsed / CoilyComponent::JumpDuration, 1.f);
		coily.SetWorldPos(JumpArc(m_start, m_dest, t));
		coily.UpdateAtlasDestRect();

		if (t >= 1.f)
		{
			m_jumping = false;
			coily.SetWorldPos(m_dest);
			coily.SetGridPos(m_destRow, m_destCol);
			coily.UpdateAtlasDestRect();
			coily.SetAtlasFrame(0);
			m_landPause = CoilyComponent::LandPause;
		}

		return nullptr;
	}

	void CoilySnakeState::OnEnter(CoilyComponent& coily)
	{
		m_jumping = false;
		m_landPause = 0.f;
		m_offGrid = false;
		m_dRow = 1; m_dCol = 0;
		coily.SetAtlasFrame(SnakeIdleFrame(m_dRow, m_dCol));
		PickNextJump(coily);
	}

	void CoilySnakeState::OnExit(CoilyComponent&) {}

	void CoilySnakeState::PickNextJump(CoilyComponent& coily)
	{
		int row = coily.GetRow();
		int col = coily.GetCol();
		int pr = coily.GetPlayer()->GetRow();
		int pc = coily.GetPlayer()->GetCol();

		const int drs[] = { -1, -1, +1, +1 };
		const int dcs[] = { -1,  0,  0, +1 };

		int bestDist = INT_MAX;
		int bestDir = 0;
		bool foundValid = false;

		for (int i = 0; i < 4; ++i)
		{
			int nr = row + drs[i];
			int nc = col + dcs[i];
			bool valid = coily.GetGrid()->IsValid(nr, nc);

			if (!valid && foundValid) continue;

			int dist = std::abs(nr - pr) + std::abs(nc - pc);

			if (valid && !foundValid)
			{
				bestDist = dist;
				bestDir = i;
				foundValid = true;
			}
			else if (dist < bestDist)
			{
				bestDist = dist;
				bestDir = i;
			}
		}

		m_dRow = drs[bestDir];
		m_dCol = dcs[bestDir];

		int nr = row + m_dRow;
		int nc = col + m_dCol;

		m_offGrid = !coily.GetGrid()->IsValid(nr, nc);
		m_destRow = nr;
		m_destCol = nc;

		glm::vec2 dp = m_offGrid
			? glm::vec2{
				coily.GetGrid()->TileWorldPos(row, col).x + m_dCol * coily.GetGrid()->TileW() - m_dRow * coily.GetGrid()->TileW() * 0.5f,
				coily.GetGrid()->TileWorldPos(row, col).y + m_dRow * coily.GetGrid()->TileH()
		}
		: coily.GetGrid()->TileWorldPos(nr, nc);

		auto sp = coily.GetGrid()->TileWorldPos(row, col);
		m_start = { sp.x, sp.y, 0.f };
		m_dest = { dp.x, dp.y, 0.f };
		m_elapsed = 0.f;
		m_jumping = true;

		coily.SetAtlasFrame(SnakeJumpFrame(m_dRow, m_dCol));
	}

	CoilyState* CoilySnakeState::Update(CoilyComponent& coily, float dt)
	{
		if (m_landPause > 0.f)
		{
			m_landPause -= dt;
			if (m_landPause <= 0.f)
			{
				if (m_offGrid)
				{
					// Coily fell off — respawn as egg at top
					return new CoilyEggState{};
				}
				PickNextJump(coily);
			}
			return nullptr;
		}

		if (!m_jumping) return nullptr;

		m_elapsed += dt;
		float t = glm::min(m_elapsed / CoilyComponent::JumpDuration, 1.f);
		coily.SetWorldPos(JumpArc(m_start, m_dest, t));
		coily.UpdateAtlasDestRect();

		if (t >= 1.f)
		{
			m_jumping = false;
			coily.SetWorldPos(m_dest);
			coily.UpdateAtlasDestRect();

			if (!m_offGrid)
			{
				coily.SetGridPos(m_destRow, m_destCol);
				coily.SetAtlasFrame(SnakeIdleFrame(m_dRow, m_dCol));
			}

			m_landPause = CoilyComponent::LandPause;
		}

		return nullptr;
	}


	void CoilyPlayerControlledState::OnEnter(CoilyComponent& coily)
	{
		m_jumping = false;
		m_landPause = 0.f;
		m_hasInput = false;
		coily.EnableCollision();
		coily.SetAtlasFrame(8); // default SW idle
	}

	void CoilyPlayerControlledState::OnExit(CoilyComponent&) {}

	void CoilyPlayerControlledState::InputJump(int dRow, int dCol)
	{
		if (m_jumping || m_landPause > 0.f) return;
		m_hasInput = true;
		m_inputDRow = dRow;
		m_inputDCol = dCol;
	}

	CoilyState* CoilyPlayerControlledState::Update(CoilyComponent& coily, float dt)
	{
		if (m_landPause > 0.f)
		{
			m_landPause -= dt;
			return nullptr;
		}

		if (!m_jumping && m_hasInput)
		{
			m_hasInput = false;
			int row = coily.GetRow();
			int col = coily.GetCol();
			int nr = row + m_inputDRow;
			int nc = col + m_inputDCol;

			bool offGrid = !coily.GetGrid()->IsValid(nr, nc);
			m_destRow = nr;
			m_destCol = nc;
			m_dRow = m_inputDRow;
			m_dCol = m_inputDCol;

			glm::vec2 dp = offGrid
				? glm::vec2{
					coily.GetGrid()->TileWorldPos(row, col).x + m_inputDCol * coily.GetGrid()->TileW() - m_inputDRow * coily.GetGrid()->TileW() * 0.5f,
					coily.GetGrid()->TileWorldPos(row, col).y + m_inputDRow * coily.GetGrid()->TileH()
			}
			: coily.GetGrid()->TileWorldPos(nr, nc);

			auto sp = coily.GetGrid()->TileWorldPos(row, col);
			m_start = { sp.x, sp.y, 0.f };
			m_dest = { dp.x, dp.y, 0.f };
			m_elapsed = 0.f;
			m_jumping = true;

			coily.SetAtlasFrame(SnakeJumpFrame(m_dRow, m_dCol));
		}

		if (!m_jumping) return nullptr;

		m_elapsed += dt;
		float t = glm::min(m_elapsed / CoilyComponent::JumpDuration, 1.f);
		coily.SetWorldPos(JumpArc(m_start, m_dest, t));
		coily.UpdateAtlasDestRect();

		if (t >= 1.f)
		{
			m_jumping = false;
			coily.SetWorldPos(m_dest);
			coily.UpdateAtlasDestRect();

			bool offGrid = !coily.GetGrid()->IsValid(m_destRow, m_destCol);
			if (!offGrid)
			{
				coily.SetGridPos(m_destRow, m_destCol);
				coily.SetAtlasFrame(SnakeIdleFrame(m_dRow, m_dCol));
			}
			else
			{
				return new CoilyEggState{};
			}

			m_landPause = CoilyComponent::LandPause;
		}

		return nullptr;
	}

}