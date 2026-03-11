#pragma once
#include "Singleton.h"
#include "Controller.h"
#include <memory>
#include <vector>
#include <SDL3/SDL.h>

namespace dae
{
	class Command;

	enum class KeyState
	{
		Down,
		Up,
		Pressed
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager();
		~InputManager();

		bool ProcessInput();

		//CONTROLLER
		int BindControllerCommand(unsigned int controllerIndex,
			Controller::Button button, KeyState keyState,
			std::unique_ptr<Command> command);

		void UnbindControllerCommand(int bindingId);

		//KEYBOARD
		int BindKeyboardCommand(SDL_Scancode key, KeyState keyState,
			std::unique_ptr<Command> command);

		void UnbindKeyboardCommand(int bindingId);

	private:
		struct ControllerBinding
		{
			int id{};
			unsigned int controllerIndex{};
			Controller::Button button{};
			KeyState keyState{};
			std::unique_ptr<Command> command;
		};

		struct KeyboardBinding
		{
			int id{};
			SDL_Scancode key{};
			KeyState keyState{};
			std::unique_ptr<Command> command;
		};

		static const int k_MaxControllers = 4;
		std::unique_ptr<Controller> m_pControllers[k_MaxControllers];

		std::vector<ControllerBinding> m_ControllerBindings{};
		std::vector<KeyboardBinding>   m_KeyboardBindings{};

		const bool* m_pCurrentKeyboardState{ nullptr };
		std::vector<bool> m_PreviousKeyboardState{};
		int m_KeyCount{ 0 };

		int m_NextBindingId{ 0 };
	};
}