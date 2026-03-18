#pragma once
#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)

namespace dae
{
	enum class AchievementId
	{
		WIN_ONE_GAME,
	};

	struct AchievementDef
	{
		AchievementId id;
		const char* apiName;
		bool          achieved{ false };
	};

	class SteamAchievements final
	{
	public:
		static SteamAchievements& GetInstance()
		{
			static SteamAchievements instance;
			return instance;
		}

		bool Init()
		{
			m_statsValid = SteamUserStats() != nullptr;
			return m_statsValid;
		}

		void Shutdown()
		{
			SteamAPI_Shutdown();
		}

		void Update()
		{
			SteamAPI_RunCallbacks();
		}

		void CheckScore(int score)
		{
			if (score >= 500)
				UnlockAchievement(AchievementId::WIN_ONE_GAME);
		}

		void UnlockAchievement(AchievementId id)
		{
			if (!m_statsValid) return;
			for (auto& ach : m_achievements)
			{
				if (ach.id == id && !ach.achieved)
				{
					ach.achieved = true;
					SteamUserStats()->SetAchievement(ach.apiName);
					SteamUserStats()->StoreStats();
				}
			}
		}

	private:
		void OnUserStatsReceived(UserStatsReceived_t* pCallback)
		{
			if (pCallback->m_nGameID == 480 && pCallback->m_eResult == k_EResultOK)
				m_statsValid = true;
		}

		SteamAchievements()
			: m_callbackUserStatsReceived(this, &SteamAchievements::OnUserStatsReceived)
		{
		}

		bool m_statsValid{ false };

		AchievementDef m_achievements[1]
		{
			{ AchievementId::WIN_ONE_GAME, "ACH_WIN_ONE_GAME" },
		};

		CCallback<SteamAchievements, UserStatsReceived_t>
			m_callbackUserStatsReceived;
	};
}
#endif