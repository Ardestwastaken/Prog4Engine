#pragma once
#include "Singleton.h"
#include <chrono>

namespace dae
{
	class Time final : public Singleton<Time>
	{
	public:
		static void CalculateDeltaTime() { 

			auto& lastTime = GetInstance().m_lastTime;
			auto& frameStart = GetInstance().m_frameStart;
			frameStart = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration<float>(frameStart - lastTime).count();
			lastTime = frameStart;
			GetInstance().m_deltaTime = deltaTime;			
		}
		static float GetDeltaTime() { return GetInstance().m_deltaTime; }
		static auto GetFrameStart() { return GetInstance().m_frameStart; }

		static constexpr float FixedTimeStep{ 0.02f };

	private:
		float m_deltaTime{ 0.f };
		std::chrono::high_resolution_clock::time_point m_lastTime{ std::chrono::high_resolution_clock::now() };
		std::chrono::high_resolution_clock::time_point m_frameStart{};

	};
}
