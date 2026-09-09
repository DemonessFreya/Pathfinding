#pragma once

#include "Node.h"
#include "raylib.h"
#include <glm/glm.hpp>
#include <vector>

namespace AIForGames
{
    class PathAgent
    {
    private:
        glm::vec2 m_position;
        std::vector<Node*> m_path;
        int m_currentIndex;
        Node* m_currentNode;
        float m_speed;

    public:
        PathAgent();
        void Update(float deltaTime);
        void GoToNode(Node* node);
        void Draw();
        void SetNode(Node* node);
        void SetSpeed(float speed);
        void GetPath(std::vector<Node*>& path);
    };
}
