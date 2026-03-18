#pragma once
#include "Component.h"
#include "Observer.h"

#ifdef USE_STEAMWORKS
#include "SteamAchievments.h"
#endif

namespace dae
{
	class PlayerComponent final : public Component, public Subject
	{
	public:
		explicit PlayerComponent(GameObject* go, int maxLives = 3)
			: Component(go)
			, m_lives(maxLives)
		{
		}

		~PlayerComponent() override = default;

		void Die()
		{
			if (m_lives <= 0) return;
			--m_lives;
			NotifyObservers(EVT_PLAYER_DIED, m_lives);
		}

		void AddScore(int points)
		{
			m_score += points;
			NotifyObservers(EVT_SCORE_CHANGED, m_score);

#ifdef USE_STEAMWORKS
			SteamAchievements::GetInstance().CheckScore(m_score);
#endif
		}

		int GetLives() const { return m_lives; }
		int GetScore() const { return m_score; }

	private:
		int m_lives{};
		int m_score{};
	};
}