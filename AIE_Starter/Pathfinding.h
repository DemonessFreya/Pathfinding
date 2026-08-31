#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "raylib.h"
#include "poly2tri/poly2tri.h"

namespace AIForGames
{
	struct Node;

	std::vector<Node*> AStarSearch(Node* startNode, Node* endNode);

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

	/*class Agent {
	public:
		Agent() {}
		Agent(INavigatable* _nodeMap, Behaviour* _behaviour) : m_current(_behaviour), m_nodeMap(_nodeMap), m_color({ 255, 255, 0, 255 }) { m_pathAgent.SetSpeed(32); }
		~Agent() { delete m_current; }

		void Update(float deltaTime);
		void Draw();

		void GoTo(Node* node);
		void GoTo(glm::vec2 point);
		void SetNode(Node* node);

		bool PathComplete();

		std::vector<Node*>& GetPath() { return m_pathAgent; }
		void SetSpeed(float speed) { m_pathAgent.SetSpeed(speed); }

		INavigatable* GetNodeMap() { return m_nodeMap; }

		Agent* GetTarget() { return m_target; }
		void SetTarget(Agent* target) { m_target = target; }

		glm::vec2 GetPosition() { return m_pathAgent.GetPosition(); }

		void Reset() { m_pathAgent.GetPath().clear(); }

		void SetColor(Color c) { m_color = c; }

	private:
		PathAgent m_pathAgent;
		Behaviour* m_current;
		INavigatable* m_nodeMap;
		Color m_color;

		Agent* m_target;
	};*/

	// interface type class for NavMesh and NodeMap, so we can give our agent's either of them
	class INavigatable {
	public:
		virtual void Draw() = 0;
		virtual Node* GetClosestNode(glm::vec2 worldPos) = 0;
		// default value used to determine if we've "moved significantly" when tracking a moving target
		virtual float GetCellSize() { return 32; }
		virtual Node* GetRandomNode() = 0;
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
