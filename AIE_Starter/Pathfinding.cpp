#include "Pathfinding.h"
#include <iostream>
#include <algorithm>

namespace AIForGames
{
	// ------------- Edge -------------
	Edge::Edge() {
		target = nullptr;
		cost = 0.0f;
	}
	Edge::Edge(Node* _target, float _cost) : target(_target), cost(_cost) {}

	// ------------- Node -------------
	Node::Node() : position(0.0f, 0.0f), gScore(0.0f), previous(nullptr) {}
	Node::Node(float x, float y) : position(x, y), gScore(0.0f), previous(nullptr) {}

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
					Node* nodeSouth = y == m_height - 1 ? nullptr : GetNode(x, y + 1);
                    if (nodeSouth) {
                        node->ConnectTo(nodeSouth, 1); // TODO: weights
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

    void NodeMap::DrawPath(std::vector<Node*> path, Color lineColor) {
		if (path.size() < 2) return; // if the path is empty or only has one node, there's nothing to draw

		// draw a line between each node in the path
        for (int i = 0; i < path.size() - 1; i++) {
            Node* node = path[i];
            Node* nextNode = path[i + 1];
            DrawLine((int)node->position.x, (int)node->position.y, (int)nextNode->position.x, (int)nextNode->position.y, lineColor);
        }
	}

    Node* NodeMap::GetNode(int x, int y) {
		if (x < 0 || x >= m_width) return nullptr;
		if (y < 0 || y >= m_height) return nullptr;

        return m_nodes[x + m_width * y];
    }

    Node* NodeMap::GetClosestNode(glm::vec2 worldPos) {
        // find the closest node to the given world position
		int i = (int)(worldPos.x / m_cellSize);
		if (i < 0 || i >= m_width) return nullptr;

		int j = (int)(worldPos.y / m_cellSize);
		if (j < 0 || j >= m_height) return nullptr;

		return GetNode(i, j);
	}

    // ------------- Dijkstra's Search Algorithm -------------
    std::vector<Node*> DijkstrasSearch(Node* startNode, Node* endNode) {
        if (startNode == nullptr || endNode == nullptr) return {}; // validate start and end nodes
        if (startNode == endNode) return { startNode }; // if start and end node are the same, return start node as the only node in the path

        // initialise starting node
		startNode->gScore = 0.0f;
		startNode->previous = nullptr;

		// create temporary lists for the open and closed nodes
		std::vector<Node*> openList;
		std::vector<Node*> closedList;

		openList.push_back(startNode);

        while (!openList.empty())
        {
            // sort open list by g-score
            std::sort(openList.begin(), openList.end(), [](const Node* a, const Node* b) { return a->gScore < b->gScore; });

			// set current node to the first node in the open list
			Node* currentNode = openList.front();

            // if we visit the endNode, then we can exit early.
            // sorting the openList above guarentees the shortest path is found, given no negative costs (a prerequisite of the algorithm).
			// this is an optional optimisation that improves performance, but doesn't always guarantee the shortest path.
            if (currentNode == endNode) break;

			// remove current node from open list and add to closed list
			openList.erase(openList.begin());
			closedList.push_back(currentNode);

			// iterate through the current node's connections
            for (auto& c : currentNode->connections) {
				// if c.target not in closed list
                if (std::find(closedList.begin(), closedList.end(), c.target) == closedList.end()) {
                    float gScore = currentNode->gScore + c.cost;

					// have not visited node yet, so calculate g-score and update its parent.
					// also add it to the open list for processing.
					auto iter = std::find(openList.begin(), openList.end(), c.target);
                    if (iter == openList.end()) {
                        c.target->gScore = gScore;
                        c.target->previous = currentNode;
                        openList.push_back(c.target);
					}
                    // node is already in the openList with a valid g-score.
                    // so compare the calculated g-score with the existing g-score to find the shorter path.
                    else {
                        if (gScore < c.target->gScore) {
                            c.target->gScore = gScore;
                            c.target->previous = currentNode;
						}
                    }
                }
            }
        }

		// create path in reverse from endNode to startNode
		std::vector<Node*> path;

        if (endNode->previous != nullptr || endNode == startNode) {
			Node* currentNode = endNode;
            while (currentNode != nullptr) {
                path.push_back(currentNode);
                currentNode = currentNode->previous;
			}
			std::reverse(path.begin(), path.end()); // reverse the path to be from startNode to endNode
		}

		// return the path to finish Dijkstra's search
		return path;
    }
}
