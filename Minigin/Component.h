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

		// Called at a fixed timestep (physics, AI)
		virtual void FixedUpdate() {}
		// Called every frame
		virtual void Update() {}
		virtual void Render() const {}
		virtual void RenderGui() const {}

	protected:
		explicit Component(GameObject* pGameObject) : m_pGameObject(pGameObject) {}

		GameObject* GetGameObject() const { return m_pGameObject; }

	private:
		// Non-owning pointer back to the owning GameObject
		GameObject* m_pGameObject;
	};
}
