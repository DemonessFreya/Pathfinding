/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"
#include "Agent.h"
#include "FSM.h"
#include "NavMesh.h"
#include <vector>
#include <string>

using namespace AIForGames;

int main(int argc, char* argv[])
{
    int screenWidth = 1280;
    int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "A* Algorithm");
    SetTargetFPS(60);

	NodeMap nodeMap;
    std::vector<std::string> asciiMap;
    asciiMap.push_back("000000000000");
    asciiMap.push_back("010111011100");
    asciiMap.push_back("010101110110");
    asciiMap.push_back("010100000010");
    asciiMap.push_back("010111111110");
    asciiMap.push_back("010000001000");
    asciiMap.push_back("011111111110");
    asciiMap.push_back("000000000000");
    nodeMap.Initialise(asciiMap, 32);

	Node* start = nodeMap.GetNode(1, 1);
	Node* end = nodeMap.GetNode(10, 2);
	std::vector<Node*> path = AStarSearch(start, end);
	Color lineColor = { 0, 255, 0, 255 }; // bright green

	Agent agent(&nodeMap, new GotoPointBehaviour());
    agent.SetNode(start);

	Agent agent2(&nodeMap, new WanderBehaviour());
	agent2.SetNode(nodeMap.GetRandomNode());

    // set up a FSM, we're going to have two states with their own conditions
    DistanceCondition* closerThan5 = new DistanceCondition(5.0f * nodeMap.GetCellSize(), true);
    DistanceCondition* furtherThan7 = new DistanceCondition(7.0f * nodeMap.GetCellSize(), false);

    // register these states with the FSM, so its responsible for deleting them now
    State* wanderState = new State(new WanderBehaviour());
    State* followState = new State(new FollowBehaviour());
    wanderState->AddTransition(closerThan5, followState);
    followState->AddTransition(furtherThan7, wanderState);

    // make a finite state machine that starts off wandering
    FiniteStateMachine* fsm = new FiniteStateMachine(wanderState);
    fsm->AddState(wanderState);
    fsm->AddState(followState);

    Agent agent3(&nodeMap, fsm);
    agent3.SetNode(nodeMap.GetRandomNode());
    agent3.SetTarget(&agent);
    agent3.SetSpeed(32);

	/*Agent agent3(&nodeMap, new SelectorBehaviour(new FollowBehaviour(), new WanderBehaviour()));
	agent3.SetNode(nodeMap.GetRandomNode());
	agent3.SetTarget(&agent);
    agent3.SetSpeed(32);*/

    NavMesh navigation(screenWidth, screenHeight);
    srand(42);
    navigation.addObstacles(12, 60, 60);
    navigation.build();

	float time = (float)GetTime();
	float deltaTime = 0.0f;

    // Main game loop
    while (!WindowShouldClose())
    {
		float fTime = (float)GetTime();
		deltaTime = fTime - time;
		time = fTime;

        BeginDrawing();
		ClearBackground(BLACK);

        nodeMap.Draw(true);
		nodeMap.DrawPath(agent.GetPath(), lineColor);

        //navigation.Draw();

		agent.Update(deltaTime);
		agent.Draw(); // draw the agent

		agent2.Update(deltaTime);
		agent2.Draw(); // draw the wandering agent

		agent3.Update(deltaTime);
		agent3.Draw(); // draw the following agent

        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();

    return 0;
}