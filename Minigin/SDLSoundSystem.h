#pragma once
#include "SoundSystem.h"
#include <memory>

namespace dae
{
	class SDLSoundSystem final : public SoundSystem
	{
	public:
		SDLSoundSystem();
		~SDLSoundSystem() override;
		void Play(sound_id id, float volume) override;
		void RegisterSound(sound_id id, const std::string& filePath) override;
		void SetMuted(bool muted) override;
		bool IsMuted() const override;

		SDLSoundSystem(const SDLSoundSystem&) = delete;
		SDLSoundSystem(SDLSoundSystem&&) = delete;
		SDLSoundSystem& operator=(const SDLSoundSystem&) = delete;
		SDLSoundSystem& operator=(SDLSoundSystem&&) = delete;
	private:
		struct Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
}
