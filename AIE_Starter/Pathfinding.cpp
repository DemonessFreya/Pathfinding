#include "Agent.h"
#include "NavMesh.h"
#include "FSM.h"
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
	Node::Node() : position(0.0f, 0.0f), gScore(0.0f), hScore(0.0f), fScore(0.0f), previous(nullptr) {}
	Node::Node(float x, float y) : position(x, y), gScore(0.0f), hScore(0.0f), fScore(0.0f), previous(nullptr) {}

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

    void NodeMap::Draw(bool shouldDraw) {
        if (!shouldDraw) return;
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

    Node* NodeMap::GetRandomNode() {
        // pick a random node from the map
		Node* node = nullptr;
        while (node == nullptr) {
            int x = rand() % m_width;
            int y = rand() % m_height;
            node = GetNode(x, y);
		}
        return node;
	}

	// ------------- End of NodeMap -------------


	// ------------- PathAgent -------------
	PathAgent::PathAgent() : m_position(0.0f, 0.0f), m_currentIndex(0), m_currentNode(nullptr), m_speed(100.0f) {}

    void PathAgent::Update(float deltaTime) {
        if (m_path.empty()) return;

        // calculate remaining distance before moving
        float currentDistance = glm::distance(m_position, m_path[m_currentIndex]->position);
        float stepDistance = m_speed * deltaTime;

        // will we reach/overshoot the node this frame?
        if (currentDistance - stepDistance > 0.0001f) {
            // safe to normalize because currentDistance > 0.0001f
            glm::vec2 unitVectorToNextNode = (m_path[m_currentIndex]->position - m_position) / currentDistance;
            m_position += unitVectorToNextNode * stepDistance;
        }
        else {
			// otherwise we have overshot the current target node
            m_currentIndex++;

            if (m_currentIndex >= m_path.size()) {
				// reached the end of the path, so snap to the last node and clear the path
                m_position = m_path.back()->position;
				m_currentNode = m_path.back();
                m_path.clear();
			}
            else {
				// invert the overshoot distance to get a positive overshoot distance
				float overshoot = stepDistance - currentDistance;

                // start from the node we just reached
				glm::vec2 previousNodePos = m_path[m_currentIndex - 1]->position;
				glm::vec2 nextNodePos = m_path[m_currentIndex]->position;

                // direction vector for new segment
				glm::vec2 newSegmentDirection = glm::normalize(nextNodePos - previousNodePos);

				// move from previous node along the new segment direction by the overshoot distance
                m_position = previousNodePos + (newSegmentDirection * overshoot);
            }
        }
    }

    void PathAgent::GoToNode(Node* node) {
		if (node == nullptr) return; // validate the target node
		m_path = AStarSearch(m_currentNode, node);
        m_currentIndex = 0; // set index to 0 if path excludes current node, or 1 if path includes current node
    }

    void PathAgent::Draw(Color color) {
        DrawCircle((int)m_position.x, (int)m_position.y, 8, color);
    }

	// ------------- End of PathAgent -------------


	// ------------- Agent -------------
    void Agent::Update(float deltaTime) {
        if (m_current) {
            m_current->Update(this, deltaTime);
		}
		m_pathAgent.Update(deltaTime);
    }

    void Agent::Draw() {
        m_pathAgent.Draw(m_color);
	}

    void Agent::GoTo(glm::vec2 point) {
        Node* end = m_nodeMap->GetClosestNode(point);

        // if the agent has no current node, find the closest one based on its current position 
        if (m_pathAgent.GetNode() == nullptr) {
            Node* startNode = m_nodeMap->GetClosestNode(m_pathAgent.GetPosition());
            if (startNode != nullptr) {
                m_pathAgent.SetNode(startNode);
            }
        }

        m_pathAgent.GoToNode(end);
    }

    bool Agent::PathComplete() {
        return GetPath().empty();
    }

    void Agent::SetNode(Node* node) {
        m_pathAgent.SetNode(node);
    }

    std::vector<Node*> Agent::GetPath() {
        std::vector<Node*> path;
        m_pathAgent.GetPath(path);
        return path;
	}

    // ------------- End of Agent -------------


	// ------------- Behaviour -------------

    void GotoPointBehaviour::Update(Agent* agent, float deltaTime) {
        // read mouseclicks, left for start node, end for right node
        if (IsMouseButtonPressed(0))
        {
            Vector2 mousePos = GetMousePosition();
            agent->GoTo(glm::vec2(mousePos.x, mousePos.y));
        }
    }

    void WanderBehaviour::Update(Agent* agent, float deltaTime) {
        // if the agent has no path, pick a random node and go to it
        if (agent->PathComplete()) {
            Node* randomNode = agent->GetNodeMap()->GetRandomNode();
            agent->GoTo(randomNode->position);
        }
	}

    void FollowBehaviour::Update(Agent* agent, float deltaTime) {
        // check if the agent has moved significantly from its last position
        // if so we want to repath towards it
        Agent* target = agent->GetTarget();

        float dist = glm::distance(target->GetPosition(), lastTargetPos);
        if (dist > agent->GetNodeMap()->GetCellSize())
        {
            lastTargetPos = target->GetPosition();
            agent->GoTo(lastTargetPos);
        }
        
	}

    void SelectorBehaviour::SetBehaviour(Behaviour* b, Agent* agent) {
        if (m_selected != b)
        {
            m_selected = b;
            agent->Reset();
        }
	}

    void SelectorBehaviour::Update(Agent* agent, float deltaTime) {
		if (glm::distance(agent->GetPosition(), agent->GetTarget()->GetPosition()) < agent->GetNodeMap()->GetCellSize() * 3) // if the agent is within 3 cells of its target, switch to follow behaviour
        {
            SetBehaviour(m_b1, agent);
            agent->SetColor({ 255, 0, 0, 255 }); // red
        }
        else
        {
            SetBehaviour(m_b2, agent);
            agent->SetColor({ 0, 255, 255, 255 }); // cyan
        }
        m_selected->Update(agent, deltaTime);
	}

	// ------------- End of Behaviour -------------


	// ------------ Finite State Machine -------------
    State::~State() {
        // we own the behaviours assigned to us
        for (Behaviour* b : m_behaviours)
            delete b;

        // we also own the Conditions in each Transition
        // (but the states are references, so don’t clean them up here)
        for (Transition t : m_transitions)
            delete t.condition;
    }

    FiniteStateMachine::~FiniteStateMachine() {
        for (State* s : m_states)
            delete s;
    }

    void FiniteStateMachine::Update(Agent* agent, float deltaTime) {
        State* newState = nullptr;

        // check the current state's transitions
        for (State::Transition t : m_currentState->GetTransitions())
        {
            if (t.condition->IsTrue(agent))
                newState = t.targetState;
        }

        // if we've changed state, clean up the old one and initialise the new one
        if (newState != nullptr && newState != m_currentState)
        {
            m_currentState->Exit(agent);
            m_currentState = newState;
            m_currentState->Enter(agent);
        }

        // update the current state
        m_currentState->Update(agent, deltaTime);
    }

    void State::Update(Agent* agent, float deltaTime) {
        for (Behaviour* b : m_behaviours)
            b->Update(agent, deltaTime);
    }

	// ------------- End of Finite State Machine -------------


    // ------------- A* Search Algorithm -------------
    std::vector<Node*> AStarSearch(Node* startNode, Node* endNode) {
        if (startNode == nullptr || endNode == nullptr) return {}; // validate start and end nodes
        if (startNode == endNode) return { startNode }; // if start and end node are the same, return start node as the only node in the path

        // initialise starting node
        startNode->gScore = 0.0f;
        startNode->hScore = glm::distance(startNode->position, endNode->position);
        startNode->fScore = startNode->gScore + startNode->hScore;
        startNode->previous = nullptr;

        // create temporary lists for the open and closed nodes
        std::vector<Node*> openList;
        std::vector<Node*> closedList;

        openList.push_back(startNode);

		bool pathFound = false;

        while (!openList.empty())
        {
            // sort open list by g-score + h-score (f-score)
            std::sort(openList.begin(), openList.end(), [](const Node* a, const Node* b) { return a->fScore < b->fScore; });

            // set current node to the first node in the open list
            Node* currentNode = openList.front();

            // if we visit the endNode, then we can exit early.
            // sorting the openList above guarentees the shortest path is found, given no negative costs (a prerequisite of the algorithm).
            // this is an optional optimisation that improves performance, but doesn't always guarantee the shortest path.
            if (currentNode == endNode) {
                pathFound = true;
                break;
            }

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
                        c.target->hScore = glm::distance(c.target->position, endNode->position);
                        c.target->fScore = c.target->gScore + c.target->hScore;
                        c.target->previous = currentNode;
                        openList.push_back(c.target);
                    }
                    // node is already in the openList with a valid g-score.
                    // so compare the calculated g-score with the existing g-score to find the shorter path.
                    else {
                        if (gScore < c.target->gScore) {
                            c.target->gScore = gScore;
                            c.target->fScore = c.target->gScore + c.target->hScore;
                            c.target->previous = currentNode;
                        }
                    }
                }
            }
        }

        // create path in reverse from endNode to startNode
        std::vector<Node*> path;

		// only reconstruct the path if we found a path to the endNode in this specific search. if we didn't find a path, then the path will be empty.
        if (pathFound) {
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

    // ------------- End of A* Search Algorithm -------------


	// ------------- NavMesh -------------
	NavMesh::NavMesh(float width, float height) {
        m_polygons.push_back({});
		m_polygons[0].push_back(new p2t::Point(0, 0));
		m_polygons[0].push_back(new p2t::Point(0, height));
		m_polygons[0].push_back(new p2t::Point(width, height));
		m_polygons[0].push_back(new p2t::Point(width, 0));

		m_cdt = new p2t::CDT(m_polygons[0]);

		extents = glm::vec2(width, height);
    }

    NavMesh::~NavMesh() {
        for (auto node : m_nodes) {
            delete node;
        }
        m_nodes.clear();

        if (m_cdt) {
            delete m_cdt;
        }
    }

    bool NavMesh::addObstacle(float x, float y, float width, float height, float padding) {
        for (auto& ob : m_obstacles) {
            if (((ob.x + ob.w + ob.padding) < x - padding ||
                (ob.y + ob.h + ob.padding) < y - padding ||
                (x + width + padding) < ob.x - ob.padding ||
                (y + height + padding) < ob.y - ob.padding) == false)
                return false; // overlap detected
        }

		Obstacle o = { x, y, width, height, padding };
		m_obstacles.push_back(o);

		// add the obstacle as a hole in the triangulation
		m_polygons.push_back({});
		m_polygons.back().push_back(new p2t::Point(o.x - padding, o.y - padding));
		m_polygons.back().push_back(new p2t::Point(o.x - padding, o.y + o.h + padding));
		m_polygons.back().push_back(new p2t::Point(o.x + o.w + padding, o.y + o.h + padding));
		m_polygons.back().push_back(new p2t::Point(o.x + o.w + padding, o.y - padding));
		m_cdt->AddHole(m_polygons.back());

		return true; // obstacle added successfully
    }

    void NavMesh::addObstacles(int num, int width, int height) {
        // random obstacles
        for (int i = 0; i < num; ++i) {
            bool safe = false;
            do {
                safe = addObstacle((rand() / float(RAND_MAX) * 0.75f + 0.125f) * extents.x,
                    (rand() / float(RAND_MAX) * 0.75f + 0.125f) * extents.y,
                    width, height, 10);
            } while (safe == false);
        }
    }

    void NavMesh::build() {
        m_cdt->Triangulate();

        // first convert triangles to NavMesh::NavMeshNode's
        std::vector<p2t::Triangle*> triangles = m_cdt->GetTriangles();
        for (auto tri : triangles) {
            auto node = new NavMesh::NavMeshNode();

            node->vertices.push_back({ (float)tri->GetPoint(0)->x, (float)tri->GetPoint(0)->y });
            node->vertices.push_back({ (float)tri->GetPoint(1)->x, (float)tri->GetPoint(1)->y });
            node->vertices.push_back({ (float)tri->GetPoint(2)->x, (float)tri->GetPoint(2)->y });

            node->position.x = (node->vertices[0].x + node->vertices[1].x + node->vertices[2].x) / 3;
            node->position.y = (node->vertices[0].y + node->vertices[1].y + node->vertices[2].y) / 3;

            m_nodes.push_back(node);
        }

        // then link nodes that share triangle edges
        for (auto n : m_nodes) {
            for (auto n2 : m_nodes) {
                if (n == n2) continue;

                glm::vec2 v[2];
                if (n->getAdjacentVertices(n2, v) == 2) {
                    float mag = (n->position.x - n2->position.x) * (n->position.x - n2->position.x) + (n->position.y - n2->position.y) * (n->position.y - n2->position.y);

                    float distance = glm::length(n->position - n2->position);
                    n->connections.push_back(Edge(n2, distance));
                    n2->connections.push_back(Edge(n, distance));
                }
            }
        }

        // cleanup polygons
        for (auto& p : m_polygons) {
            for (auto ptr : p) delete ptr;
        }
        m_polygons.clear();

        // close up Poly2Tri
        delete m_cdt;
        m_cdt = nullptr;
    }

    int NavMesh::NavMeshNode::getAdjacentVertices(NavMesh::NavMeshNode* other, glm::vec2* adjacent) {
        int count = 0;
        for (auto v : vertices) {
            for (auto v2 : other->vertices) {
                if (v.x == v2.x && v.y == v2.y) {
                    adjacent[count++] = v;
                    break;
                }
            }
        }
        return count;
    }

    void NavMesh::Draw() {
        Color m_lineColor = { 0, 128, 255, 255 }; // pale blue
        Color m_obstacleColor = { 255, 0, 0, 255 }; // red

        // draw nav mesh polygons
        for (auto node : getNodes()) {
            DrawLine((int)node->vertices[0].x, (int)node->vertices[0].y, (int)node->vertices[1].x, (int)node->vertices[1].y, m_lineColor);
            DrawLine((int)node->vertices[1].x, (int)node->vertices[1].y, (int)node->vertices[2].x, (int)node->vertices[2].y, m_lineColor);
            DrawLine((int)node->vertices[2].x, (int)node->vertices[2].y, (int)node->vertices[0].x, (int)node->vertices[0].y, m_lineColor);
        }

        // draw obstacles
        for (auto& o : getObstacles()) {
            DrawRectangle((int)o.x, (int)o.y, (int)o.w, (int)o.h, m_obstacleColor);
        }
    }

	// ------------- End of NavMesh -------------
}
