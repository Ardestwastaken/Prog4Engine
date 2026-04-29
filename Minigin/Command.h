#pragma once
#include "GameObject.h"
#include "Timer.h"
#include <glm/vec3.hpp>
#include "PlayerComponent.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"

namespace dae
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;
	};

	class GameObject;

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

	class MoveCommand final : public GameActorCommand
	{
	public:
		MoveCommand(GameObject* pGameObject, const glm::vec3& direction, float speed)
			: GameActorCommand(pGameObject)
			, m_Direction(direction)
			, m_Speed(speed)
		{
		}

		void Execute() override
		{
			auto* go = GetGameObject();
			if (!go) return;

			const float dt = Time::GetInstance().GetDeltaTime();
			const glm::vec3 pos = go->GetLocalPosition();
			go->SetLocalPosition(pos + m_Direction * m_Speed * dt);
		}

	private:
		glm::vec3 m_Direction{};
		float m_Speed{};
	};

	class DieCommand final : public GameActorCommand
	{
	public:
		explicit DieCommand(GameObject* go) : GameActorCommand(go) {}

		void Execute() override
		{
			auto* player = GetGameObject()->GetComponent<PlayerComponent>();
			if (player) player->Die();
		}
	};

	class GainPointsCommand final : public GameActorCommand
	{
	public:
		GainPointsCommand(GameObject* go, int points)
			: GameActorCommand(go), m_points(points) {
		}

		void Execute() override
		{
			auto* player = GetGameObject()->GetComponent<PlayerComponent>();
			if (player) player->AddScore(m_points);
		}

	private:
		int m_points{};
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
