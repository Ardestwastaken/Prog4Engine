#include "GameObject.h"
#include <algorithm>
#include <cassert>

namespace dae
{

	GameObject::~GameObject() = default;

	void GameObject::Update()
	{
		for (auto& pComp : m_pComponents)
			pComp->Update();
	}

	void GameObject::Render() const
	{
		for (auto& pComp : m_pComponents)
		{
			pComp->Render();
			pComp->RenderGui();
		}
	}

	void GameObject::SetParent(GameObject* newParent, bool keepWorldPosition)
	{
		if (newParent == this || newParent == m_parent || IsChild(newParent))
			return;

		if (newParent == nullptr)
		{
			SetLocalPosition(GetWorldPosition());
		}
		else
		{
			if (keepWorldPosition)
			{
				SetLocalPosition(GetWorldPosition() - newParent->GetWorldPosition());
			}
			SetPositionDirty();
		}

		if (m_parent)
			m_parent->RemoveChild(this);

		m_parent = newParent;

		if (m_parent)
			m_parent->AddChild(this);
	}

	void GameObject::AddChild(GameObject* child)
	{
		assert(child != nullptr);
		assert(child != this);
		child->SetPositionDirty();
	}

	void GameObject::RemoveChild(GameObject* child)
	{
		assert(child != nullptr);
		auto it = std::find(m_childrenView.begin(), m_childrenView.end(), child);
		if (it != m_childrenView.end())
			m_childrenView.erase(it);

		child->m_parent = nullptr;
		child->SetPositionDirty();
	}

	bool GameObject::IsChild(const GameObject* candidate) const
	{
		for (const auto* child : m_childrenView)
		{
			if (child == candidate)
				return true;
			if (child->IsChild(candidate))
				return true;
		}
		return false;
	}

	void GameObject::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition({ x, y, z });
	}

	void GameObject::SetLocalPosition(const glm::vec3& pos)
	{
		m_localPosition = pos;
		SetPositionDirty();
	}

	void GameObject::SetPositionDirty()
	{
		m_positionIsDirty = true;
		for (auto* child : m_childrenView)
			child->SetPositionDirty();
	}

	void GameObject::UpdateWorldPosition()
	{
		if (m_positionIsDirty)
		{
			if (m_parent == nullptr)
				m_worldPosition = m_localPosition;
			else
				m_worldPosition = m_parent->GetWorldPosition() + m_localPosition;
		}
		m_positionIsDirty = false;
	}

	const glm::vec3& GameObject::GetWorldPosition()
	{
		if (m_positionIsDirty)
			UpdateWorldPosition();
		return m_worldPosition;
	}

}