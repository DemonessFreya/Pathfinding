#pragma once

#include "Node.h"
#include <glm/glm.hpp>
#include "raylib.h"
#include <vector>

namespace AIForGames
{
	class NodeMap;

    class PathAgent
    {
    private:
        glm::vec2 m_position;
        std::vector<Node*> m_path;
        int m_currentIndex;
		NodeMap* m_nodeMap;
        Node* m_currentNode;
        float m_speed;

    public:
        PathAgent();
        void Update(float deltaTime);
        void GoToNode(Node* node);
        void Draw(Color color);
		void SetNode(Node* node) { m_currentNode = node; if (node != nullptr) m_position = node->position; }
		Node* GetNode() const { return m_currentNode; }
		void SetSpeed(float speed) { m_speed = speed; }
		std::vector<Node*> GetPath() const { return m_path; }
        glm::vec2 GetPosition() const { return m_position; }
		void SetNodeMap(NodeMap* nodeMap) { m_nodeMap = nodeMap; }
    };
}
