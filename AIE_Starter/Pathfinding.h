#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "raylib.h"
#include "poly2tri/poly2tri.h"

namespace AIForGames
{
	struct Node;

	std::vector<Node*> DijkstrasSearch(Node* startNode, Node* endNode);

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

	class NavMesh
	{
	public:
		// create base walkable area
		NavMesh(float width, float height);
		~NavMesh();

		// triangylar navigation node
		class NavMeshNode : public Node {
		public:
			NavMeshNode() {}
			virtual ~NavMeshNode() {}

			int getAdjacentVertices(NavMeshNode* other, glm::vec2* adjacent);

			// the vertices of the triangle surrounding this node
			std::vector<glm::vec2> vertices;
		};

		struct Obstacle {
			float x, y, w, h, padding;
		};

		// returns true if the obstacle was added safely (does not overlap)
		bool addObstacle(float x, float y, float w, float h, float padding);

		void addObstacles(int num, int width, int height);

		void build();

		void Draw();

		std::vector<NavMeshNode*>& getNodes() { return m_nodes; }
		std::vector<Obstacle>& getObstacles() { return m_obstacles; }

	protected:
		std::vector<Obstacle> m_obstacles;
		std::vector<NavMeshNode*> m_nodes;

		// this is used for building the mesh
		// uses Poly2Tri
		p2t::CDT* m_cdt;
		std::vector<std::vector<p2t::Point*>> m_polygons;

		glm::vec2 extents;
	};
}
