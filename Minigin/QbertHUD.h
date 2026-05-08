#pragma once
#include "Component.h"
#include "Observer.h"
#include "TextObject.h"
#include "QbertPlayerComponent.h"
#include <string>

namespace dae
{
    class QbertLivesDisplay final : public Component, public IObserver
    {
    public:
        QbertLivesDisplay(GameObject* go, int playerIndex = 0)
            : Component(go), m_playerIndex(playerIndex) {
        }

        void OnNotify(EventId eventId, int value) override
        {
            if (eventId == EVT_QBERT_DIED)
            {
                if (auto* t = GetGameObject()->GetComponent<TextObject>())
                    t->SetText("Q*bert P" + std::to_string(m_playerIndex + 1)
                        + " Lives: " + std::to_string(value));
            }
        }
    private:
        int m_playerIndex{};
    };

    class QbertScoreDisplay final : public Component, public IObserver
    {
    public:
        QbertScoreDisplay(GameObject* go, int playerIndex = 0)
            : Component(go), m_playerIndex(playerIndex) {
        }

        void OnNotify(EventId eventId, int value) override
        {
            if (eventId == EVT_QBERT_SCORE)
            {
                if (auto* t = GetGameObject()->GetComponent<TextObject>())
                    t->SetText("Q*bert P" + std::to_string(m_playerIndex + 1)
                        + " Score: " + std::to_string(value));
            }
        }
    private:
        int m_playerIndex{};
    };
}
