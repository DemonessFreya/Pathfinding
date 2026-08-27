#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

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

		void ConnectTo(Node* other, float cost);
	};

	class NodeMap
	{
		int m_width = 0;
		int m_height = 0;
		float m_cellSize = 0.0f;

		Node** m_nodes = nullptr;

	public:
		~NodeMap();

		void Initialise(std::vector<std::string> asciiMap, int cellSize);
		void Draw();
		Node* GetNode(int x, int y);
	};
}
