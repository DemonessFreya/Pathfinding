#pragma once

#include "Node.h"
#include "raylib.h"
#include <vector>
#include <string>

namespace AIForGames
{
    class NodeMap
    {
        int m_width = 0;
        int m_height = 0;
        float m_cellSize = 0.0f;
        Node** m_nodes = nullptr;

    public:
        ~NodeMap();

        void Initialise(std::vector<std::string> asciiMap, int cellSize);
        void Draw(bool shouldDraw);
        void DrawPath(std::vector<Node*> path, Color lineColor);
		std::vector<Node*> SmoothPath(std::vector<Node*> path);
        bool IsVisibleFrom(Node* start, Node* end);
        Node* GetNode(int x, int y);
        Node* GetClosestNode(glm::vec2 worldPos);
        Node* GetRandomNode();
		float GetCellSize() const { return m_cellSize; }
    };
}
