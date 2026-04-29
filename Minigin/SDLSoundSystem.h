#pragma once
#include "SoundSystem.h"
#include <memory>

namespace dae
{
	// Concrete sound system using SDL_mixer on a background thread.
	// SDL_mixer is completely hidden behind a Pimpl - engine users never see it.
	class SDLSoundSystem final : public SoundSystem
	{
	public:
		SDLSoundSystem();
		~SDLSoundSystem() override;

		void Play(sound_id id, float volume) override;
		void RegisterSound(sound_id id, const std::string& filePath) override;

		SDLSoundSystem(const SDLSoundSystem&) = delete;
		SDLSoundSystem(SDLSoundSystem&&) = delete;
		SDLSoundSystem& operator=(const SDLSoundSystem&) = delete;
		SDLSoundSystem& operator=(SDLSoundSystem&&) = delete;

	private:
		struct Impl;          // forward-declared Pimpl struct
		std::unique_ptr<Impl> m_pImpl;
	};
}
