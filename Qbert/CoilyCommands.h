#pragma once
#include "Command.h"
#include "CoilyComponent.h"
#include "CoilyStates.h"

namespace dae
{
	class CoilyJumpCommand final : public Command
	{
	public:
		CoilyJumpCommand(CoilyComponent* coily, int dRow, int dCol)
			: m_coily(coily), m_dRow(dRow), m_dCol(dCol) {}

		void Execute() override
		{
			if (!m_coily) return;
			// Only works when in player-controlled state
			if (auto* state = dynamic_cast<CoilyPlayerControlledState*>(m_coily->GetCurrentState()))
				state->InputJump(m_dRow, m_dCol);
		}

	private:
		CoilyComponent* m_coily;
		int m_dRow, m_dCol;
	};
}
