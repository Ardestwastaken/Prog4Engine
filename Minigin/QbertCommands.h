#pragma once
#include "Command.h"
#include "QbertPlayerComponent.h"

namespace dae
{
    class QbertCommand : public GameActorCommand
    {
    public:
        explicit QbertCommand(GameObject* go) : GameActorCommand(go) {}
    protected:
        QbertPlayerComponent* GetPlayer() const
        {
            return GetGameObject() ? GetGameObject()->GetComponent<QbertPlayerComponent>() : nullptr;
        }
    };

    class QbertJumpUpLeftCommand final : public QbertCommand 
    { 
        public: 
            explicit QbertJumpUpLeftCommand(GameObject* go) : QbertCommand(go) {} 
            void Execute() override { if (auto* p = GetPlayer()) p->JumpUpLeft(); } 
    };
    class QbertJumpUpRightCommand final : public QbertCommand 
    { 
        public: 
            explicit QbertJumpUpRightCommand(GameObject* go) : QbertCommand(go) {} 
            void Execute() override { if (auto* p = GetPlayer()) p->JumpUpRight(); } 
    };
    class QbertJumpDownLeftCommand final : public QbertCommand 
    { 
        public: 
            explicit QbertJumpDownLeftCommand(GameObject* go) : QbertCommand(go) {} 
            void Execute() override { if (auto* p = GetPlayer()) p->JumpDownLeft(); } 
    };
    class QbertJumpDownRightCommand final : public QbertCommand 
    { 
        public: 
            explicit QbertJumpDownRightCommand(GameObject* go) : QbertCommand(go) {} 
            void Execute() override { if (auto* p = GetPlayer()) p->JumpDownRight(); } 
    };
    class QbertDieCommand final : public QbertCommand 
    { 
        public: 
            explicit QbertDieCommand(GameObject* go) : QbertCommand(go) {} 
            void Execute() override { if (auto* p = GetPlayer()) p->Die(); } 
    };
}