#include "SoundSystem.h"
#include <iostream>
#include <memory>

namespace dae
{
	LoggingSoundSystem::LoggingSoundSystem(std::unique_ptr<SoundSystem> wrapped)
		: m_wrapped(std::move(wrapped))
	{
	}

	void LoggingSoundSystem::Play(sound_id id, float volume)
	{
		std::cout << "[Sound] Playing sound id=" << id << " volume=" << volume << "\n";
		m_wrapped->Play(id, volume);
	}

	void LoggingSoundSystem::RegisterSound(sound_id id, const std::string& filePath)
	{
		std::cout << "[Sound] Registering sound id=" << id << " -> " << filePath << "\n";
		m_wrapped->RegisterSound(id, filePath);
	}
}
