#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "Scene.h"
#include "Singleton.h"

namespace dae
{
	class Scene;

	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();

		void RegisterScene(int index, std::function<void()> loadFunc);
		void LoadScene(int index, float freezeDelay = 0.f);

		bool IsUnloading() const { return m_isUnloading; }

		void SetPaused(bool paused) { m_paused = paused; }
		bool IsPaused()       const { return m_paused; }

		void FixedUpdate();
		void Update();
		void Render();

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;

		std::vector<std::unique_ptr<Scene>> m_scenes{};
		std::vector<std::function<void()>>  m_loadFunctions{};

		int   m_pendingLoad{ -1 };
		float m_freezeTimer{ 0.f };
		bool  m_isUnloading{ false };
		bool  m_paused{ false };
	};
}
