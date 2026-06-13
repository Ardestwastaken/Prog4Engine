#pragma once

namespace dae
{
	enum class GameMode { Solo, Coop, Versus };

	struct GameSession
	{
		static GameSession& Get()
		{
			static GameSession instance;
			return instance;
		}

		void Reset()
		{
			lives = 4;
			score = 0;
			mode  = GameMode::Solo;
		}

		int      lives{ 4 };
		int      score{ 0 };
		GameMode mode{ GameMode::Solo };

	private:
		GameSession() = default;
	};
}
