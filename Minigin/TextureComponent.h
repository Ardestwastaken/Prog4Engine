#pragma once
#include <string>
#include <memory>
#include "Component.h"

namespace dae
{
	class Texture2D;

	class TextureComponent final : public Component
	{
	public:
		explicit TextureComponent(GameObject* gameObject);
		~TextureComponent() override = default;

		TextureComponent(const TextureComponent&) = delete;
		TextureComponent(TextureComponent&&) = delete;
		TextureComponent& operator=(const TextureComponent&) = delete;
		TextureComponent& operator=(TextureComponent&&) = delete;

		void Render() const override;

		void SetTexture(const std::string& fileName);

		void SetScale(float scale) { m_scale = scale; }
		float GetScale() const { return m_scale; }

	private:
		std::shared_ptr<Texture2D> m_texture{};
		float m_scale{ 1.f };
	};
}