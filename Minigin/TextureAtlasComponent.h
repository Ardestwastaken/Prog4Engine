#pragma once
#include <string>
#include <memory>
#include "Component.h"
#include <SDL3/SDL.h>

namespace dae {
    class Texture2D;

    class TextureAtlasComponent final : public Component
    {
    public:
        explicit TextureAtlasComponent(GameObject* gameObject, const std::string& texturePath, int cols, int rows);
        ~TextureAtlasComponent() = default;
        TextureAtlasComponent(const TextureAtlasComponent&) = delete;
        TextureAtlasComponent(TextureAtlasComponent&&) = delete;
        TextureAtlasComponent& operator=(const TextureAtlasComponent&) = delete;
        TextureAtlasComponent& operator=(TextureAtlasComponent&&) = delete;

        void SetFrame(int col, int row);
        void SetDestRect(float x, float y, float w, float h);
        void Render() const override;

    private:
        int m_cols;
        int m_rows;
        float m_frameWidth;
        float m_frameHeight;
        SDL_FRect m_frameSourceRect;
        SDL_FRect m_frameDestRect;
        std::shared_ptr<Texture2D> m_texture;
    };
}