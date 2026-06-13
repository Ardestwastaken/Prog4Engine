#include "PlayerComponent.h"
#include "PlayerStates.h"
#include "GridComponent.h"
#include "TextureAtlasComponent.h"
#include "TextureComponent.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Timer.h"
#include "GameSession.h"

namespace dae
{
	PlayerComponent::PlayerComponent(GameObject* go,
		GridComponent* grid,
		Scene* scene,
		float spriteW, float spriteH,
		int startRow, int startCol,
		int maxLives)
		: Component(go)
		, m_pGrid(grid)
		, m_pScene(scene)
		, m_spriteW(spriteW)
		, m_spriteH(spriteH)
		, m_row(startRow)
		, m_col(startCol)
		, m_pendingRow(startRow)
		, m_pendingCol(startCol)
		, m_startRow(startRow)
		, m_startCol(startCol)
		, m_lastSafeRow(startRow)
		, m_lastSafeCol(startCol)
		, m_lives(GameSession::Get().lives)
		, m_score(GameSession::Get().score)
	{
		(void)maxLives;

		if (m_pGrid)
		{
			auto p = m_pGrid->TileWorldPos(m_row, m_col);
			go->SetLocalPosition(p.x, p.y);
		}

		m_pState = std::make_unique<IdleState>();
		m_pState->OnEnter(*this);
	}

	void PlayerComponent::Update()
	{
		const float dt = Time::GetDeltaTime();
		if (!m_pState) return;

		TickBuffer(dt);

		PlayerState* next = m_pState->Update(*this, dt);
		if (next) TransitionTo(next);

		if (auto* atlas = GetGameObject()->GetComponent<TextureAtlasComponent>())
		{
			const auto& wp = GetGameObject()->GetWorldPosition();
			atlas->SetDestRect(wp.x - m_spriteW * 0.5f, wp.y - m_spriteH, m_spriteW, m_spriteH);
		}
	}

	void PlayerComponent::TransitionTo(PlayerState* newState)
	{
		if (m_pState) m_pState->OnExit(*this);
		m_pState.reset(newState);
		if (m_pState) m_pState->OnEnter(*this);
		NotifyObservers(EVT_PLAYER_STATE_CHANGE, 0);
	}

	bool PlayerComponent::TryJump(int dRow, int dCol)
	{
		int nr = m_row + dRow;
		int nc = m_col + dCol;

		m_pendingRow = nr;
		m_pendingCol = nc;

		if (!m_pGrid || !m_pGrid->IsValid(nr, nc))
		{
			m_jumpOffGrid = true;
			glm::vec2 p = m_pGrid
				? glm::vec2{
					m_pGrid->TileWorldPos(m_row, m_col).x + dCol * m_pGrid->TileW() - dRow * m_pGrid->TileW() * 0.5f,
					m_pGrid->TileWorldPos(m_row, m_col).y + dRow * m_pGrid->TileH()
				  }
				: glm::vec2{ GetGameObject()->GetWorldPosition().x, GetGameObject()->GetWorldPosition().y };
			m_jumpTarget = { p.x, p.y, 0.f };
			return true;
		}

		m_jumpOffGrid = false;
		auto p = m_pGrid->TileWorldPos(nr, nc);
		m_jumpTarget = { p.x, p.y, 0.f };
		return true;
	}

	void PlayerComponent::CommitJump(int newRow, int newCol)
	{
		if (!m_pGrid || !m_pGrid->IsValid(newRow, newCol))
			return;

		const bool wasComplete = m_pGrid->AllTilesColored();
		const bool wasColored  = m_pGrid->IsTileColored(newRow, newCol);

		m_lastSafeRow = m_row;
		m_lastSafeCol = m_col;
		m_row = newRow;
		m_col = newCol;

		if (!wasColored)
		{
			AddScore(25);
			NotifyObservers(EVT_PLAYER_TILE_COLORED, 0);
		}

		m_pGrid->ColorTile(newRow, newCol);

		if (!wasComplete && m_pGrid->AllTilesColored())
			NotifyObservers(EVT_ALL_TILES_COLORED, 0);
	}

