#include "TextureComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"

dae::TextureComponent::TextureComponent(GameObject* gameObject)
	: Component(gameObject),
	m_texture(nullptr)
{
}

void dae::TextureComponent::Render() const
{
	if (m_texture != nullptr)
	{
		const auto& pos = GetGameObject()->GetTransfrom()->GetPosition();
		Renderer::GetInstance().RenderTexture(*m_texture, pos.x, pos.y);
	}
}

void dae::TextureComponent::SetTexture(const std::string& fileName)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(fileName);
}

void dae::TextureComponent::SetPosition(const float x, const float y)
{
	GetGameObject()->GetTransfrom()->SetPosition(x, y);
}