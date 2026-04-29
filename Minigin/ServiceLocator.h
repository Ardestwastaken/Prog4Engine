#pragma once
#include "SoundSystem.h"
#include <memory>

namespace dae
{
	// Static service locator for the sound system.
	// Defaults to a NullSoundSystem so callers never need to null-check.
	class ServiceLocator final
	{
	public:
		static SoundSystem& GetSoundSystem();
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem> ss);

	private:
		static std::unique_ptr<SoundSystem> m_soundSystem;
	};
}
