#include "TextureComponent.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include <SDL3/SDL.h>

namespace dae
{

	TextureComponent::TextureComponent(GameObject* gameObject)
		: Component(gameObject)
		, m_texture(nullptr)
	{
	}

	void TextureComponent::Render() const
	{
		if (m_texture)
		{
			const auto& pos = GetGameObject()->GetWorldPosition();
			const auto  size = m_texture->GetSize();

			SDL_FRect dst{};
			dst.x = pos.x;
			dst.y = pos.y;
			dst.w = size.x * m_scale;
			dst.h = size.y * m_scale;

			SDL_RenderTexture(Renderer::GetInstance().GetSDLRenderer(),
				m_texture->GetSDLTexture(), nullptr, &dst);
		}
	}

	void TextureComponent::SetTexture(const std::string& fileName)
	{
		m_texture = ResourceManager::GetInstance().LoadTexture(fileName);
	}

}