	void PlayerComponent::CommitOffGridJump()
	{
		m_lastSafeRow = m_row;
		m_lastSafeCol = m_col;
	}

	void PlayerComponent::Respawn()
	{
		m_jumpOffGrid = false;
		m_row = m_lastSafeRow;
		m_col = m_lastSafeCol;
		m_pendingRow = m_row;
		m_pendingCol = m_col;

		if (m_pGrid)
		{
			auto p = m_pGrid->TileWorldPos(m_row, m_col);
			GetGameObject()->SetLocalPosition(p.x, p.y);
		}

		if (auto* atlas = GetGameObject()->GetComponent<TextureAtlasComponent>())
			atlas->SetFrame(2, 0);
	}

	void PlayerComponent::ResetPosition()
	{
		m_jumpOffGrid = false;
		m_row = m_startRow;
		m_col = m_startCol;
		m_pendingRow = m_row;
		m_pendingCol = m_col;
		m_lastSafeRow = m_row;
		m_lastSafeCol = m_col;

		if (m_pGrid)
		{
			auto p = m_pGrid->TileWorldPos(m_row, m_col);
			GetGameObject()->SetLocalPosition(p.x, p.y);
		}

		if (auto* atlas = GetGameObject()->GetComponent<TextureAtlasComponent>())
			atlas->SetFrame(2, 0);

		m_pState = std::make_unique<IdleState>();
		m_pState->OnEnter(*this);
	}

	void PlayerComponent::Kill()
	{
		if (m_lives > 0) --m_lives;
		GameSession::Get().lives = m_lives;
		NotifyObservers(EVT_PLAYER_DIED, m_lives);
	}

	void PlayerComponent::AddScore(int pts)
	{
		m_score += pts;
		GameSession::Get().score = m_score;
		NotifyObservers(EVT_PLAYER_SCORE, m_score);
	}

	void PlayerComponent::SpawnCurses(glm::vec2 worldPos)
	{
		auto cursesGO  = std::make_unique<GameObject>();
		auto* cursesRaw = cursesGO.get();

		auto* tex = cursesGO->AddComponent<TextureComponent>(true);
		tex->SetTexture("qbert/Qbert_Curses.png");
		tex->SetScale(2.f);

		cursesGO->SetLocalPosition(worldPos.x, worldPos.y - m_spriteH - 4.f);

		m_pScene->Add(std::move(cursesGO));

		m_pCursesObject = cursesRaw;
	}

	void PlayerComponent::DespawnCurses()
	{
		if (!m_pCursesObject) return;
		m_pScene->Remove(*m_pCursesObject);
		m_pCursesObject = nullptr;
	}

	void PlayerComponent::SetFacingFrame(int col)
	{
		if (auto* atlas = GetGameObject()->GetComponent<TextureAtlasComponent>())
			atlas->SetFrame(col, 0);
	}

	void PlayerComponent::BufferJump(int dRow, int dCol)
	{
		m_inputBuffer = { dRow, dCol, true, BufferWindow };
	}

	void PlayerComponent::ConsumeBuffer()
	{
		m_inputBuffer.active = false;
	}

	void PlayerComponent::TickBuffer(float dt)
	{
		if (!m_inputBuffer.active) return;
		m_inputBuffer.ttl -= dt;
		if (m_inputBuffer.ttl <= 0.f)
			m_inputBuffer.active = false;
	}

	void PlayerComponent::JumpUpLeft()    { auto* n = m_pState->OnJumpUpLeft(*this);    if (n) TransitionTo(n); }
	void PlayerComponent::JumpUpRight()   { auto* n = m_pState->OnJumpUpRight(*this);   if (n) TransitionTo(n); }
	void PlayerComponent::JumpDownLeft()  { auto* n = m_pState->OnJumpDownLeft(*this);  if (n) TransitionTo(n); }
	void PlayerComponent::JumpDownRight() { auto* n = m_pState->OnJumpDownRight(*this); if (n) TransitionTo(n); }
	void PlayerComponent::Die()           { auto* n = m_pState->OnDie(*this);           if (n) TransitionTo(n); }
}
