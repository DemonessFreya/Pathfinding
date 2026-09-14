#pragma once

#include <vector>

namespace AIForGames {
	class Agent;
	class Behaviour;

	class Condition
	{
	public:
		virtual bool IsTrue(Agent* agent) = 0;
	};

	class State
	{
	public:
		struct Transition {
			Condition* condition;
			State* targetState;
		};

	private:
		std::vector<Behaviour*> m_behaviours;
		std::vector<Transition> m_transitions;

	public:
		State() : m_behaviours({}), m_transitions({}) {}
		State(Behaviour* behaviour);
		~State();
		virtual void Update(Agent* agent, float deltaTime);
		virtual void Enter(Agent* agent);
		virtual void Exit(Agent* agent);

		void AddTransition(Condition* condition, State* targetState);

		std::vector<Transition> GetTransitions() const { return m_transitions; }
	};

	class FiniteStateMachine : public Behaviour
	{
	private:
		std::vector<State*> m_states;

		State* m_currentState;
		State* m_newState;

	public:
		FiniteStateMachine(State* s) : m_currentState(s), m_newState(nullptr) {}
		virtual ~FiniteStateMachine();

		void Update(Agent* agent, float deltaTime);
		virtual void Enter(Agent* agent) override;
		virtual void Exit(Agent* agent) override;
		void AddState(State* state);
	};

	class DistanceCondition : public Condition
	{
	private:
		float m_distance;
		bool m_lessThan;

	public:
		DistanceCondition(float d, bool lt) : m_distance(d), m_lessThan(lt) {}
		virtual bool IsTrue(Agent* agent);
	};
}
