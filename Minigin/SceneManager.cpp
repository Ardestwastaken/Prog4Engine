#include "SceneManager.h"
#include "Scene.h"
#include "InputManager.h"
#include "Timer.h"

void dae::SceneManager::FixedUpdate()
{
	if (m_paused) return;
	if (m_freezeTimer > 0.f || m_pendingLoad >= 0) return;

	for (auto& scene : m_scenes)
		scene->FixedUpdate();
}

void dae::SceneManager::Update()
{
	if (m_pendingLoad >= 0)
	{
		if (m_freezeTimer > 0.f)
		{
			m_freezeTimer -= Time::GetDeltaTime();
			return;
		}

		int idx = m_pendingLoad;
		m_pendingLoad = -1;
		m_paused      = false;

		m_isUnloading = true;
		m_scenes.clear();
		m_isUnloading = false;

		InputManager::GetInstance().UnbindAllCommands();

		if (idx < static_cast<int>(m_loadFunctions.size()))
			m_loadFunctions[idx]();

		return;
	}

	if (m_paused) return;

	for (auto& scene : m_scenes)
		scene->Update();
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
		scene->Render();
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}

void dae::SceneManager::RegisterScene(int index, std::function<void()> loadFunc)
{
	if (index >= static_cast<int>(m_loadFunctions.size()))
		m_loadFunctions.resize(index + 1);
	m_loadFunctions[index] = std::move(loadFunc);
}

void dae::SceneManager::LoadScene(int index, float freezeDelay)
{
	m_pendingLoad = index;
	m_freezeTimer = freezeDelay;
}
