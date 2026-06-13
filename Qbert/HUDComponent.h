#pragma once
#include "Component.h"
#include "Observer.h"
#include "PlayerComponent.h"
#include "TextureComponent.h"
#include "TextObject.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "GameSession.h"
#include <vector>
#include <string>

namespace dae
{
	class HUDComponent final : public Component, public IObserver
	{
	public:
		HUDComponent(GameObject* go,
			PlayerComponent* player,
			Scene* scene,
			float heartX, float firstHeartY, float heartSpacing,
			float scoreX, float scoreY)
			: Component(go)
			, m_scene(scene)
			, m_heartX(heartX)
			, m_firstHeartY(firstHeartY)
			, m_heartSpacing(heartSpacing)
		{
			player->AddObserver(this);

			auto font = ResourceManager::GetInstance().LoadFont("qbert/Minecraft.ttf", 20);
			SDL_Color yellow{ 255, 215, 0, 255 };

			auto scoreGO = std::make_unique<GameObject>();
			scoreGO->SetLocalPosition(scoreX, scoreY);
			m_scoreText = scoreGO->AddComponent<TextObject>("Score: 0", font, false, yellow);
			scene->Add(std::move(scoreGO));

			for (int i = 0; i < GameSession::Get().lives; ++i)
				SpawnHeart(i);
		}

		~HUDComponent() override = default;

		void OnNotify(EventId eventId, int value) override
		{
			if (eventId == EVT_PLAYER_DIED)
			{
				int livesLeft = value;
				int heartIndex = livesLeft; // hearts are 0-indexed from bottom
				if (heartIndex < static_cast<int>(m_heartObjects.size()))
				{
					m_scene->Remove(*m_heartObjects[heartIndex]);
					m_heartObjects.erase(m_heartObjects.begin() + heartIndex);
				}
				GameSession::Get().lives = livesLeft;
			}
			else if (eventId == EVT_PLAYER_SCORE)
			{
				GameSession::Get().score = value;
				if (m_scoreText)
					m_scoreText->SetText("Score: " + std::to_string(value));
			}
		}

	private:
		void SpawnHeart(int index)
		{
			auto heartGO = std::make_unique<GameObject>();
			float y = m_firstHeartY + index * m_heartSpacing;
			heartGO->SetLocalPosition(m_heartX, y);
			auto* tex = heartGO->AddComponent<TextureComponent>(true);
			tex->SetTexture("qbert/Heart.png");
			tex->SetScale(2.f);
			auto* raw = heartGO.get();
			m_scene->Add(std::move(heartGO));
			m_heartObjects.push_back(raw);
		}

		Scene*                  m_scene;
		TextObject*             m_scoreText{ nullptr };
		std::vector<GameObject*> m_heartObjects;

		float m_heartX;
		float m_firstHeartY;
		float m_heartSpacing;
	};
}
