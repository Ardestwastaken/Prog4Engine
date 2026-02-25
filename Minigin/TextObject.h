#pragma once
#include <string>
#include <memory>
#include <SDL3/SDL.h> 
#include "Component.h"

namespace dae
{
	class Font;
	class Texture2D;
	class GameObject;

	class TextObject final : public Component
	{
	public:
		TextObject(GameObject* gameObject,
			const std::string& text,
			std::shared_ptr<Font> font,
			const SDL_Color& color = { 255, 255, 255, 255 });

		~TextObject() override = default;

		TextObject(const TextObject&) = delete;
		TextObject(TextObject&&) = delete;
		TextObject& operator=(const TextObject&) = delete;
		TextObject& operator=(TextObject&&) = delete;

		void Update()       override;
		void Render() const override;

		void SetText(const std::string& text);
		void SetColor(const SDL_Color& color);

	private:
		bool                       m_needsUpdate{ true };
		std::string                m_text{};
		SDL_Color                  m_color{ 255, 255, 255, 255 };
		std::shared_ptr<Font>      m_font{};
		std::shared_ptr<Texture2D> m_textTexture{};
	};
}