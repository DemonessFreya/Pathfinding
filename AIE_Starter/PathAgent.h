#pragma once

#include "Node.h"
#include <glm/glm.hpp>
#include "raylib.h"
#include <vector>

namespace AIForGames
{
	class INavigatable;

    class PathAgent
    {
    private:
        glm::vec2 m_position;
        std::vector<Node*> m_path;
		std::vector<glm::vec2> m_smoothPath;
        int m_currentIndex;
		INavigatable* m_nodeMap;
        glm::vec2 m_currentNode;
        float m_speed;

    public:
        PathAgent();
        void Update(float deltaTime);
        void GoToNode(Node* node);
        void Draw(Color color);
		void SetNode(Node* node);
        void SetNode(glm::vec2 node);
		glm::vec2 GetNode() const { return m_currentNode; }
		void SetSpeed(float speed) { m_speed = speed; }
		std::vector<Node*> GetPath() const { return m_path; }
		std::vector<glm::vec2>& GetSmoothPath() { return m_smoothPath; }
        glm::vec2 GetPosition() const { return m_position; }
		void SetNodeMap(INavigatable* nodeMap) { m_nodeMap = nodeMap; }
    };
}
