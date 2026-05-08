#include "QbertPlayerComponent.h"
#include "QbertPlayerStates.h"
#include "GameObject.h"
#include "Timer.h"

namespace dae
{
    QbertPlayerComponent::QbertPlayerComponent(GameObject* go,
        QbertGridComponent* grid,
        int startRow, int startCol,
        int maxLives)
        : Component(go)
        , m_pGrid(grid)
        , m_row(startRow)
        , m_col(startCol)
        , m_pendingRow(startRow)
        , m_pendingCol(startCol)
        , m_lives(maxLives)
    {
        if (m_pGrid)
        {
            auto p = m_pGrid->TileWorldPos(m_row, m_col);
            go->SetLocalPosition(p.x, p.y);
            m_pGrid->SetHighlight(m_row, m_col);
        }

        m_pState = std::make_unique<QbertIdleState>();
        m_pState->OnEnter(*this);
    }

    void QbertPlayerComponent::Update()
    {
        const float dt = Time::GetInstance().GetDeltaTime();
        if (!m_pState) return;

        QbertPlayerState* next = m_pState->Update(*this, dt);
        if (next) TransitionTo(next);

        if (m_pGrid) m_pGrid->SetHighlight(m_row, m_col);
    }

    void QbertPlayerComponent::TransitionTo(QbertPlayerState* newState)
    {
        if (m_pState) m_pState->OnExit(*this);
        m_pState.reset(newState);
        if (m_pState) m_pState->OnEnter(*this);
        NotifyObservers(EVT_QBERT_STATE_CHANGE, 0);
    }

    bool QbertPlayerComponent::TryJump(int dRow, int dCol)
    {
        int nr = m_row + dRow;
        int nc = m_col + dCol;

        if (!m_pGrid || !m_pGrid->IsValid(nr, nc))
            return false;

        m_pendingRow = nr;
        m_pendingCol = nc;

        auto p = m_pGrid->TileWorldPos(nr, nc);
        m_jumpTarget = { p.x, p.y, 0.f };
        return true;
    }

    void QbertPlayerComponent::CommitJump(int newRow, int newCol)
    {
        m_row = newRow;
        m_col = newCol;
        AddScore(25);
    }

    void QbertPlayerComponent::Kill()
    {
        if (m_lives > 0) --m_lives;
        NotifyObservers(EVT_QBERT_DIED, m_lives);
    }

    void QbertPlayerComponent::AddScore(int pts)
    {
        m_score += pts;
        NotifyObservers(EVT_QBERT_SCORE, m_score);
    }

    void QbertPlayerComponent::JumpUpLeft() { auto* n = m_pState->OnJumpUpLeft(*this); if (n) TransitionTo(n); }
    void QbertPlayerComponent::JumpUpRight() { auto* n = m_pState->OnJumpUpRight(*this); if (n) TransitionTo(n); }
    void QbertPlayerComponent::JumpDownLeft() { auto* n = m_pState->OnJumpDownLeft(*this); if (n) TransitionTo(n); }
    void QbertPlayerComponent::JumpDownRight() { auto* n = m_pState->OnJumpDownRight(*this); if (n) TransitionTo(n); }
    void QbertPlayerComponent::Die() { auto* n = m_pState->OnDie(*this); if (n) TransitionTo(n); }
}