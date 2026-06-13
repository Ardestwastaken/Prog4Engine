#pragma once
#include "Command.h"
#include "ServiceLocator.h"
#include "SoundSystem.h"

namespace dae
{
	class MuteToggleCommand final : public Command
	{
	public:
		void Execute() override
		{
			auto& ss = ServiceLocator::GetSoundSystem();
			ss.SetMuted(!ss.IsMuted());
		}
	};
}
