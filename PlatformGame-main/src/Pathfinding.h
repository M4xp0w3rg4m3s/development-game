#pragma once
#include <list>
#include <queue>
#include <functional> // for std::greater
#include "Vector2D.h"
#include "SDL2/SDL.h"
#include "Map.h"

enum ASTAR_HEURISTICS {
    MANHATTAN = 0,
    EUCLIDEAN,
    SQUARED
};

enum EnemyType {
    FLOOR,
    AIR,
    WATER
};

class Pathfinding
{

public:

	Pathfinding();

	~Pathfinding();

    void ResetPath(Vector2D pos);
    void DrawPath();
    bool IsWalkable(int x, int y);

    int MovementCost(int x, int y);
    void ComputePath(int x, int y);

    void SetEnemyType(EnemyType type);

    // L13: A* Pathfinding methods
    void PropagateAStar(ASTAR_HEURISTICS heuristic);

    void Compute();

private:
    int Find(std::vector<Vector2D> vector, Vector2D elem);

public:
    EnemyType currentType;

    Map* map;
    MapLayer* layerNav;
    std::vector<Vector2D> visited;
    SDL_Texture* pathTex = nullptr;
    Vector2D destination;

    std::vector<Vector2D> breadcrumbs; //list of tiles that form the path
    std::vector<std::vector<int>> costSoFar; //matrix that stores the accumulated cost in the propagation of the Dijkstra algorithm
    std::vector<Vector2D> pathTiles; //list of tiles that form the path
    SDL_Texture* tileX = nullptr; //texture used to show the path 

    // L13: A* Pathfinding variables
    std::priority_queue<std::pair<int, Vector2D>, std::vector<std::pair<int, Vector2D>>, std::greater<std::pair<int, Vector2D>> > frontierAStar;

    int blockedGid = 1081; //Gid of the tiles that block the path
    int highCostGid = 1080; //Gid of the tiles that have high cost

    int floorGid = 1082; //Gid of the tiles for the floor enemies
    int airGid = 1083; //Gid of the tiles for the air enemies
    int waterGid = 1084; //Gid of the tiles for the water enemies

    bool finished = false;
    bool objectiveFound = false;
    bool resetPathAfterEnd = false;

    Timer computeTimer;
    int computeTime = 2;
};

