#pragma once
#include "Component.h"
#include "Renderer.h"
#include <SDL3/SDL.h>
#include <vector>
#include <glm/vec2.hpp>
#include <algorithm>

namespace dae
{
    class QbertGridComponent final : public Component
    {
    public:
        QbertGridComponent(GameObject* go,
            glm::vec2 topCenter,
            float tileW, float tileH,
            int numRows)
            : Component(go)
            , m_topCenter(topCenter)
            , m_tileW(tileW)
            , m_tileH(tileH)
            , m_numRows(numRows)
        {
        }

        glm::vec2 TileWorldPos(int row, int col) const
        {
            return {
                m_topCenter.x + (col - row * 0.5f) * m_tileW,
                m_topCenter.y + row * m_tileH
            };
        }

        bool IsValid(int row, int col) const
        {
            return row >= 0 && row < m_numRows && col >= 0 && col <= row;
        }

        int  NumRows() const { return m_numRows; }
        float TileW()  const { return m_tileW; }
        
        std::pair<int, int> NearestTile(float wx, float wy) const
        {
            float threshold = (m_tileW * 0.5f) * (m_tileW * 0.5f)
                + (m_tileH * 0.5f) * (m_tileH * 0.5f);
            float best = threshold;
            int br = -1, bc = -1;
            for (int r = 0; r < m_numRows; ++r)
                for (int c = 0; c <= r; ++c)
                {
                    auto p = TileWorldPos(r, c);
                    float dx = wx - p.x, dy = wy - p.y;
                    float d2 = dx * dx + dy * dy;
                    if (d2 < best) { best = d2; br = r; bc = c; }
                }
            return { br, bc };
        }

        void Render() const override
        {
            SDL_Renderer* sdl = Renderer::GetInstance().GetSDLRenderer();
            if (!sdl) return;

            for (int r = 0; r < m_numRows; ++r)
                for (int c = 0; c <= r; ++c)
                {
                    auto p = TileWorldPos(r, c);
                    bool highlight = (r == m_highlightRow && c == m_highlightCol);
                    DrawTile(sdl, p.x, p.y, highlight);
                }
        }

        void SetHighlight(int row, int col)
        {
            m_highlightRow = row;
            m_highlightCol = col;
        }

    private:
        void DrawTile(SDL_Renderer* sdl, float cx, float cy, bool highlighted) const
        {
            const float hw = m_tileW * 0.5f;
            const float hh = m_tileH * 0.5f;

            SDL_FPoint pts[5] = {
                { cx, cy - hh },
                { cx + hw, cy },
                { cx, cy + hh }, 
                { cx - hw, cy },
                { cx, cy - hh }, 
            };

            if (highlighted)
                SDL_SetRenderDrawColor(sdl, 255, 220, 50, 200);
            else
                SDL_SetRenderDrawColor(sdl, 30, 80, 180, 200);

            FillDiamond(sdl, pts);

            SDL_SetRenderDrawColor(sdl, 180, 210, 255, 255);
            SDL_RenderLines(sdl, pts, 5);
        }

        static void FillDiamond(SDL_Renderer* sdl, const SDL_FPoint pts[5])
        {
            FillTriangle(sdl, pts[3], pts[0], pts[1]);
            FillTriangle(sdl, pts[3], pts[2], pts[1]);
        }

        static void FillTriangle(SDL_Renderer* sdl, SDL_FPoint a, SDL_FPoint b, SDL_FPoint c)
        {
            SDL_FPoint v[3] = { a, b, c };
            if (v[1].y < v[0].y) std::swap(v[0], v[1]);
            if (v[2].y < v[0].y) std::swap(v[0], v[2]);
            if (v[2].y < v[1].y) std::swap(v[1], v[2]);

            float totalH = v[2].y - v[0].y;
            if (totalH < 0.5f) return;

            for (float y = v[0].y; y <= v[2].y; y += 1.f)
            {
                float xa = v[0].x + (v[2].x - v[0].x) * (y - v[0].y) / totalH;
                float xb;
                if (y < v[1].y)
                {
                    float segH = v[1].y - v[0].y;
                    xb = (segH < 0.001f) ? v[0].x
                        : v[0].x + (v[1].x - v[0].x) * (y - v[0].y) / segH;
                }
                else
                {
                    float segH = v[2].y - v[1].y;
                    xb = (segH < 0.001f) ? v[1].x
                        : v[1].x + (v[2].x - v[1].x) * (y - v[1].y) / segH;
                }
                if (xa > xb) std::swap(xa, xb);
                SDL_RenderLine(sdl, xa, y, xb, y);
            }
        }

        glm::vec2 m_topCenter;
        float m_tileW;
        float m_tileH;
        int m_numRows;

        int m_highlightRow{ 0 };
        int m_highlightCol{ 0 };
    };
}