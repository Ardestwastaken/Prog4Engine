#pragma once
#include "PlayerState.h"
#include <glm/vec3.hpp>

namespace dae
{
	class IdleState final : public PlayerState
	{
	public:
		void OnEnter(PlayerComponent& player) override;
		void OnExit(PlayerComponent& player)  override;
		PlayerState* Update(PlayerComponent& player, float dt) override;

		PlayerState* OnJumpUpLeft(PlayerComponent& p)    override;
		PlayerState* OnJumpUpRight(PlayerComponent& p)   override;
		PlayerState* OnJumpDownLeft(PlayerComponent& p)  override;
		PlayerState* OnJumpDownRight(PlayerComponent& p) override;
		PlayerState* OnDie(PlayerComponent& p)           override;
	};

	class JumpingState final : public PlayerState
	{
	public:
		int m_dRow{}, m_dCol{};

		void OnEnter(PlayerComponent& player) override;
		void OnExit(PlayerComponent& player)  override;
		PlayerState* Update(PlayerComponent& player, float dt) override;

		PlayerState* OnJumpUpLeft(PlayerComponent& p)    override;
		PlayerState* OnJumpUpRight(PlayerComponent& p)   override;
		PlayerState* OnJumpDownLeft(PlayerComponent& p)  override;
		PlayerState* OnJumpDownRight(PlayerComponent& p) override;
		PlayerState* OnDie(PlayerComponent& p)           override;

	private:
		int       m_pendingRow{}, m_pendingCol{};
		float     m_elapsed{};
		glm::vec3 m_start{}, m_dest{};
	};

	class LandingState final : public PlayerState
	{
	public:
		void OnEnter(PlayerComponent& player) override;
		void OnExit(PlayerComponent& player)  override;
		PlayerState* Update(PlayerComponent& player, float dt) override;

		PlayerState* OnJumpUpLeft(PlayerComponent& p)    override;
		PlayerState* OnJumpUpRight(PlayerComponent& p)   override;
		PlayerState* OnJumpDownLeft(PlayerComponent& p)  override;
		PlayerState* OnJumpDownRight(PlayerComponent& p) override;
		PlayerState* OnDie(PlayerComponent& p)           override;

	private:
		float m_timer{ 0.12f };
	};

	class DeadState final : public PlayerState
	{
	public:
		void OnEnter(PlayerComponent& player) override;
		void OnExit(PlayerComponent& player)  override;
		PlayerState* Update(PlayerComponent& player, float dt) override;

	private:
		float m_timer{ 2.0f };
		bool  m_killed{ false };
	};
}