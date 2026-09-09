#pragma once

#include "PathAgent.h"
#include "NodeMap.h"
#include "Behaviour.h"
#include <vector>

namespace AIForGames
{
    class Agent
    {
    public:
        Agent() : m_current(nullptr), m_nodeMap(nullptr), m_target(nullptr), m_speed(0), m_color({ 255, 255, 0, 255 }) {}
        Agent(NodeMap* _nodeMap, Behaviour* _behaviour) : m_current(_behaviour), m_nodeMap(_nodeMap), m_target(nullptr), m_speed(0), m_color({ 255, 255, 0, 255 }) {}
        ~Agent() { delete m_current; }

        void Update(float deltaTime);
        void Draw();
        void GoTo(glm::vec2 point);
        bool PathComplete();
		void Reset() { m_pathAgent.SetNode(nullptr); }

        // wrapper helpers for PathAgent
        void SetNode(Node* node);
        std::vector<Node*> GetPath();

		NodeMap* GetNodeMap() const { return m_nodeMap; }
        glm::vec2 GetPosition() const { return m_pathAgent.GetPosition(); }
        Agent* GetTarget() const { return m_target; }
		void SetTarget(Agent* target) { m_target = target; }
        void SetSpeed(int speed) { m_speed = speed; m_pathAgent.SetSpeed((float)speed); }
        void SetColor(Color color) { m_color = color; }

    private:
        PathAgent m_pathAgent;
        Behaviour* m_current;
        NodeMap* m_nodeMap;
        Agent* m_target;
        int m_speed;
        Color m_color;
    };
}
