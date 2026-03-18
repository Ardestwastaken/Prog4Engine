#pragma once
#include <vector>
#include <algorithm>

namespace dae
{
	template <int length> struct sdbm_hash
	{
		consteval static unsigned int _calculate(const char* const text, unsigned int& value)
		{
			const unsigned int character = sdbm_hash<length - 1>::_calculate(text, value);
			value = character + (value << 6) + (value << 16) - value;
			return text[length - 1];
		}
		consteval static unsigned int calculate(const char* const text)
		{
			unsigned int value = 0;
			const auto character = _calculate(text, value);
			return character + (value << 6) + (value << 16) - value;
		}
	};
	template <> struct sdbm_hash<1>
	{
		consteval static int _calculate(const char* const text, unsigned int&) { return text[0]; }
	};
	template <size_t N> consteval unsigned int make_sdbm_hash(const char(&text)[N])
	{
		return sdbm_hash<N - 1>::calculate(text);
	}

	using EventId = unsigned int;

	inline constexpr EventId EVT_PLAYER_DIED = make_sdbm_hash("PlayerDied");
	inline constexpr EventId EVT_ENEMY_DIED = make_sdbm_hash("EnemyDied");
	inline constexpr EventId EVT_PICKUP_FOUND = make_sdbm_hash("PickupFound");
	inline constexpr EventId EVT_SCORE_CHANGED = make_sdbm_hash("ScoreChanged");

	class IObserver
	{
	public:
		virtual ~IObserver() = default;
		virtual void OnNotify(EventId eventId, int value = 0) = 0;
	};

	class Subject
	{
	public:
		Subject() = default;
		virtual ~Subject() = default;

		Subject(const Subject&) = delete;
		Subject(Subject&&) = delete;
		Subject& operator=(const Subject&) = delete;
		Subject& operator=(Subject&&) = delete;

		void AddObserver(IObserver* observer)
		{
			m_observers.push_back(observer);
		}

		void RemoveObserver(IObserver* observer)
		{
			m_observers.erase(
				std::remove(m_observers.begin(), m_observers.end(), observer),
				m_observers.end());
		}

	protected:
		void NotifyObservers(EventId eventId, int value = 0)
		{
			for (auto* observer : m_observers)
				observer->OnNotify(eventId, value);
		}

	private:
		std::vector<IObserver*> m_observers{};
	};
}