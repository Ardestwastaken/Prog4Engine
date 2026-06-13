#include "SoundSystem.h"
#include <iostream>

namespace dae
{
	LoggingSoundSystem::LoggingSoundSystem(std::unique_ptr<SoundSystem> wrapped)
		: m_wrapped(std::move(wrapped)) {}

	void LoggingSoundSystem::Play(sound_id id, float volume)
	{
		if (!IsMuted())
			std::cout << "[Sound] Playing sound id=" << id << " volume=" << volume << "\n";
		m_wrapped->Play(id, volume);
	}

	void LoggingSoundSystem::RegisterSound(sound_id id, const std::string& filePath)
	{
		std::cout << "[Sound] Registering sound id=" << id << " -> " << filePath << "\n";
		m_wrapped->RegisterSound(id, filePath);
	}

	void LoggingSoundSystem::SetMuted(bool muted)
	{
		m_wrapped->SetMuted(muted);
	}

	bool LoggingSoundSystem::IsMuted() const
	{
		return m_wrapped->IsMuted();
	}
}
