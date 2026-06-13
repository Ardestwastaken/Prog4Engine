#pragma once
#include "Component.h"
#include "Observer.h"
#include "PlayerComponent.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Controller.h"
#include "Command.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "GameSession.h"
#include <SDL3/SDL.h>
#include <memory>

namespace dae
{
	class GameOverComponent final : public Component, public IObserver
	{
	public:
		GameOverComponent(GameObject* go,
			PlayerComponent* player,
			int menuScene,
			int windowW, int windowH)
			: Component(go)
			, m_menuScene(menuScene)
			, m_windowW(static_cast<float>(windowW))
			, m_windowH(static_cast<float>(windowH))
		{
			player->AddObserver(this);
			m_overlay = ResourceManager::GetInstance().LoadTexture("qbert/Game_Over_Title.png");
		}

		~GameOverComponent() override = default;

		void OnNotify(EventId eventId, int value) override
		{
			if (eventId == EVT_PLAYER_DIED && value == 0 && !m_active)
			{
				m_active = true;
				SceneManager::GetInstance().SetPaused(true);

				auto& input = InputManager::GetInstance();
				input.UnbindAllKeyboardCommands();
				input.BindKeyboardCommand(SDL_SCANCODE_RETURN, KeyState::Down,
					std::make_unique<GameOverConfirmCommand>(m_menuScene));
				input.BindKeyboardCommand(SDL_SCANCODE_SPACE, KeyState::Down,
					std::make_unique<GameOverConfirmCommand>(m_menuScene));
				input.BindControllerCommand(0, Controller::Button::ButtonY, KeyState::Down,
					std::make_unique<GameOverConfirmCommand>(m_menuScene));
				input.BindControllerCommand(0, Controller::Button::ButtonA, KeyState::Down,
					std::make_unique<GameOverConfirmCommand>(m_menuScene));
			}
		}

		void Render() const override
		{
			if (!m_active || !m_overlay) return;

			SDL_Renderer* sdl = Renderer::GetInstance().GetSDLRenderer();
			auto* tex = m_overlay->GetSDLTexture();
			SDL_SetTextureAlphaMod(tex, 255);
			SDL_FRect dst{ 0.f, 0.f, m_windowW, m_windowH };
			SDL_RenderTexture(sdl, tex, nullptr, &dst);
		}

	private:
		class GameOverConfirmCommand final : public Command
		{
		public:
			explicit GameOverConfirmCommand(int menuScene) : m_menuScene(menuScene) {}
			void Execute() override
			{
				GameSession::Get().Reset();
				SceneManager::GetInstance().SetPaused(false);
				SceneManager::GetInstance().LoadScene(m_menuScene);
			}
		private:
			int m_menuScene;
		};

		int   m_menuScene;
		float m_windowW;
		float m_windowH;
		bool  m_active{ false };

		std::shared_ptr<Texture2D> m_overlay;
	};
}