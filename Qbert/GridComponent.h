#pragma once
#include "Component.h"
#include <glm/vec2.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace dae
{
	class GridComponent final : public Component
	{
	public:
		enum class ColorMode { Normal, AlternatingAfterFirst };

		GridComponent(GameObject* go,
			glm::vec2 topCenter,
			float tileW, float tileH,
			int numRows,
			int startCubeType = 0,
			int numCubeTypes = 6,
			int requiredState = 1,
			ColorMode colorMode = ColorMode::Normal);

		glm::vec2 TileWorldPos(int row, int col) const;
		bool IsValid(int row, int col) const;

		int   NumRows()        const { return m_numRows; }
		float TileW()          const { return m_tileW; }
		float TileH()          const { return m_tileH; }
		int   RequiredState()  const { return m_requiredState; }
		int   CubeType()       const { return m_cubeType; }
		bool  IsLastCubeType() const { return m_cubeType >= m_startCubeType + m_numCubeTypes - 1; }
		bool  IsCelebrating()  const { return m_state == State::Celebrating; }

		bool IsTileColored(int row, int col) const;
		void ColorTile(int row, int col);
		int  GetTileState(int row, int col) const;
		bool AllTilesColored() const;
		void ResetToNextCubeType();

		void StartCelebration(float duration, float flashInterval,
			std::function<void()> onComplete);

		void Update() override;
		void Render() const override;

	private:
		enum class State { Playing, Celebrating };

		int TileIndex(int row, int col) const;

		glm::vec2 m_topCenter;
		float     m_tileW;
		float     m_tileH;
		int       m_numRows;
		int       m_startCubeType;
		int       m_numCubeTypes;
		int       m_cubeType;
		int       m_requiredState;
		ColorMode m_colorMode;

		State m_state{ State::Playing };
		float m_celebElapsed{ 0.f };
		float m_celebDuration{ 0.f };
		float m_celebFlashInterval{ 0.f };
		float m_celebFlashTimer{ 0.f };
		int   m_celebDisplayState{ 0 };
		std::function<void()> m_onCelebComplete;

		std::vector<int> m_tileStates;

		std::shared_ptr<class Texture2D> m_texture;
		float m_atlasFrameW{ 0.f };
		float m_atlasFrameH{ 0.f };
	};
}