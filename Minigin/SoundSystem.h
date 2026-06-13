#pragma once
#include <string>
#include <memory>

namespace dae
{
	using sound_id = unsigned short;

	class SoundSystem
	{
	public:
		virtual ~SoundSystem() = default;
		virtual void Play(sound_id id, float volume) = 0;
		virtual void RegisterSound(sound_id id, const std::string& filePath) = 0;
		virtual void SetMuted(bool muted) = 0;
		virtual bool IsMuted() const = 0;

		SoundSystem(const SoundSystem&) = delete;
		SoundSystem(SoundSystem&&) = delete;
		SoundSystem& operator=(const SoundSystem&) = delete;
		SoundSystem& operator=(SoundSystem&&) = delete;
	protected:
		SoundSystem() = default;
	};

	class NullSoundSystem final : public SoundSystem
	{
	public:
		void Play(sound_id, float) override {}
		void RegisterSound(sound_id, const std::string&) override {}
		void SetMuted(bool) override {}
		bool IsMuted() const override { return false; }
	};

	class LoggingSoundSystem final : public SoundSystem
	{
	public:
		explicit LoggingSoundSystem(std::unique_ptr<SoundSystem> wrapped);
		~LoggingSoundSystem() override = default;
		void Play(sound_id id, float volume) override;
		void RegisterSound(sound_id id, const std::string& filePath) override;
		void SetMuted(bool muted) override;
		bool IsMuted() const override;
	private:
		std::unique_ptr<SoundSystem> m_wrapped;
	};
}
