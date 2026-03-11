#include "Controller.h"
#include <SDL3/SDL.h>

namespace dae
{
	class Controller::ControllerImpl
	{
	public:
		explicit ControllerImpl(unsigned int controllerIndex)
			: m_ControllerIndex(controllerIndex)
		{
			TryOpen();
		}

		~ControllerImpl()
		{
			if (m_pGamepad)
				SDL_CloseGamepad(m_pGamepad);
		}

		void TryOpen()
		{
			int count = 0;
			SDL_JoystickID* ids = SDL_GetGamepads(&count);
			if (ids && m_ControllerIndex < static_cast<unsigned int>(count))
			{
				m_pGamepad = SDL_OpenGamepad(ids[m_ControllerIndex]);
			}
			if (ids) SDL_free(ids);
		}

		void Update()
		{
			if (!m_pGamepad) { TryOpen(); return; }

			m_ButtonsPressedThisFrame = 0;
			m_ButtonsReleasedThisFrame = 0;

			static const struct { unsigned int bit; SDL_GamepadButton sdlBtn; } k_Map[] =
			{
				{ 0x0001, SDL_GAMEPAD_BUTTON_DPAD_UP        },
				{ 0x0002, SDL_GAMEPAD_BUTTON_DPAD_DOWN      },
				{ 0x0004, SDL_GAMEPAD_BUTTON_DPAD_LEFT      },
				{ 0x0008, SDL_GAMEPAD_BUTTON_DPAD_RIGHT     },
				{ 0x0010, SDL_GAMEPAD_BUTTON_START          },
				{ 0x0020, SDL_GAMEPAD_BUTTON_BACK           },
				{ 0x0040, SDL_GAMEPAD_BUTTON_LEFT_STICK     },
				{ 0x0080, SDL_GAMEPAD_BUTTON_RIGHT_STICK    },
				{ 0x0100, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER  },
				{ 0x0200, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER },
				{ 0x1000, SDL_GAMEPAD_BUTTON_SOUTH          }, // A
				{ 0x2000, SDL_GAMEPAD_BUTTON_EAST           }, // B
				{ 0x4000, SDL_GAMEPAD_BUTTON_WEST           }, // X
				{ 0x8000, SDL_GAMEPAD_BUTTON_NORTH          }, // Y
			};

			unsigned int current = 0;
			for (auto& entry : k_Map)
			{
				if (SDL_GetGamepadButton(m_pGamepad, entry.sdlBtn))
					current |= entry.bit;
			}

			unsigned int changes = current ^ m_PreviousButtons;
			m_ButtonsPressedThisFrame = changes & current;
			m_ButtonsReleasedThisFrame = changes & (~current);
			m_PreviousButtons = current;
			m_CurrentButtons = current;
		}

		bool IsDownThisFrame(unsigned int button) const { return m_ButtonsPressedThisFrame & button; }
		bool IsUpThisFrame(unsigned int button) const { return m_ButtonsReleasedThisFrame & button; }
		bool IsPressed(unsigned int button) const { return m_CurrentButtons & button; }

	private:
		unsigned int  m_ControllerIndex{};
		SDL_Gamepad* m_pGamepad{ nullptr };
		unsigned int  m_CurrentButtons{ 0 };
		unsigned int  m_PreviousButtons{ 0 };
		unsigned int  m_ButtonsPressedThisFrame{ 0 };
		unsigned int  m_ButtonsReleasedThisFrame{ 0 };
	};

	Controller::Controller(unsigned int controllerIndex)
		: m_pImpl(std::make_unique<ControllerImpl>(controllerIndex))
	{
	}

	Controller::~Controller() = default;

	void Controller::Update() { m_pImpl->Update(); }
	bool Controller::IsDownThisFrame(Button b) const { return m_pImpl->IsDownThisFrame(static_cast<unsigned int>(b)); }
	bool Controller::IsUpThisFrame(Button b) const { return m_pImpl->IsUpThisFrame(static_cast<unsigned int>(b)); }
	bool Controller::IsPressed(Button b) const { return m_pImpl->IsPressed(static_cast<unsigned int>(b)); }
}