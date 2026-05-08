#pragma once
#include "Component.h"
#include "Observer.h"
#include "QbertPlayerState.h"
#include "QbertGridComponent.h"
#include <memory>
#include <glm/vec3.hpp>

namespace dae
{
    inline constexpr EventId EVT_QBERT_DIED = make_sdbm_hash("QbertDied");
    inline constexpr EventId EVT_QBERT_SCORE = make_sdbm_hash("QbertScore");
    inline constexpr EventId EVT_QBERT_STATE_CHANGE = make_sdbm_hash("QbertStateChange");

    class QbertPlayerComponent final : public Component, public Subject
    {
    public:
        QbertPlayerComponent(GameObject* go, QbertGridComponent* grid,
            int startRow = 0, int startCol = 0,
            int maxLives = 3);
        ~QbertPlayerComponent() override = default;

        void Update() override;

        void TransitionTo(QbertPlayerState* newState);

        GameObject* GetGO() const { return GetGameObject(); }

        bool TryJump(int dRow, int dCol);

        void CommitJump(int newRow, int newCol);

        int GetRow() const { return m_row; }
        int GetCol() const { return m_col; }

        const glm::vec3& GetJumpTarget() const { return m_jumpTarget; }

        void Kill();
        void AddScore(int pts);

        int GetLives() const { return m_lives; }
        int GetScore() const { return m_score; }

        void JumpUpLeft();
        void JumpUpRight();
        void JumpDownLeft();
        void JumpDownRight();
        void Die();

        static constexpr float JumpDuration = 0.25f;

    private:
        std::unique_ptr<QbertPlayerState> m_pState;

        QbertGridComponent* m_pGrid;
        int m_row{};
        int m_col{};
        int m_pendingRow{};
        int m_pendingCol{};

        glm::vec3 m_jumpTarget{};

        int m_lives{};
        int m_score{};
    };
}