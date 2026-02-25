#include <sstream>
#include <iomanip>
#include "FpsComponent.h"
#include "GameObject.h"
#include "TextObject.h"
#include "Timer.h"

namespace dae
{

	FpsComponent::FpsComponent(GameObject* gameObject)
		: Component(gameObject)
	{
		m_pTextObject = GetGameObject()->GetComponent<TextObject>();
	}

	void FpsComponent::Update()
	{
		const float deltaTime = Time::GetInstance().GetDeltaTime();

		m_updateTimer += deltaTime;
		m_frameCount++;

		if (m_updateTimer >= m_frameTime)
		{
			const float fps = static_cast<float>(m_frameCount) / m_updateTimer;

			if (m_pTextObject)
			{
				std::stringstream ss;
				ss << std::fixed << std::setprecision(1) << fps << " FPS";
				m_pTextObject->SetText(ss.str());
			}

			m_frameCount = 0;
			m_updateTimer -= m_frameTime;
		}
	}

}