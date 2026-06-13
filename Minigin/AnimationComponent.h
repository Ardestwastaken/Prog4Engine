#pragma once
#include <string>
#include <memory>
#include "Component.h"
#include <SDL3/SDL.h>

namespace dae {

	class Texture2D;

	class AnimationComponent final : public Component
	{

	public:
		explicit AnimationComponent(GameObject* gameObject, const std::string& texturePath, int frameCount, int fps);
		~AnimationComponent() = default;

		AnimationComponent(const AnimationComponent& other) = delete;
		AnimationComponent(AnimationComponent&& other) = delete;
		AnimationComponent& operator=(const AnimationComponent& other) = delete;
		AnimationComponent& operator=(AnimationComponent&& other) = delete;

		void NextFrame();
		void PreviousFrame();
		void GoToFrame(int frame);
		void SetDestRect(float x, float y, float w, float h);
		void Render() const override;
		void Update() override;
		void Start() { m_isPlaying = true; }
		void Stop() { m_isPlaying = false; }


	private:

		void ReloadFrameRect();

		int m_frameCount;
		int m_currentFrame;
		float m_frameTime;
		float m_TimeSinceLastFrame;
		float m_frameWidth;
		bool m_isPlaying;
		SDL_FRect m_frameSourceRect;
		SDL_FRect m_frameDestRect;
		std::shared_ptr<Texture2D> m_texture;
	};

}