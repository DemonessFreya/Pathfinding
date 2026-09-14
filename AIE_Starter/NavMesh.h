#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "poly2tri/poly2tri.h"
#include "Node.h"

namespace AIForGames
{
	// interface type class for NavMesh and NodeMap, so we can give our agent's either of them
	class INavigatable
	{
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
