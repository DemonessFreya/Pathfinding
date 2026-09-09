#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace AIForGames
{
    struct Node;

    struct Edge {
        Edge();
        Edge(Node* _target, float _cost);

        Node* target;
        float cost;
    };

    struct Node {
        Node();
        Node(float x, float y);

        glm::vec2 position;
        std::vector<Edge> connections;

        float gScore;
        float hScore;
        float fScore;
        Node* previous;

        void ConnectTo(Node* other, float cost);
    };

    std::vector<Node*> AStarSearch(Node* startNode, Node* endNode);
}
