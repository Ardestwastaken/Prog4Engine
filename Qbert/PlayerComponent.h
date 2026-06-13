#pragma once
#include "Component.h"
#include "Observer.h"
#include "PlayerState.h"
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace dae
{
	class GridComponent;
	class TextureAtlasComponent;
	class Scene;

	inline constexpr EventId EVT_PLAYER_DIED         = make_sdbm_hash("PlayerDied");
	inline constexpr EventId EVT_PLAYER_SCORE        = make_sdbm_hash("PlayerScore");
	inline constexpr EventId EVT_PLAYER_STATE_CHANGE = make_sdbm_hash("PlayerStateChange");
	inline constexpr EventId EVT_PLAYER_TILE_COLORED = make_sdbm_hash("PlayerTileColored");
	inline constexpr EventId EVT_ALL_TILES_COLORED   = make_sdbm_hash("AllTilesColored");

	class PlayerComponent final : public Component, public Subject
	{
	public:
		PlayerComponent(GameObject* go,
			GridComponent* grid,
			Scene* scene,
			float spriteW, float spriteH,
			int startRow = 0, int startCol = 0,
			int maxLives = 3);

		~PlayerComponent() override = default;

		void Update() override;

		void TransitionTo(PlayerState* newState);

		GameObject* GetGO() const { return GetGameObject(); }

		bool TryJump(int dRow, int dCol);
		void CommitJump(int newRow, int newCol);
		void CommitOffGridJump();
		void Respawn();
		void ResetPosition();

		int GetRow() const { return m_row; }
		int GetCol() const { return m_col; }
		int GetLastSafeRow() const { return m_lastSafeRow; }
		int GetLastSafeCol() const { return m_lastSafeCol; }

		const glm::vec3& GetJumpTarget() const { return m_jumpTarget; }

		int  GetPendingRow() const { return m_pendingRow; }
		int  GetPendingCol() const { return m_pendingCol; }
		bool IsJumpOffGrid() const { return m_jumpOffGrid; }

		void Kill();
		void AddScore(int pts);

		int GetLives() const { return m_lives; }
		int GetScore() const { return m_score; }

		void JumpUpLeft();
		void JumpUpRight();
		void JumpDownLeft();
		void JumpDownRight();
		void Die();

		void SpawnCurses(glm::vec2 worldPos);
		void DespawnCurses();

		void SetFacingFrame(int col);

		float GetSpriteW() const { return m_spriteW; }
		float GetSpriteH() const { return m_spriteH; }

		Scene* GetScene() const { return m_pScene; }

		struct BufferedInput
		{
			int   dRow{ 0 };
			int   dCol{ 0 };
			bool  active{ false };
			float ttl{ 0.f };
		};

		void BufferJump(int dRow, int dCol);
		void ConsumeBuffer();
		void TickBuffer(float dt);
		bool HasBufferedJump() const { return m_inputBuffer.active; }
		BufferedInput GetInputBuffer() const { return m_inputBuffer; }

		static constexpr float JumpDuration = 0.25f;
		static constexpr float BufferWindow  = 0.2f;

	private:
		std::unique_ptr<PlayerState> m_pState;

		GridComponent* m_pGrid;
		Scene*         m_pScene;
		GameObject*    m_pCursesObject{ nullptr };

		float m_spriteW;
		float m_spriteH;

		int m_row{};
		int m_col{};
		int m_pendingRow{};
		int m_pendingCol{};
		int m_startRow{};
		int m_startCol{};
		int m_lastSafeRow{};
		int m_lastSafeCol{};

		bool m_jumpOffGrid{ false };

		glm::vec3 m_jumpTarget{};

		int m_lives{};
		int m_score{};

		BufferedInput m_inputBuffer{};
	};
}
