#pragma once
#include "Component.h"
#include "CoilyState.h"
#include <memory>
#include <glm/vec3.hpp>

namespace dae
{
	class GridComponent;
	class PlayerComponent;
	class TextureAtlasComponent;

	class CoilyComponent final : public Component
	{
	public:
		CoilyComponent(GameObject* go,
			GridComponent*   grid,
			PlayerComponent* player,
			float spriteW, float spriteH);

		~CoilyComponent() override = default;

		void Update() override;

		void TransitionTo(CoilyState* next);

		GridComponent*   GetGrid()   const { return m_grid; }
		PlayerComponent* GetPlayer() const { return m_player; }

		int GetRow() const { return m_row; }
		int GetCol() const { return m_col; }
		void SetGridPos(int row, int col);

		glm::vec3 GetWorldPos() const;
		void SetWorldPos(glm::vec3 pos);

		void SetAtlasFrame(int col);
		void UpdateAtlasDestRect();

		void EnableCollision() { m_collisionEnabled = true; }
		void Respawn();
		void SetPlayerControlled();

		CoilyState* GetCurrentState() const { return m_state.get(); }

		static constexpr float JumpDuration = 0.75f;
		static constexpr float LandPause    = 0.4f;

	private:
		std::unique_ptr<CoilyState> m_state;

		GridComponent*   m_grid;
		PlayerComponent* m_player;

		int m_row{ 0 };
		int m_col{ 0 };

		float m_spriteW;
		float m_spriteH;
		bool  m_collisionEnabled{ false };
	};
}
