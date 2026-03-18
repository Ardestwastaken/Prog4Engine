#pragma once
#include "Component.h"
#include "Observer.h"
#include "TextObject.h"
#include <string>

namespace dae
{
	class LivesDisplayComponent final : public Component, public IObserver
	{
	public:
		LivesDisplayComponent(GameObject* go, int playerIndex = 0)
			: Component(go)
			, m_playerIndex(playerIndex)
		{
		}

		~LivesDisplayComponent() override = default;

		void OnNotify(EventId eventId, int value) override
		{
			if (eventId == EVT_PLAYER_DIED)
			{
				auto* text = GetGameObject()->GetComponent<TextObject>();
				if (text)
					text->SetText("P" + std::to_string(m_playerIndex + 1) + " Lives: " + std::to_string(value));
			}
		}

	private:
		int m_playerIndex{};
	};

	class ScoreDisplayComponent final : public Component, public IObserver
	{
	public:
		ScoreDisplayComponent(GameObject* go, int playerIndex = 0)
			: Component(go)
			, m_playerIndex(playerIndex)
		{
		}

		~ScoreDisplayComponent() override = default;

		void OnNotify(EventId eventId, int value) override
		{
			if (eventId == EVT_SCORE_CHANGED)
			{
				auto* text = GetGameObject()->GetComponent<TextObject>();
				if (text)
					text->SetText("P" + std::to_string(m_playerIndex + 1) + " Score: " + std::to_string(value));
			}
		}

	private:
		int m_playerIndex{};
	};
}