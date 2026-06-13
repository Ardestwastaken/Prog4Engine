#include "GridComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Timer.h"
#include <SDL3/SDL.h>

namespace dae
{
	GridComponent::GridComponent(GameObject* go,
		glm::vec2 topCenter,
		float tileW, float tileH,
		int numRows,
		int startCubeType,
		int numCubeTypes,
		int requiredState,
		ColorMode colorMode)
		: Component(go)
		, m_topCenter(topCenter)
		, m_tileW(tileW)
		, m_tileH(tileH)
		, m_numRows(numRows)
		, m_startCubeType(startCubeType)
		, m_numCubeTypes(numCubeTypes)
		, m_cubeType(startCubeType)
		, m_requiredState(requiredState)
		, m_colorMode(colorMode)
	{
		const int totalTiles = numRows * (numRows + 1) / 2;
		m_tileStates.assign(totalTiles, 0);

		m_texture = ResourceManager::GetInstance().LoadTexture("qbert/Qbert_Cubes.png");
		SDL_SetTextureScaleMode(m_texture->GetSDLTexture(), SDL_SCALEMODE_NEAREST);
		glm::vec2 size = m_texture->GetSize();
		m_atlasFrameW = size.x / 6.f;
		m_atlasFrameH = size.y / 3.f;
	}

	glm::vec2 GridComponent::TileWorldPos(int row, int col) const
	{
		return {
			m_topCenter.x + (col - row * 0.5f) * m_tileW,
			m_topCenter.y + row * m_tileH
		};
	}

	bool GridComponent::IsValid(int row, int col) const
	{
		return row >= 0 && row < m_numRows && col >= 0 && col <= row;
	}

	int GridComponent::TileIndex(int row, int col) const
	{
		return row * (row + 1) / 2 + col;
	}

	bool GridComponent::IsTileColored(int row, int col) const
	{
		if (!IsValid(row, col)) return false;
		return m_tileStates[TileIndex(row, col)] >= 1;
	}

	void GridComponent::ColorTile(int row, int col)
	{
		if (!IsValid(row, col) || m_state != State::Playing) return;
		int& state = m_tileStates[TileIndex(row, col)];

		if (m_colorMode == ColorMode::Normal)
		{
			if (state < m_requiredState) ++state;
		}
		else
		{
			state = (state == 0) ? 1 : (state == 1 ? 2 : 1);
		}
	}

	int GridComponent::GetTileState(int row, int col) const
	{
		if (!IsValid(row, col)) return 0;
		return m_tileStates[TileIndex(row, col)];
	}

	bool GridComponent::AllTilesColored() const
	{
		for (int s : m_tileStates)
			if (s < m_requiredState) return false;
		return true;
	}

	void GridComponent::ResetToNextCubeType()
	{
		++m_cubeType;
		m_tileStates.assign(static_cast<int>(m_tileStates.size()), 0);
		m_state = State::Playing;
	}

	void GridComponent::StartCelebration(float duration, float flashInterval,
		std::function<void()> onComplete)
	{
		m_state = State::Celebrating;
		m_celebElapsed = 0.f;
		m_celebDuration = duration;
		m_celebFlashInterval = flashInterval;
		m_celebFlashTimer = 0.f;
		m_celebDisplayState = 0;
		m_onCelebComplete = std::move(onComplete);
	}

	void GridComponent::Update()
	{
		if (m_state != State::Celebrating) return;

		const float dt = Time::GetDeltaTime();
		m_celebElapsed += dt;
		m_celebFlashTimer += dt;

		if (m_celebFlashTimer >= m_celebFlashInterval)
		{
			m_celebFlashTimer -= m_celebFlashInterval;
			m_celebDisplayState = (m_celebDisplayState + 1) % 3;
		}

		if (m_celebElapsed >= m_celebDuration)
		{
			m_state = State::Playing;
			if (m_onCelebComplete)
			{
				auto cb = std::move(m_onCelebComplete);
				cb();
			}
		}
	}

	void GridComponent::Render() const
	{
		SDL_Renderer* sdl = Renderer::GetInstance().GetSDLRenderer();
		if (!sdl || !m_texture) return;

		SDL_Texture* tex = m_texture->GetSDLTexture();

		const float spriteW = m_tileW;
		const float spriteH = m_tileW;
		const float halfSpriteW = spriteW * 0.5f;
		const float topFaceOffset = spriteH * 0.25f;

		for (int r = 0; r < m_numRows; ++r)
		{
			for (int c = 0; c <= r; ++c)
			{
				auto p = TileWorldPos(r, c);
				int  state = (m_state == State::Celebrating)
					? m_celebDisplayState
					: m_tileStates[TileIndex(r, c)];

				SDL_FRect src{
					m_cubeType * m_atlasFrameW,
					state * m_atlasFrameH,
					m_atlasFrameW,
					m_atlasFrameH
				};
				SDL_FRect dst{
					p.x - halfSpriteW,
					p.y - topFaceOffset,
					spriteW,
					spriteH
				};
				SDL_RenderTexture(sdl, tex, &src, &dst);
			}
		}
	}
}