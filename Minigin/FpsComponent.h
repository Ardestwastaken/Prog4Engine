#pragma once
#include "Component.h"

namespace dae
{
	class TextObject;

	class FpsComponent final : public Component
	{
	public:
		explicit FpsComponent(GameObject* gameObject);
		~FpsComponent() override = default;

		FpsComponent(const FpsComponent&) = delete;
		FpsComponent(FpsComponent&&) = delete;
		FpsComponent& operator=(const FpsComponent&) = delete;
		FpsComponent& operator=(FpsComponent&&) = delete;

		void Update() override;

	private:
		TextObject* m_pTextObject{ nullptr };
		float m_updateTimer{ 0.f };
		int   m_frameCount{ 0 };
		float m_frameTime{ 1.f };
	};
}