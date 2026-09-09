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
    };

    class GotoPointBehaviour : public Behaviour
    {
    public:
        virtual void Update(Agent* agent, float deltaTime) override;
    };
}
