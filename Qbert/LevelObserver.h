#pragma once
#include "Component.h"
#include "Observer.h"
#include "PlayerComponent.h"
#include "GridComponent.h"
#include "CoilyComponent.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"

namespace dae
{
	class LevelObserverComponent final : public Component, public IObserver
	{
	public:
		LevelObserverComponent(GameObject* go,
			PlayerComponent* player,
			GridComponent*   grid,
			CoilyComponent*  coily,
			int nextSceneIndex,
			float freezeDelay = 2.f)
			: Component(go)
			, m_player(player)
			, m_grid(grid)
			, m_coily(coily)
			, m_nextSceneIndex(nextSceneIndex)
			, m_freezeDelay(freezeDelay)
		{
			player->AddObserver(this);
		}

		~LevelObserverComponent() override = default;

		void ForceComplete() { OnNotify(EVT_ALL_TILES_COLORED, 0); }

		void OnNotify(EventId eventId, int) override
		{
			if (eventId != EVT_ALL_TILES_COLORED || m_grid->IsCelebrating()) return;

			ServiceLocator::GetSoundSystem().Play(SND_ROUND_COMPLETE, 1.f);

			const bool isLast = m_grid->IsLastCubeType();

			m_grid->StartCelebration(2.f, 0.1f, [this, isLast]()
			{
				if (isLast)
				{
					SceneManager::GetInstance().LoadScene(m_nextSceneIndex, m_freezeDelay);
				}
				else
				{
					m_grid->ResetToNextCubeType();
					m_player->ResetPosition();
					if (m_coily) m_coily->Respawn();
				}
			});
		}

	private:
		static constexpr dae::sound_id SND_ROUND_COMPLETE = 2;

		PlayerComponent* m_player;
		GridComponent*   m_grid;
		CoilyComponent*  m_coily;
		int              m_nextSceneIndex;
		float            m_freezeDelay;
	};
}
