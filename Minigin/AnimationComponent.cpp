#include "AnimationComponent.h"
#include "ResourceManager.h"
#include "Texture2D.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Timer.h"

namespace dae {

	AnimationComponent::AnimationComponent(GameObject* gameObject, const std::string& texturePath, int frameCount, int fps)
		: Component(gameObject)
		, m_frameCount{ frameCount }
		, m_frameTime{ 1.f / static_cast<float>(fps) }
		, m_TimeSinceLastFrame{ 0.f }
		, m_currentFrame{ 0 }
		, m_frameDestRect{ }
		, m_isPlaying{ true }
		, m_frameWidth{ 0.f } //calculated under
		, m_frameSourceRect{ } //calculated under
		, m_texture{} //calculated under
	{
		m_texture = ResourceManager::GetInstance().LoadTexture(texturePath);
		SDL_SetTextureScaleMode(m_texture->GetSDLTexture(), SDL_SCALEMODE_NEAREST);
		glm::vec2 size = m_texture->GetSize();
		m_frameWidth = size.x / static_cast<float>(frameCount);
		m_frameSourceRect = SDL_FRect{ 0,0,m_frameWidth, size.y };
	}

	void AnimationComponent::NextFrame()
	{
		m_currentFrame++;

		if (m_currentFrame > m_frameCount - 1) m_currentFrame = 0;
		ReloadFrameRect();
	}

	void AnimationComponent::PreviousFrame()
	{
		m_currentFrame--;

		if (m_currentFrame < 0) m_currentFrame = m_frameCount - 1;
		ReloadFrameRect();
	}


	void AnimationComponent::Render() const
	{
		SDL_RenderTexture(
			Renderer::GetInstance().GetSDLRenderer(),
			m_texture->GetSDLTexture(), 
			&m_frameSourceRect, 
			&m_frameDestRect
		);
	}

	void AnimationComponent::Update()
	{
		if (!m_isPlaying) return;
		m_TimeSinceLastFrame += Time::GetDeltaTime();
		if (m_TimeSinceLastFrame >= m_frameTime) {
			NextFrame();
			m_TimeSinceLastFrame = 0.f;
		}
	}

	
	void AnimationComponent::GoToFrame(int frame)
	{
		if (frame >= 0 || frame <= m_frameCount - 1) m_currentFrame = frame;
		ReloadFrameRect();
	}

	void AnimationComponent::SetDestRect(float x, float y, float w, float h)
	{
		m_frameDestRect = SDL_FRect{ x,y,w,h };
	}

	void AnimationComponent::ReloadFrameRect()
	{
		m_frameSourceRect.x = static_cast<float>(m_currentFrame) * m_frameWidth;
	}


}