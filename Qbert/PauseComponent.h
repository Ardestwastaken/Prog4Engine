#pragma once
#include "Component.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Command.h"
#include "Controller.h"
#include "GlobalCommands.h"
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "GameObject.h"
#include <SDL3/SDL.h>
#include <memory>

namespace dae
{
	class PauseToMenuCommand final : public Command
	{
	public:
		explicit PauseToMenuCommand(int menuScene) : m_menuScene(menuScene) {}
		void Execute() override
		{
			SceneManager::GetInstance().SetPaused(false);
			SceneManager::GetInstance().LoadScene(m_menuScene);
		}
	private:
		int m_menuScene;
	};

	class TogglePauseCommand final : public Command
	{
	public:
		TogglePauseCommand(int menuScene, int* escBindIdPtr, int* ctrlYBindIdPtr)
			: m_menuScene(menuScene)
			, m_escBindIdPtr(escBindIdPtr)
			, m_ctrlYBindIdPtr(ctrlYBindIdPtr)
		{
		}

		void Execute() override
		{
			auto& sm = SceneManager::GetInstance();
			auto& input = InputManager::GetInstance();

			if (!sm.IsPaused())
			{
				sm.SetPaused(true);
				input.UnbindKeyboardCommand(*m_escBindIdPtr);
				*m_escBindIdPtr = input.BindKeyboardCommand(
					SDL_SCANCODE_ESCAPE, KeyState::Down,
					std::make_unique<PauseToMenuCommand>(m_menuScene));
				input.UnbindControllerCommand(*m_ctrlYBindIdPtr);
				*m_ctrlYBindIdPtr = input.BindControllerCommand(
					0, Controller::Button::ButtonY, KeyState::Down,
					std::make_unique<PauseToMenuCommand>(m_menuScene));
			}
			else
			{
				sm.SetPaused(false);
				input.UnbindKeyboardCommand(*m_escBindIdPtr);
				*m_escBindIdPtr = input.BindKeyboardCommand(
					SDL_SCANCODE_ESCAPE, KeyState::Down,
					std::make_unique<QuitGameCommand>());
				input.UnbindControllerCommand(*m_ctrlYBindIdPtr);
				*m_ctrlYBindIdPtr = input.BindControllerCommand(
					0, Controller::Button::ButtonY, KeyState::Down,
					std::make_unique<QuitGameCommand>());
			}
		}

	private:
		int  m_menuScene;
		int* m_escBindIdPtr;
		int* m_ctrlYBindIdPtr;
	};

	class PauseManagerComponent final : public Component
	{
	public:
		PauseManagerComponent(GameObject* go, int menuScene)
			: Component(go)
		{
			auto& input = InputManager::GetInstance();

			m_escBindId = input.BindKeyboardCommand(
				SDL_SCANCODE_ESCAPE, KeyState::Down,
				std::make_unique<QuitGameCommand>());

			m_ctrlYBindId = input.BindControllerCommand(
				0, Controller::Button::ButtonY, KeyState::Down,
				std::make_unique<QuitGameCommand>());

			input.BindKeyboardCommand(
				SDL_SCANCODE_P, KeyState::Down,
				std::make_unique<TogglePauseCommand>(menuScene, &m_escBindId, &m_ctrlYBindId));

			input.BindControllerCommand(
				0, Controller::Button::Start, KeyState::Down,
				std::make_unique<TogglePauseCommand>(menuScene, &m_escBindId, &m_ctrlYBindId));
		}

	private:
		int m_escBindId{ -1 };
		int m_ctrlYBindId{ -1 };
	};

	class PauseOverlayComponent final : public Component
	{
	public:
		PauseOverlayComponent(GameObject* go, const std::string& imagePath,
			int windowW, int windowH)
			: Component(go)
			, m_windowW(static_cast<float>(windowW))
			, m_windowH(static_cast<float>(windowH))
		{
			m_texture = ResourceManager::GetInstance().LoadTexture(imagePath);
		}

		void Render() const override
		{
			if (!SceneManager::GetInstance().IsPaused()) return;
			if (!m_texture) return;

			SDL_Renderer* sdl = Renderer::GetInstance().GetSDLRenderer();
			auto* tex = m_texture->GetSDLTexture();
			SDL_SetTextureAlphaMod(tex, 128);
			SDL_FRect dst{ 0.f, 0.f, m_windowW, m_windowH };
			SDL_RenderTexture(sdl, tex, nullptr, &dst);
		}

	private:
		std::shared_ptr<Texture2D> m_texture;
		float m_windowW;
		float m_windowH;
	};
}