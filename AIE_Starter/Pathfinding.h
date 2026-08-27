#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "raylib.h"

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
		Node* previous;

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
		void Draw(bool shouldDraw);
		void DrawPath(std::vector<Node*> path, Color lineColor);
		Node* GetNode(int x, int y);
		Node* GetClosestNode(glm::vec2 worldPos);
	};

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

	std::vector<Node*> DijkstrasSearch(Node* startNode, Node* endNode);
}
