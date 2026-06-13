#pragma once

namespace dae
{
	class CoilyComponent;

	class CoilyState
	{
	public:
		virtual ~CoilyState() = default;
		virtual void OnEnter(CoilyComponent& coily) = 0;
		virtual void OnExit(CoilyComponent& coily)  = 0;
		virtual CoilyState* Update(CoilyComponent& coily, float dt) = 0;
	protected:
		CoilyState() = default;
	};
}
