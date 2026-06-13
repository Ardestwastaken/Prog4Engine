#pragma once
#include "Component.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "GameSession.h"
#include <vector>
#include <glm/vec2.hpp>

namespace dae
{
	class Command;

	class MenuComponent final : public Component
	{
	public:
		MenuComponent(GameObject* go,
			int soloScene, int coopScene, int versusScene)
			: Component(go)
			, m_selectedIndex(0)
			, m_scenePerItem{ soloScene, coopScene, versusScene }
			, m_modePerItem{ GameMode::Solo, GameMode::Coop, GameMode::Versus }
		{
		}

		void MoveUp()
		{
			m_selectedIndex = (m_selectedIndex - 1 + k_ItemCount) % k_ItemCount;
			UpdateArrow();
			PlaySelectionSound();
		}

		void MoveDown()
		{
			m_selectedIndex = (m_selectedIndex + 1) % k_ItemCount;
			UpdateArrow();
			PlaySelectionSound();
		}

		void Confirm()
		{
			GameSession::Get().mode = m_modePerItem[m_selectedIndex];
			SceneManager::GetInstance().LoadScene(m_scenePerItem[m_selectedIndex]);
		}

		void SetArrow(GameObject* arrow) { m_arrow = arrow; UpdateArrow(); }

		void SetItemPositions(const std::vector<glm::vec2>& positions)
		{
			m_itemPositions = positions;
			UpdateArrow();
		}

	private:
		static constexpr int        k_ItemCount = 3;
		static constexpr float      k_ArrowScreenX = 280.f; // fixed left position
		static constexpr dae::sound_id k_SelectSound = 3;     // SND_CHANGE_SELECTION

		void UpdateArrow()
		{
			if (!m_arrow || m_itemPositions.empty()) return;
			m_arrow->SetLocalPosition(k_ArrowScreenX, m_itemPositions[m_selectedIndex].y);
		}

		void PlaySelectionSound() const
		{
			ServiceLocator::GetSoundSystem().Play(k_SelectSound, 1.f);
		}

		int               m_selectedIndex;
		int               m_scenePerItem[3];
		GameMode          m_modePerItem[3];
		GameObject* m_arrow{ nullptr };
		std::vector<glm::vec2> m_itemPositions;
	};

	class MenuUpCommand final : public Command
	{
	public:
		explicit MenuUpCommand(MenuComponent* menu) : m_menu(menu) {}
		void Execute() override { m_menu->MoveUp(); }
	private:
		MenuComponent* m_menu;
	};

	class MenuDownCommand final : public Command
	{
	public:
		explicit MenuDownCommand(MenuComponent* menu) : m_menu(menu) {}
		void Execute() override { m_menu->MoveDown(); }
	private:
		MenuComponent* m_menu;
	};

	class MenuConfirmCommand final : public Command
	{
	public:
		explicit MenuConfirmCommand(MenuComponent* menu) : m_menu(menu) {}
		void Execute() override { m_menu->Confirm(); }
	private:
		MenuComponent* m_menu;
	};

	class LoadSceneCommand final : public Command
	{
	public:
		explicit LoadSceneCommand(int sceneIndex) : m_sceneIndex(sceneIndex) {}
		void Execute() override { SceneManager::GetInstance().LoadScene(m_sceneIndex); }
	private:
		int m_sceneIndex;
	};
}
