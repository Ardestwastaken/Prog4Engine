#include "ServiceLocator.h"

namespace dae
{
	// Initialise with a NullSoundSystem so it's always valid
	std::unique_ptr<SoundSystem> ServiceLocator::m_soundSystem{
		std::make_unique<NullSoundSystem>()
	};

	SoundSystem& ServiceLocator::GetSoundSystem()
	{
		return *m_soundSystem;
	}

	void ServiceLocator::RegisterSoundSystem(std::unique_ptr<SoundSystem> ss)
	{
		if (ss == nullptr)
			m_soundSystem = std::make_unique<NullSoundSystem>();
		else
			m_soundSystem = std::move(ss);
	}
}
