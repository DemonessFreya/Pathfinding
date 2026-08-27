#include "Pathfinding.h"
#include <iostream>
#include "raylib.h"

namespace AIForGames
{
	// ------------- Edge -------------
	Edge::Edge() {
		target = nullptr;
		cost = 0.0f;
	}
	Edge::Edge(Node* _target, float _cost) : target(_target), cost(_cost) {}

	// ------------- Node -------------
	Node::Node() : position(0.0f, 0.0f) {}
	Node::Node(float x, float y) : position(x, y) {}

	void Node::ConnectTo(Node* other, float cost) {
		connections.push_back(Edge(other, cost));
	}

	// ------------- NodeMap -------------
    NodeMap::~NodeMap() {
        delete[] m_nodes;
    }

	void NodeMap::Initialise(std::vector<std::string> asciiMap, int cellSize) {
        m_cellSize = static_cast<float>(cellSize);
        const char emptySquare = '0';

        // assume all strings are the same length, so we'll size the map according to the number of strings and the length of the first one
        m_height = static_cast<int>(asciiMap.size());
        m_width = static_cast<int>(asciiMap[0].size());

        m_nodes = new Node * [m_width * m_height];

        // loop over the strings, creating Node entries as we go
        for (int y = 0; y < m_height; y++) {
            std::string& line = asciiMap[y];
            // report to the user that you have a mis-matched string length
            if (line.size() != static_cast<size_t>(m_width))
                std::cout << "Mismatched line #" << y << " in ASCII map (" << line.size() << " instead of " << m_width << ")" << std::endl;

            for (int x = 0; x < m_width; x++) {
                // get the x-th character, or return an empty node if the string isn't long enough
                char tile = (x < static_cast<int>(line.size())) ? line[x] : emptySquare;

                // create a node for anything but a '.' character
                m_nodes[x + m_width * y] = tile == emptySquare ? nullptr : new Node(((float)x + 0.5f) * m_cellSize, ((float)y + 0.5f) * m_cellSize);
            }
        }

        // now loop over the nodes, creating connections between each node and its neighbour to the West and South on the grid. this will link up all nodes
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                Node* node = GetNode(x, y);
                if (node) {
                    // see if there's a node to our west, checking for array overruns first if we're on the west-most edge
                    Node* nodeWest = x == 0 ? nullptr : GetNode(x - 1, y);
                    if (nodeWest) {
                        node->ConnectTo(nodeWest, 1); // TODO: weights
                        nodeWest->ConnectTo(node, 1);
                    }

                    // see if there's a node south of us, checking for array index overruns again
                    Node* nodeSouth = y == 0 ? nullptr : GetNode(x, y - 1);
                    if (nodeSouth) {
                        node->ConnectTo(nodeSouth, 1);
                        nodeSouth->ConnectTo(node, 1);
                    }
                }
            }
        }
    }

    void NodeMap::Draw() {
        // red colour for blocks
        Color cellColor;
        cellColor.a = 255;
        cellColor.r = 255;
        cellColor.g = 0;
        cellColor.b = 0;

        // black colour for connections
        Color lineColor;
        lineColor.a = 255;

        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                Node* node = GetNode(x, y);
                if (node == nullptr) {
                    // draw a solid block in empty squares without a navigation node
                    DrawRectangle((int)(x * m_cellSize), (int)(y * m_cellSize), (int)m_cellSize - 1, (int)m_cellSize - 1, cellColor);
                }
                else {
                    // draw the connections between the node and its neighbours
                    for (int i = 0; i < node->connections.size(); i++) {
                        Node* other = node->connections[i].target;
                        DrawLine((x + 0.5f) * m_cellSize, (y + 0.5f) * m_cellSize, (int)other->position.x, (int)other->position.y, lineColor);
                    }
                }
            }
        }
    }

    Node* NodeMap::GetNode(int x, int y) {
        return m_nodes[x + m_width * y];
    }
}
