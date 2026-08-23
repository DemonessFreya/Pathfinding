#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace AIForGames
{
    struct Node;

    struct Edge {
        Node* target;
        float cost;
    };

    struct Node {
        glm::vec2 position;
        std::vector<Edge> connections;
    };

    class NodeMap
    {
        int m_width, m_height;
        float m_cellSize;

        Node** m_cellSize;
    public:
        void Init(std::vector<std::string> asciiMap, int cellSize) {
            m_cellSize = cellSize;
            const char emptySquare = '0';
        }
    };
}
