#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Command.h"
#include <algorithm>

namespace dae
{
	InputManager::InputManager()
	{
		for (unsigned int i = 0; i < k_MaxControllers; ++i)
			m_pControllers[i] = std::make_unique<Controller>(i);

		m_pCurrentKeyboardState = SDL_GetKeyboardState(&m_KeyCount);
		m_PreviousKeyboardState.assign(m_pCurrentKeyboardState,
			m_pCurrentKeyboardState + m_KeyCount);
	}

	InputManager::~InputManager() = default;

	bool InputManager::ProcessInput()
	{
		m_PreviousKeyboardState.assign(m_pCurrentKeyboardState,
			m_pCurrentKeyboardState + m_KeyCount);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_EVENT_QUIT)
				return false;

			ImGui_ImplSDL3_ProcessEvent(&e);
		}

		for (auto& controller : m_pControllers)
			controller->Update();

		//KEYBOARD
		for (auto& binding : m_KeyboardBindings)
		{
			const bool curDown  = m_pCurrentKeyboardState[binding.key] != 0;
			const bool prevDown = m_PreviousKeyboardState[binding.key] != 0;

			bool fire = false;
			switch (binding.keyState)
			{
			case KeyState::Down:    
				fire = curDown && !prevDown;  
				break;
			case KeyState::Up:      
				fire = !curDown && prevDown; 
				break;
			case KeyState::Pressed: 
				fire = curDown;               
				break;
			}
			if (fire) binding.command->Execute();
		}

		//CONTROLLER
		for (auto& binding : m_ControllerBindings)
		{
			if (binding.controllerIndex >= k_MaxControllers) continue;
			const auto& ctrl = *m_pControllers[binding.controllerIndex];

			bool fire = false;
			switch (binding.keyState)
			{
			case KeyState::Down:    fire = ctrl.IsDownThisFrame(binding.button); break;
			case KeyState::Up:      fire = ctrl.IsUpThisFrame(binding.button);   break;
			case KeyState::Pressed: fire = ctrl.IsPressed(binding.button);       break;
			}
			if (fire) binding.command->Execute();
		}

		return true;
	}

	int InputManager::BindControllerCommand(
		unsigned int controllerIndex,
		Controller::Button button, KeyState keyState,
		std::unique_ptr<Command> command)
	{
		int id = m_NextBindingId++;
		m_ControllerBindings.push_back({ id, controllerIndex, button, keyState, std::move(command) });
		return id;
	}

	void InputManager::UnbindControllerCommand(int bindingId)
	{
		m_ControllerBindings.erase(
			std::remove_if(m_ControllerBindings.begin(), m_ControllerBindings.end(),
				[bindingId](const ControllerBinding& b) { return b.id == bindingId; }),
			m_ControllerBindings.end());
	}

	int InputManager::BindKeyboardCommand(SDL_Scancode key, KeyState keyState,
		std::unique_ptr<Command> command)
	{
		int id = m_NextBindingId++;
		m_KeyboardBindings.push_back({ id, key, keyState, std::move(command) });
		return id;
	}

	void InputManager::UnbindKeyboardCommand(int bindingId)
	{
		m_KeyboardBindings.erase(
			std::remove_if(m_KeyboardBindings.begin(), m_KeyboardBindings.end(),
				[bindingId](const KeyboardBinding& b) { return b.id == bindingId; }),
			m_KeyboardBindings.end());
	}
}