#include "Controller.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>
#pragma comment(lib, "xinput.lib")

namespace dae
{
	class Controller::ControllerImpl
	{
	public:
		explicit ControllerImpl(unsigned int controllerIndex)
			: m_ControllerIndex(controllerIndex)
		{
			ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
			ZeroMemory(&m_PreviousState, sizeof(XINPUT_STATE));
		}

		void Update()
		{
			CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
			ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
			XInputGetState(m_ControllerIndex, &m_CurrentState);

			auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
			m_ButtonsPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
			m_ButtonsReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
		}

		bool IsDownThisFrame(unsigned int button) const
		{
			return m_ButtonsPressedThisFrame & button;
		}

		bool IsUpThisFrame(unsigned int button) const
		{
			return m_ButtonsReleasedThisFrame & button;
		}

		bool IsPressed(unsigned int button) const
		{
			return m_CurrentState.Gamepad.wButtons & button;
		}

	private:
		unsigned int m_ControllerIndex{};
		XINPUT_STATE m_CurrentState{};
		XINPUT_STATE m_PreviousState{};
		WORD m_ButtonsPressedThisFrame{};
		WORD m_ButtonsReleasedThisFrame{};
	};

	Controller::Controller(unsigned int controllerIndex)
		: m_pImpl(std::make_unique<ControllerImpl>(controllerIndex))
	{
	}

	Controller::~Controller() = default;

	void Controller::Update()
	{
		m_pImpl->Update();
	}

	bool Controller::IsDownThisFrame(Button button) const
	{
		return m_pImpl->IsDownThisFrame(static_cast<unsigned int>(button));
	}

	bool Controller::IsUpThisFrame(Button button) const
	{
		return m_pImpl->IsUpThisFrame(static_cast<unsigned int>(button));
	}

	bool Controller::IsPressed(Button button) const
	{
		return m_pImpl->IsPressed(static_cast<unsigned int>(button));
	}
}