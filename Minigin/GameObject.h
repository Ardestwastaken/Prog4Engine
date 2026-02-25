#pragma once
#include <string>
#include <memory>
#include <vector>
#include <glm/vec3.hpp>
#include "Component.h"

namespace dae
{
	class Texture2D;

	class GameObject final
	{
	public:
		GameObject() = default;
		~GameObject();

		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = delete;

		void Update();
		void Render() const;

		void SetParent(GameObject* parent, bool keepWorldPosition = true);

		GameObject* GetParent()   const { return m_parent; }
		const std::vector<GameObject*>& GetChildren() const { return m_childrenView; }

		void SetLocalPosition(float x, float y, float z = 0.f);
		void SetLocalPosition(const glm::vec3& pos);

		const glm::vec3& GetLocalPosition()  const { return m_localPosition; }
		const glm::vec3& GetWorldPosition(); 

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value,
				"T must inherit from Component");

			auto pComponent = std::make_unique<T>(this, std::forward<Args>(args)...);
			T* ptr = pComponent.get();
			m_pComponents.push_back(std::move(pComponent));
			return ptr;
		}

		template<typename T>
		void RemoveComponent()
		{
			for (auto it = m_pComponents.begin(); it != m_pComponents.end(); ++it)
			{
				if (dynamic_cast<T*>(it->get()))
				{
					m_pComponents.erase(it);
					return;
				}
			}
		}

		template<typename T>
		T* GetComponent() const
		{
			for (const auto& pComp : m_pComponents)
			{
				if (auto* c = dynamic_cast<T*>(pComp.get()))
					return c;
			}
			return nullptr;
		}

		template<typename T>
		bool HasComponent() const { return GetComponent<T>() != nullptr; }

	private:
		GameObject* m_parent{ nullptr };

		std::vector<std::unique_ptr<GameObject>> m_children{};
		std::vector<GameObject*> m_childrenView{};

		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child);
		bool IsChild(const GameObject* potentialChild) const;

		glm::vec3 m_localPosition{ 0, 0, 0 };
		glm::vec3 m_worldPosition{ 0, 0, 0 }; 
		bool m_positionIsDirty{ true };  

		void SetPositionDirty();
		void UpdateWorldPosition();

		std::vector<std::unique_ptr<Component>> m_pComponents{};
	};
}