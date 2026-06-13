#pragma once
#include "Command.h"
#include "PlayerComponent.h"

namespace dae
{
	class PlayerCommand : public GameActorCommand
	{
	public:
		explicit PlayerCommand(GameObject* go) : GameActorCommand(go) {}
	protected:
		PlayerComponent* GetPlayer() const
		{
			return GetGameObject() ? GetGameObject()->GetComponent<PlayerComponent>() : nullptr;
		}
	};

	class JumpUpLeftCommand final : public PlayerCommand
	{
	public:
		explicit JumpUpLeftCommand(GameObject* go) : PlayerCommand(go) {}
		void Execute() override { if (auto* p = GetPlayer()) p->JumpUpLeft(); }
	};

	class JumpUpRightCommand final : public PlayerCommand
	{
	public:
		explicit JumpUpRightCommand(GameObject* go) : PlayerCommand(go) {}
		void Execute() override { if (auto* p = GetPlayer()) p->JumpUpRight(); }
	};

	class JumpDownLeftCommand final : public PlayerCommand
	{
	public:
		explicit JumpDownLeftCommand(GameObject* go) : PlayerCommand(go) {}
		void Execute() override { if (auto* p = GetPlayer()) p->JumpDownLeft(); }
	};

	class JumpDownRightCommand final : public PlayerCommand
	{
	public:
		explicit JumpDownRightCommand(GameObject* go) : PlayerCommand(go) {}
		void Execute() override { if (auto* p = GetPlayer()) p->JumpDownRight(); }
	};

	class DieCommand final : public PlayerCommand
	{
	public:
		explicit DieCommand(GameObject* go) : PlayerCommand(go) {}
		void Execute() override { if (auto* p = GetPlayer()) p->Die(); }
	};
}
