#include "SDLSoundSystem.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>

namespace dae
{
	struct SDLSoundSystem::Impl
	{
		struct SoundEvent
		{
			sound_id id{};
			float    volume{ 1.f };
		};

		MIX_Mixer* m_mixer{ nullptr };

		std::unordered_map<sound_id, std::string>  m_soundPaths;

		struct SoundEntry
		{
			MIX_Audio* audio{ nullptr };
			MIX_Track* track{ nullptr };
		};
		std::unordered_map<sound_id, SoundEntry> m_sounds;

		std::queue<SoundEvent>  m_queue;
		std::mutex              m_queueMutex;
		std::condition_variable m_cv;
		bool                    m_quit{ false };
		std::thread             m_audioThread;

		Impl()
		{
			if (!SDL_Init(SDL_INIT_AUDIO))
			{
				std::cerr << "[SDLSoundSystem] SDL_Init(AUDIO) failed: " << SDL_GetError() << "\n";
				return;
			}

			if (!MIX_Init())
			{
				std::cerr << "[SDLSoundSystem] MIX_Init failed: " << SDL_GetError() << "\n";
				return;
			}

			m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
			if (!m_mixer)
			{
				std::cerr << "[SDLSoundSystem] MIX_CreateMixerDevice failed: " << SDL_GetError() << "\n";
				return;
			}

			m_audioThread = std::thread(&Impl::AudioThreadFunc, this);
		}

		~Impl()
		{
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				m_quit = true;
			}
			m_cv.notify_one();

			if (m_audioThread.joinable())
				m_audioThread.join();

			MIX_Quit();
			m_mixer = nullptr;
		}

		void EnqueuePlay(sound_id id, float volume)
		{
			{
				std::lock_guard<std::mutex> lock(m_queueMutex);
				m_queue.push({ id, volume });
			}
			m_cv.notify_one();
		}

		void AudioThreadFunc()
		{
			while (true)
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_cv.wait(lock, [this] { return !m_queue.empty() || m_quit; });

				if (m_quit && m_queue.empty())
					break;

				// Swap out the whole queue so we release the lock quickly
				std::queue<SoundEvent> local;
				std::swap(local, m_queue);
				lock.unlock();

				while (!local.empty())
				{
					ProcessEvent(local.front());
					local.pop();
				}
			}
		}

		void ProcessEvent(const SoundEvent& ev)
		{
			if (!m_mixer) return;

			// Lazy-load: first time we see this ID, load the audio and create a track
			if (m_sounds.find(ev.id) == m_sounds.end())
			{
				auto pathIt = m_soundPaths.find(ev.id);
				if (pathIt == m_soundPaths.end())
				{
					std::cerr << "[SDLSoundSystem] No path registered for sound id=" << ev.id << "\n";
					return;
				}

				SoundEntry entry{};

				// predecode=true: decode whole file into memory (fast playback, no I/O on play)
				entry.audio = MIX_LoadAudio(m_mixer, pathIt->second.c_str(), true);
				if (!entry.audio)
				{
					std::cerr << "[SDLSoundSystem] MIX_LoadAudio failed for '"
					          << pathIt->second << "': " << SDL_GetError() << "\n";
					m_sounds[ev.id] = {};
					return;
				}

				entry.track = MIX_CreateTrack(m_mixer);
				if (!entry.track)
				{
					std::cerr << "[SDLSoundSystem] MIX_CreateTrack failed: " << SDL_GetError() << "\n";
					MIX_DestroyAudio(entry.audio);
					m_sounds[ev.id] = {};
					return;
				}

				// Bind audio to track (track holds a reference — safe to call MIX_DestroyAudio after)
				MIX_SetTrackAudio(entry.track, entry.audio);
				m_sounds[ev.id] = entry;
			}

			auto& entry = m_sounds[ev.id];
			if (!entry.track || !entry.audio) return;

			// Set gain (volume, where 1.0 = original level) and play
			MIX_SetTrackGain(entry.track, ev.volume);
			MIX_PlayTrack(entry.track, 0); // 0 = default options (play once, no fade)
		}

		// Register path — called from main thread before any Play() calls
		void RegisterPath(sound_id id, const std::string& path)
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_soundPaths[id] = path;
		}
	};

	SDLSoundSystem::SDLSoundSystem()
		: m_pImpl(std::make_unique<Impl>())
	{
	}

	SDLSoundSystem::~SDLSoundSystem() = default;

	void SDLSoundSystem::Play(sound_id id, float volume)
	{
		m_pImpl->EnqueuePlay(id, volume);
	}

	void SDLSoundSystem::RegisterSound(sound_id id, const std::string& filePath)
	{
		m_pImpl->RegisterPath(id, filePath);
	}
}
