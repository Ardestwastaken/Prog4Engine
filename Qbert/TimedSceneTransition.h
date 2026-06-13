#pragma once
#include "Component.h"
#include "SceneManager.h"
#include "Timer.h"

namespace dae
{
	class TimedSceneTransitionComponent final : public Component
	{
	public:
		TimedSceneTransitionComponent(GameObject* go, int nextScene, float delay)
			: Component(go), m_nextScene(nextScene), m_timer(delay) {}

		void Update() override
		{
			m_timer -= Time::GetDeltaTime();
			if (m_timer <= 0.f && !m_fired)
			{
				m_fired = true;
				SceneManager::GetInstance().LoadScene(m_nextScene);
			}
		}

	private:
		int   m_nextScene;
		float m_timer;
		bool  m_fired{ false };
	};
}
