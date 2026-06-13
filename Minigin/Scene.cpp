#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");
	m_pendingAdd.emplace_back(std::move(object));
}

void Scene::Remove(const GameObject& object)
{
	m_pendingRemove.push_back(&object);
}

void Scene::RemoveAll()
{
	m_pendingAdd.clear();
	m_pendingRemove.clear();
	m_objects.clear();
}

void Scene::FlushPending()
{
	for (const GameObject* toRemove : m_pendingRemove)
	{
		m_objects.erase(
			std::remove_if(m_objects.begin(), m_objects.end(),
				[toRemove](const auto& ptr) { return ptr.get() == toRemove; }),
			m_objects.end());
	}
	m_pendingRemove.clear();

	for (auto& obj : m_pendingAdd)
		m_objects.emplace_back(std::move(obj));
	m_pendingAdd.clear();
}

void Scene::FixedUpdate()
{
	FlushPending();
	for (auto& object : m_objects)
		object->FixedUpdate();
}

void Scene::Update()
{
	FlushPending();
	for (auto& object : m_objects)
		object->Update();
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
		object->Render();
}