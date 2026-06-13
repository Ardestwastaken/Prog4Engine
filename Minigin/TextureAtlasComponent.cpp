#include "TextureAtlasComponent.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Renderer.h"

namespace dae {
    TextureAtlasComponent::TextureAtlasComponent(GameObject* gameObject, const std::string& texturePath, int cols, int rows)
        : Component(gameObject)
        , m_cols{ cols }
        , m_rows{ rows }
        , m_frameWidth{ 0.f }
        , m_frameHeight{ 0.f }
        , m_frameSourceRect{ }
        , m_frameDestRect{ }
        , m_texture{ }
    {
        m_texture = ResourceManager::GetInstance().LoadTexture(texturePath);
        SDL_SetTextureScaleMode(m_texture->GetSDLTexture(), SDL_SCALEMODE_NEAREST);
        glm::vec2 size = m_texture->GetSize();
        m_frameWidth = size.x / static_cast<float>(m_cols);
        m_frameHeight = size.y / static_cast<float>(m_rows);
        m_frameSourceRect = SDL_FRect{ 0, 0, m_frameWidth, m_frameHeight };
    }

    void TextureAtlasComponent::SetFrame(int col, int row)
    {
        m_frameSourceRect.x = static_cast<float>(col) * m_frameWidth;
        m_frameSourceRect.y = static_cast<float>(row) * m_frameHeight;
    }

    void TextureAtlasComponent::SetDestRect(float x, float y, float w, float h)
    {
        m_frameDestRect = SDL_FRect{ x, y, w, h };
    }

    void TextureAtlasComponent::Render() const
    {
        SDL_RenderTexture(
            Renderer::GetInstance().GetSDLRenderer(),
            m_texture->GetSDLTexture(),
            &m_frameSourceRect,
            &m_frameDestRect
        );
    }
}