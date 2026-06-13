#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include "glm/vec2.hpp"

namespace dae
{
	class Minigin final
	{
		static inline bool m_quit{};
	public:
		explicit Minigin(const std::filesystem::path& dataPath, const glm::ivec2& dimensions);
		~Minigin();
		void Run(const std::function<void()>& load);
		void RunOneFrame();

		static void RequestQuit() { m_quit = true; }

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

	private:
		float m_lag{ 0.f };

	};
}