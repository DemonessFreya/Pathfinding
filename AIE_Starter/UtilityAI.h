#pragma once

#include <vector>

namespace AIForGames
{
	class Agent;
	class Behaviour;

	class UtilityAI : public Behaviour
	{
	private:
		std::vector<Behaviour*> m_behaviours;
		Behaviour* currentBehaviour;

	public:
		UtilityAI() : m_behaviours({}), currentBehaviour(nullptr) {}
		~UtilityAI();

		virtual void Update(Agent* agent, float deltaTime);

		void AddBehaviour(Behaviour* behaviour) { m_behaviours.push_back(behaviour); }
	};
}
