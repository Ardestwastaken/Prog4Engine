#pragma once

namespace dae
{
	class GameObject;

	class Component
	{
	public:
		virtual ~Component() = default;

		Component(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(const Component&) = delete;
		Component& operator=(Component&&) = delete;

		virtual void Update() {}
		virtual void Render() const {}
		virtual void RenderGui() const {}

	protected:
		
		explicit Component(GameObject* pGameObject) : m_pGameObject(pGameObject) {}

		GameObject* GetGameObject() const { return m_pGameObject; }

	private:
		GameObject* m_pGameObject;
	};
}