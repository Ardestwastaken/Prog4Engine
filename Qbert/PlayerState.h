#pragma once

namespace dae
{
	class PlayerComponent;

	class PlayerState
	{
	public:
		virtual ~PlayerState() = default;

		virtual void OnEnter(PlayerComponent& player) = 0;
		virtual void OnExit(PlayerComponent& player)  = 0;
		virtual PlayerState* Update(PlayerComponent& player, float dt) = 0;

		virtual PlayerState* OnJumpUpLeft(PlayerComponent& p)    { (void)p; return nullptr; }
		virtual PlayerState* OnJumpUpRight(PlayerComponent& p)   { (void)p; return nullptr; }
		virtual PlayerState* OnJumpDownLeft(PlayerComponent& p)  { (void)p; return nullptr; }
		virtual PlayerState* OnJumpDownRight(PlayerComponent& p) { (void)p; return nullptr; }
		virtual PlayerState* OnDie(PlayerComponent& p)           { (void)p; return nullptr; }

	protected:
		PlayerState() = default;
	};
}
