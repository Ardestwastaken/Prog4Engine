#pragma once

namespace dae
{
    class QbertPlayerComponent;

    class QbertPlayerState
    {
    public:
        virtual ~QbertPlayerState() = default;

        virtual void OnEnter(QbertPlayerComponent& player) = 0;
        virtual void OnExit(QbertPlayerComponent& player) = 0;
        virtual QbertPlayerState* Update(QbertPlayerComponent& player, float dt) = 0;

        virtual QbertPlayerState* OnJumpUpLeft(QbertPlayerComponent& p) { (void)p; return nullptr; }
        virtual QbertPlayerState* OnJumpUpRight(QbertPlayerComponent& p) { (void)p; return nullptr; }
        virtual QbertPlayerState* OnJumpDownLeft(QbertPlayerComponent& p) { (void)p; return nullptr; }
        virtual QbertPlayerState* OnJumpDownRight(QbertPlayerComponent& p) { (void)p; return nullptr; }
        virtual QbertPlayerState* OnDie(QbertPlayerComponent& p) { (void)p; return nullptr; }

    protected:
        QbertPlayerState() = default;
    };
}