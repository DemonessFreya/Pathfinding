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
#include "Pathfinding.h"
#include <vector>
#include <string>

using namespace AIForGames;

int main(int argc, char* argv[])
{
    int screenWidth = 600;
    int screenHeight = 400;

	NodeMap nodeMap;
    std::vector<std::string> asciiMap;
    asciiMap.push_back("000000000000");
    asciiMap.push_back("010111011100");
    asciiMap.push_back("010101110110");
    asciiMap.push_back("010100000000");
    asciiMap.push_back("010111111110");
    asciiMap.push_back("010000001000");
    asciiMap.push_back("011111111110");
    asciiMap.push_back("000000000000");
    nodeMap.Initialise(asciiMap, 50);

	Node* start = nodeMap.GetNode(1, 1);
	Node* end = nodeMap.GetNode(10, 2);
	std::vector<Node*> path = DijkstrasSearch(start, end);
	Color lineColor = { 0, 255, 0, 255 }; // bright green

    InitWindow(screenWidth, screenHeight, "Dijkstra's Algorithm");

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        BeginDrawing();

		ClearBackground(BLACK); // black background

		// click on node map to set a new target node and recalculate the path
        if (IsMouseButtonPressed(0)) {
			Vector2 mousePos = GetMousePosition();
			end = nodeMap.GetClosestNode(glm::vec2(mousePos.x, mousePos.y));
			if (end == nullptr) end = nodeMap.GetNode(10, 2); // if no node found, use the default end node
			path = DijkstrasSearch(start, end);
        }

		nodeMap.Draw(); // draw the node map
		nodeMap.DrawPath(path, lineColor); // draw the path

        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();

    return 0;
}