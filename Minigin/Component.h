#pragma once

namespace dae
{
	class GameObject;

	class Component
	{
	public:
		Component(GameObject* pGameObject) : m_pGameObject(pGameObject) {}

		virtual ~Component() = default;
		Component(const Component& other) = default;
		Component(Component&& other) = default;
		Component& operator=(const Component& other) = default;
		Component& operator=(Component&& other) = default;

		virtual void Update() {}
		virtual void Render() const {}

	protected:
		virtual GameObject* GetGameObject() const { return m_pGameObject; }

	private:
		GameObject* m_pGameObject;
	};
}