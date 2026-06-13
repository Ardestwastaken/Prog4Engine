#pragma once
#ifdef USE_STEAMWORKS
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam_api.h>
#pragma warning(pop)

#include "Observer.h"
#include "Singleton.h"

namespace dae
{
	// Event fired by the game when a condition for an achievement is met.
	// The game should fire these in response to in-game events (e.g. killing
	// an enemy, completing a level), NOT by checking a raw score value.
	inline constexpr EventId EVT_ACHIEVEMENT_TRIGGER = make_sdbm_hash("AchievementTrigger");

	enum class AchievementId
	{
		WIN_ONE_GAME,
	};

	struct AchievementDef
	{
		AchievementId id;
		const char*   apiName;
		bool          achieved{ false };
	};

	// SteamAchievements observes game events and unlocks Steam achievements.
	// Once an achievement is unlocked, it removes itself as an observer so it
	// no longer receives events for that achievement.
	class SteamAchievements final : public Singleton<SteamAchievements>, public IObserver
	{
	public:
		bool Init()
		{
			m_statsValid = SteamUserStats() != nullptr;
			return m_statsValid;
		}

		void Update()
		{
			SteamAPI_RunCallbacks();
		}

		// IObserver: called when a Subject fires an event.
		// value = AchievementId cast to int.
		void OnNotify(EventId eventId, int value) override
		{
			if (eventId == EVT_ACHIEVEMENT_TRIGGER)
				UnlockAchievement(static_cast<AchievementId>(value));
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
					// Achievement is now permanently unlocked; no need to listen anymore.
					// The game should call subject.RemoveObserver(this) after this point.
				}
			}
		}

	private:
		friend class Singleton<SteamAchievements>;

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
