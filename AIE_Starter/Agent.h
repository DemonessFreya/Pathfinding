#pragma once

#include "PathAgent.h"
#include "NodeMap.h"
#include "Behaviour.h"
#include "raylib.h"
#include <vector>

namespace AIForGames
{
    class Agent
    {
    public:
        Agent() : m_current(nullptr), m_nodeMap(nullptr), m_color({ 255, 255, 0, 255 }) {}
        Agent(NodeMap* _nodeMap, Behaviour* _behaviour) : m_current(_behaviour), m_nodeMap(_nodeMap), m_color({ 255, 255, 0, 255 }) {}
        ~Agent() { delete m_current; }

        void Update(float deltaTime);
        void Draw();
        void GoTo(glm::vec2 point);
        bool PathComplete();

        // wrapper helpers for PathAgent
        void SetNode(Node* node);
        std::vector<Node*> GetPath();

        NodeMap* GetNodeMap() const;

    private:
        PathAgent m_pathAgent;
        Behaviour* m_current;
        NodeMap* m_nodeMap;
        Color m_color;
    };
}
