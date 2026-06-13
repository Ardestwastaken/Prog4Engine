#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "TextObject.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Font.h"
#include "Texture2D.h"

namespace dae
{

	TextObject::TextObject(GameObject* gameObject,
		const std::string& text,
		std::shared_ptr<Font> font,
		bool centred,
		const SDL_Color& color)
		: Component(gameObject)
		, m_needsUpdate(true)
		, m_IsCentred{ centred }
		, m_text(text)
		, m_color(color)
		, m_font(std::move(font))
		, m_textTexture(nullptr)
	{
	}

	void TextObject::Update()
	{
		if (m_needsUpdate)
		{
			const auto surf = TTF_RenderText_Blended(
				m_font->GetFont(), m_text.c_str(), m_text.length(), m_color);

			if (!surf)
				throw std::runtime_error(std::string("Render text failed: ") + SDL_GetError());

			auto* sdlTex = SDL_CreateTextureFromSurface(
				Renderer::GetInstance().GetSDLRenderer(), surf);

			if (!sdlTex)
				throw std::runtime_error(
					std::string("Create text texture from surface failed: ") + SDL_GetError());

			SDL_DestroySurface(surf);
			m_textTexture = std::make_shared<Texture2D>(sdlTex);
			m_needsUpdate = false;
		}
	}

	void TextObject::Render() const
	{
		if (m_textTexture)
		{
			const auto& pos = GetGameObject()->GetWorldPosition();
			//width set later
			SDL_FRect dst{ pos.x, pos.y, 0, 0 };
			//width
			SDL_GetTextureSize(m_textTexture->GetSDLTexture(), &dst.w, &dst.h);
			//rendertime
			if (m_IsCentred) {
				dst.x -= dst.w / 2.f;
				dst.y -= dst.h / 2.f;

			}
			SDL_RenderTexture(Renderer::GetInstance().GetSDLRenderer(), m_textTexture->GetSDLTexture(), nullptr, &dst);
		}
	}

	void TextObject::SetText(const std::string& text)
	{
		m_text = text;
		m_needsUpdate = true;
	}

	void TextObject::SetColor(const SDL_Color& color)
	{
		m_color = color;
		m_needsUpdate = true;
	}

}