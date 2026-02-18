#pragma once
#include <string>
#include <memory>
#include "Transform.h"
#include "Component.h"

namespace dae
{
	class Texture2D;
	class GameObject final
	{
		Transform m_transform{};
		std::shared_ptr<Texture2D> m_texture{};
	public:
		virtual void Update();
		virtual void Render() const;

		void SetTexture(const std::string& filename);
		void SetPosition(float x, float y);
		Transform* GetTransfrom() { return &m_transform; }

		GameObject() = default;
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;


		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of<Component, T>::value, "T does not inherit from Component");

			auto pComponent{ std::make_unique<T>(this, std::forward<Args>(args)...) };

			T* ptr = pComponent.get();
			m_pComponents.push_back(std::move(pComponent));

			return ptr;
		}

		template<typename T>
		void RemoveComponent()
		{
			for (auto i = m_pComponents.begin(); i < m_pComponents.end(); i++)
			{
				if (dynamic_cast<T*>(i->get()))
				{
					m_pComponents.erase(i);
					return;
				}
			}
		}

		template<typename T>
		T* GetComponent() const
		{
			for (const auto& pComp : m_pComponents)
			{
				if (pComp)
				{
					if (auto component = dynamic_cast<T*>(pComp.get()))
					{
						return component;
					}
				}
			}
			return nullptr;
		}

		template<typename T>
		bool HasComponent() const
		{
			return GetComponent<T>() != nullptr;
		}

	private:
		std::vector<std::unique_ptr<Component>> m_pComponents{};
	};
}
