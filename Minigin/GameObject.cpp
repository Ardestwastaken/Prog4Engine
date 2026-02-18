#include <string>
#include "GameObject.h"
#include "ResourceManager.h"
#include "Renderer.h"

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(){
	
	for (int i{}; i < m_pComponents.size(); ++i) {

		m_pComponents[i]->Update();
	}

}

void dae::GameObject::Render() const
{
	for (int i{}; i < m_pComponents.size(); ++i) {

		m_pComponents[i]->Render();
	}

}

void dae::GameObject::SetTexture(const std::string& filename)
{
	m_texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::GameObject::SetPosition(float x, float y)
{
	m_transform.SetPosition(x, y, 0.0f);
}
