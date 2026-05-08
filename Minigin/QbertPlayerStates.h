#pragma once
#include "QbertPlayerState.h"
#include "QbertPlayerComponent.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

namespace dae
{
    class QbertDeadState;
    class QbertIdleState;
    class QbertJumpingState;
    class QbertLandingState;

    class QbertDeadState final : public QbertPlayerState
    {
    public:
        void OnEnter(QbertPlayerComponent&) override
        {
            m_timer = 1.0f;
            std::cout << "[Qbert] DEAD\n";
        }
        void OnExit(QbertPlayerComponent&) override {}
        QbertPlayerState* Update(QbertPlayerComponent& p, float dt) override
        {
            m_timer -= dt;
            if (m_timer <= 0.f) p.Kill();
            return nullptr;
        }
    private:
        float m_timer{ 1.f };
    };

    class QbertIdleState final : public QbertPlayerState
    {
    public:
        void OnEnter(QbertPlayerComponent&) override { std::cout << "[Qbert] IDLE\n"; }
        void OnExit(QbertPlayerComponent&) override {}
        QbertPlayerState* Update(QbertPlayerComponent&, float) override { return nullptr; }

        QbertPlayerState* OnJumpUpLeft(QbertPlayerComponent& p) override;
        QbertPlayerState* OnJumpUpRight(QbertPlayerComponent& p) override;
        QbertPlayerState* OnJumpDownLeft(QbertPlayerComponent& p) override;
        QbertPlayerState* OnJumpDownRight(QbertPlayerComponent& p) override;
        QbertPlayerState* OnDie(QbertPlayerComponent&)   override;
    };

    class QbertLandingState final : public QbertPlayerState
    {
    public:
        void OnEnter(QbertPlayerComponent&) override { m_timer = 0.12f; std::cout << "[Qbert] LANDING\n"; }
        void OnExit(QbertPlayerComponent&) override {}
        QbertPlayerState* Update(QbertPlayerComponent&, float dt) override
        {
            m_timer -= dt;
            return (m_timer <= 0.f) ? new QbertIdleState{} : nullptr;
        }
        QbertPlayerState* OnDie(QbertPlayerComponent&) override { return new QbertDeadState{}; }
    private:
        float m_timer{ 0.12f };
    };

    class QbertJumpingState final : public QbertPlayerState
    {
    public:
        void OnEnter(QbertPlayerComponent& p) override
        {
            m_elapsed = 0.f;
            m_start = { p.GetGO()->GetWorldPosition().x,
                        p.GetGO()->GetWorldPosition().y,
                        0.f };
            m_dest = p.GetJumpTarget();
            m_pendingRow = p.GetRow() + m_dRow;
            m_pendingCol = p.GetCol() + m_dCol;
            std::cout << "[Qbert] JUMPING -> (" << m_dest.x << "," << m_dest.y << ")\n";
        }
        void OnExit(QbertPlayerComponent& p) override
        {
            p.GetGO()->SetLocalPosition(m_dest);
            p.CommitJump(m_pendingRow, m_pendingCol);
        }
        QbertPlayerState* Update(QbertPlayerComponent& p, float dt) override
        {
            m_elapsed += dt;
            float t = std::min(m_elapsed / QbertPlayerComponent::JumpDuration, 1.f);
            glm::vec3 pos = m_start + (m_dest - m_start) * t;
            pos.y -= 24.f * std::sin(t * 3.14159265f);
            p.GetGO()->SetLocalPosition(pos);
            return (t >= 1.f) ? static_cast<QbertPlayerState*>(new QbertLandingState{}) : nullptr;
        }
        QbertPlayerState* OnJumpUpLeft(QbertPlayerComponent&) override { return nullptr; }
        QbertPlayerState* OnJumpUpRight(QbertPlayerComponent&) override { return nullptr; }
        QbertPlayerState* OnJumpDownLeft(QbertPlayerComponent&) override { return nullptr; }
        QbertPlayerState* OnJumpDownRight(QbertPlayerComponent&) override { return nullptr; }
        QbertPlayerState* OnDie(QbertPlayerComponent&) override { return new QbertDeadState{}; }

        int m_dRow{}, m_dCol{};
        int m_pendingRow{}, m_pendingCol{};
    private:
        float m_elapsed{};
        glm::vec3 m_start{}, m_dest{};
    };

    
    inline QbertPlayerState* QbertIdleState::OnJumpUpLeft(QbertPlayerComponent& p)
    {
        if (!p.TryJump(-1, -1)) return nullptr;
        auto* s = new QbertJumpingState{};
        s->m_dRow = -1; s->m_dCol = -1;
        return s;
    }
    inline QbertPlayerState* QbertIdleState::OnJumpUpRight(QbertPlayerComponent& p)
    {
        if (!p.TryJump(-1, 0)) return nullptr;
        auto* s = new QbertJumpingState{};
        s->m_dRow = -1; s->m_dCol = 0;
        return s;
    }
    inline QbertPlayerState* QbertIdleState::OnJumpDownLeft(QbertPlayerComponent& p)
    {
        if (!p.TryJump(+1, 0)) return nullptr;
        auto* s = new QbertJumpingState{};
        s->m_dRow = +1; s->m_dCol = 0;
        return s;
    }
    inline QbertPlayerState* QbertIdleState::OnJumpDownRight(QbertPlayerComponent& p)
    {
        if (!p.TryJump(+1, +1)) return nullptr;
        auto* s = new QbertJumpingState{};
        s->m_dRow = +1; s->m_dCol = +1;
        return s;
    }
    inline QbertPlayerState* QbertIdleState::OnDie(QbertPlayerComponent&)
    {
        return new QbertDeadState{};
    }
}