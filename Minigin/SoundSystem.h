#pragma once
#include <string>
#include <memory>

namespace dae
{
	using sound_id = unsigned short;

	// Abstract interface for the sound service
	class SoundSystem
	{
	public:
		virtual ~SoundSystem() = default;

		// Queue a request to play a sound. Non-blocking.
		virtual void Play(sound_id id, float volume) = 0;

		// Register a sound file path for a given id.
		virtual void RegisterSound(sound_id id, const std::string& filePath) = 0;

		SoundSystem(const SoundSystem&) = delete;
		SoundSystem(SoundSystem&&) = delete;
		SoundSystem& operator=(const SoundSystem&) = delete;
		SoundSystem& operator=(SoundSystem&&) = delete;

	protected:
		SoundSystem() = default;
	};

	// Null object: does nothing, safe default
	class NullSoundSystem final : public SoundSystem
	{
	public:
		void Play(sound_id, float) override {}
		void RegisterSound(sound_id, const std::string&) override {}
	};

	// Decorator: logs every play request, then forwards to real system
	class LoggingSoundSystem final : public SoundSystem
	{
	public:
		explicit LoggingSoundSystem(std::unique_ptr<SoundSystem> wrapped);
		~LoggingSoundSystem() override = default;

		void Play(sound_id id, float volume) override;
		void RegisterSound(sound_id id, const std::string& filePath) override;

	private:
		std::unique_ptr<SoundSystem> m_wrapped;
	};
}
