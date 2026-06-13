#pragma once
#include "GameObject.h"
#include "Timer.h"
#include <glm/vec3.hpp>
#include "ServiceLocator.h"
#include "SoundSystem.h"
#include "Minigin.h"

namespace dae
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
	};

	class GameObject;

	class QuitGameCommand : public Command
	{
		void Execute() override
		{
			dae::Minigin::RequestQuit();
		}
	};
	class GameActorCommand : public Command
	{
	public:
		explicit GameActorCommand(GameObject* pGameObject) : m_pGameObject(pGameObject) {}
		virtual ~GameActorCommand() = default;

	protected:
		GameObject* GetGameObject() const { return m_pGameObject; }

	private:
		GameObject* m_pGameObject;
	};

	// Command that plays a sound via the ServiceLocator - no direct dependency on SDL_mixer
	class PlaySoundCommand final : public Command
	{
	public:
		PlaySoundCommand(sound_id id, float volume)
			: m_id(id), m_volume(volume) {}

		void Execute() override
		{
			ServiceLocator::GetSoundSystem().Play(m_id, m_volume);
		}

	private:
		sound_id m_id{};
		float    m_volume{ 1.f };
	};
}
