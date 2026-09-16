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
#include "UtilityAI.h"
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
    asciiMap.push_back("00000000000000000000000");
    asciiMap.push_back("01101111111111111111110");
    asciiMap.push_back("01101111111111111111110");
    asciiMap.push_back("01101100000000000000010");
    asciiMap.push_back("01101111111110111111110");
    asciiMap.push_back("01101111111110111000000");
    asciiMap.push_back("01101111101110111111110");
    asciiMap.push_back("01101111101110000011110");
    asciiMap.push_back("01101111101111111111110");
    asciiMap.push_back("01111111100000000000010");
    asciiMap.push_back("01110111101111111111110");
    asciiMap.push_back("01110111101111111111110");
    asciiMap.push_back("01110111101111111111110");
    asciiMap.push_back("01110111111111111111110");
    asciiMap.push_back("00000000000000000000000");
    
    nodeMap.Initialise(asciiMap, 32);

	//Node* start = nodeMap.GetNode(1, 1);
	//Node* end = nodeMap.GetNode(23, 14);
	//std::vector<Node*> path = AStarSearch(start, end);
	//Color lineColor = { 0, 255, 0, 255 }; // bright green

	NavMesh navigation(screenWidth, screenHeight);
	srand(42);
    navigation.addObstacles(12, 60, 60);
    navigation.build();
	Node* start = navigation.getNodes()[0];

	Agent agent(&navigation, new GotoPointBehaviour());
    agent.SetNode(start);
	agent.SetSpeed(64);

	Agent agent2(&navigation, new WanderBehaviour());
	agent2.SetNode(navigation.GetRandomNode());
	agent2.SetSpeed(64);

    UtilityAI* utilityAI = new UtilityAI();
	utilityAI->AddBehaviour(new WanderBehaviour());
	utilityAI->AddBehaviour(new FollowBehaviour());

	Agent agent3(&navigation, utilityAI);
	agent3.SetNode(navigation.GetRandomNode());
	agent3.SetTarget(&agent);
	agent3.SetSpeed(32);

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

  //      nodeMap.Draw(true);
		//nodeMap.DrawPath(agent.GetPath(), lineColor);

        navigation.Draw();
		navigation.DrawPath(agent.GetPath(), { 0, 255, 0, 255 }); // draw the path of the agent in bright green
        navigation.DrawSmoothPath(navigation.SmoothPath(agent.GetPath()), RED);

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