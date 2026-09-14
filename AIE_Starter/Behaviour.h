#pragma once

namespace AIForGames
{
    // forward declaration
    class Agent;

    class Behaviour
    {
    public:
        virtual ~Behaviour() {}
        virtual void Update(Agent* agent, float deltaTime) = 0;
		virtual void Enter(Agent* agent) {}
		virtual void Exit(Agent* agent) {}
    };

    class GotoPointBehaviour : public Behaviour
    {
    public:
        virtual void Update(Agent* agent, float deltaTime) override;
    };

    class WanderBehaviour : public Behaviour
    {
    public:
		virtual void Update(Agent* agent, float deltaTime);
		virtual void Enter(Agent* agent) override;
    };

    class FollowBehaviour : public Behaviour
    {
    public:
		FollowBehaviour() : lastTargetPos(0.0f, 0.0f) {}
        virtual void Update(Agent* agent, float deltaTime);
		virtual void Enter(Agent* agent) override;
    private:
        glm::vec2 lastTargetPos;
    };

    class SelectorBehaviour : public Behaviour
    {
    private:
		Behaviour* m_b1;
		Behaviour* m_b2;
		Behaviour* m_selected;
    public:
		SelectorBehaviour(Behaviour* b1, Behaviour* b2) : m_b1(b1), m_b2(b2), m_selected(nullptr) {}
		~SelectorBehaviour() { delete m_b1; delete m_b2; }

        virtual void Update(Agent* agent, float deltaTime);

        void SetBehaviour(Behaviour* b, Agent* agent);
    };
}
