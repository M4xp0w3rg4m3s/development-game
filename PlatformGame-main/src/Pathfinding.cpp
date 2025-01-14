#include "Pathfinding.h"
#include "Engine.h"
#include "Textures.h"
#include "Map.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"

Pathfinding::Pathfinding() {
    
     //Loads texture to draw the path
    pathTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");
    tileX = Engine::GetInstance().textures.get()->Load("Assets/Maps/x.png");
    map = Engine::GetInstance().map.get();
    layerNav = map->GetNavigationLayer();

    // Initialize the costSoFar with all elements set to 0
    costSoFar = std::vector<std::vector<int>>(map->GetWidth(), std::vector<int>(map->GetHeight(), 0));

    computeTimer.Start();
}

Pathfinding::~Pathfinding() {
    Engine::GetInstance().textures->UnLoad(pathTex);
    Engine::GetInstance().textures->UnLoad(tileX);

    if (layerNav != nullptr)
    {
        delete layerNav;
    }
    if (map != nullptr)
    {
        //delete map;
    }
}

// L11: BFS Pathfinding methods
void Pathfinding::ResetPath(Vector2D pos) {

    // Clear the frontierAStar queue
    while (!frontierAStar.empty()) {
		frontierAStar.pop();
	}

    visited.clear(); //Clear the visited list
    breadcrumbs.clear(); //Clear the breadcrumbs list
    pathTiles.clear(); //Clear the pathTiles list

    // Inserts the first position in the queue and visited list
    frontierAStar.push(std::make_pair(0, pos)); //AStar
    visited.push_back(pos);
    breadcrumbs.push_back(pos);

    //reset the costSoFar matrix
    costSoFar = std::vector<std::vector<int>>(map->GetWidth(), std::vector<int>(map->GetHeight(), 0));
    
}

void Pathfinding::DrawPath() {

    Vector2D point;

    // Draw visited
    for (const auto& pathTile : visited) {
        Vector2D pathTileWorld = Engine::GetInstance().map.get()->MapToWorld((int)(pathTile.getX()), (int)(pathTile.getY()));
        SDL_Rect rect = { 32,0,32,32 };
        Engine::GetInstance().render.get()->DrawTexture(pathTex, (int)(pathTileWorld.getX()), (int)(pathTileWorld.getY()), &rect);
    }

    // ---------------- Draw frontier AStar

    // Create a copy of the queue to iterate over
    std::priority_queue<std::pair<int, Vector2D>, std::vector<std::pair<int, Vector2D>>, std::greater<std::pair<int, Vector2D>> > frontierAStarCopy = frontierAStar;

    // Iterate over the elements of the frontier copy
    while (!frontierAStarCopy.empty()) {

        //Get the first element of the queue
        Vector2D frontierTile = frontierAStarCopy.top().second;
        //Get the position of the frontier tile in the world
        Vector2D pos = Engine::GetInstance().map.get()->MapToWorld((int)frontierTile.getX(), (int)frontierTile.getY());
        //Draw the frontier tile
        SDL_Rect rect = { 0,0,32,32 };
        Engine::GetInstance().render.get()->DrawTexture(pathTex, (int)pos.getX(), (int)pos.getY(), &rect);
        //Remove the front element from the queue
        frontierAStarCopy.pop();
    }


    // Draw path
    for (const auto& pathTile : pathTiles) {
        Vector2D pathTileWorld = map->MapToWorld((int)pathTile.getX(), (int)pathTile.getY());
        Engine::GetInstance().render.get()->DrawTexture(tileX, (int)pathTileWorld.getX(), (int)pathTileWorld.getY());
    }

}

bool Pathfinding::IsWalkable(int x, int y) {

    bool isWalkable = false;

    // L11: TODO 3: return true only if x and y are within map limits
    // and the tile is walkable (tile id 0 in the navigation layer)

    //Set isWalkable to true if the position is inside map limits and is a position that is not blocked in the navigation layer
    //Get the navigation layer

    if (layerNav != nullptr) {
        if (x >= 0 && y >= 0 && x < map->GetWidth() && y < map->GetHeight()) {
            int gid = layerNav->Get(x, y);
            if (currentType == EnemyType::FLOOR) {
                if (gid != blockedGid && gid != airGid && gid != waterGid) isWalkable = true;
            }
            else if (currentType == EnemyType::AIR) {
                if (gid != blockedGid && gid != waterGid) isWalkable = true;
            }
            else if (currentType == EnemyType::WATER) {
                if (gid != blockedGid && gid != floorGid && gid != airGid) isWalkable = true;
            }
        }
    }

    return isWalkable;
}

void Pathfinding::PropagateAStar(ASTAR_HEURISTICS heuristic) {

    // Dijkstra algorithm for AStar. Consider the different heuristics

    Vector2D playerPos = Engine::GetInstance().scene.get()->GetPlayerPosition();
    if (currentType == EnemyType::WATER) {
        playerPos = { Engine::GetInstance().scene.get()->GetPlayerPosition().getX(), Engine::GetInstance().scene.get()->GetPlayerPosition().getY() };
    }
    Vector2D playerPosTile = Engine::GetInstance().map.get()->WorldToMap((int)playerPos.getX(), (int)playerPos.getY());
    

    bool foundDestination = false;
    if (frontierAStar.size() > 0) {
        Vector2D frontierTile = frontierAStar.top().second;

        if (frontierTile == playerPosTile) {
            foundDestination = true;

            //When the destination is reach, call the function ComputePath
            ComputePath((int)frontierTile.getX(), (int)frontierTile.getY());
        }
    }

    //If frontier queue contains elements pop the first element and find the neighbors
    if (frontierAStar.size() > 0 && !foundDestination) {

        //Get the value of the firt element in the queue
        Vector2D frontierTile = frontierAStar.top().second;
        //remove the first element from the queue
        frontierAStar.pop();

        std::list<Vector2D> neighbors;
        if (IsWalkable((int)(frontierTile.getX() + 1), (int)(frontierTile.getY()))) {
            neighbors.push_back(Vector2D((float)((int)frontierTile.getX() + 1), (float)((int)frontierTile.getY())));
        }
        if (IsWalkable((int)frontierTile.getX(), (int)frontierTile.getY() + 1)) {
            neighbors.push_back(Vector2D((float)((int)frontierTile.getX()), (float)((int)frontierTile.getY() + 1)));
        }
        if (IsWalkable((int)frontierTile.getX() - 1, (int)frontierTile.getY())) {
            neighbors.push_back(Vector2D((float)((int)frontierTile.getX() - 1), (float)((int)frontierTile.getY())));
        }
        if (IsWalkable((int)frontierTile.getX(), (int)frontierTile.getY() - 1)) {
            neighbors.push_back(Vector2D((float)((int)frontierTile.getX()), (float)((int)frontierTile.getY() - 1)));
        }

        //For each neighbor, if not visited, add it to the frontier queue and visited list
        for (const auto& neighbor : neighbors) {

            // the movement cost from the start point A to the current tile.
            int g = costSoFar[(int)frontierTile.getX()][(int)frontierTile.getY()] + MovementCost((int)neighbor.getX(), (int)neighbor.getY());
            
            // the estimated movement cost from the current square to the destination point.
            int h = 0;

            switch (heuristic)
            {
            case ASTAR_HEURISTICS::MANHATTAN:
                h = (int)neighbor.distanceMahattan(playerPosTile);
                break;
            case ASTAR_HEURISTICS::EUCLIDEAN:
                h = (int)neighbor.distanceEuclidean(playerPosTile);
                break;
            case ASTAR_HEURISTICS::SQUARED:
                h = (int)neighbor.distanceSquared(playerPosTile);
                break;
            }

            // A* Priority function
            int f = g + h;

            if (std::find(visited.begin(), visited.end(), neighbor) == visited.end() || g < costSoFar[(int)neighbor.getX()][(int)neighbor.getY()]) {
                costSoFar[(int)neighbor.getX()][(int)neighbor.getY()] = g;
                frontierAStar.push(std::make_pair(f, neighbor));
                visited.push_back(neighbor);
                breadcrumbs.push_back(frontierTile);
            }
        }

    }

    if (frontierAStar.empty()) {
        finished = true;
        objectiveFound = false;
    }
}

void Pathfinding::Compute()
{
    while (!finished) {
        if (currentType == EnemyType::AIR) {
            PropagateAStar(SQUARED);
        }
        else {
            PropagateAStar(MANHATTAN);
        }
      
    }
    if (finished) {
        if (computeTimer.ReadSec() >= computeTime) {
            finished = false;
            objectiveFound = false;
            resetPathAfterEnd = true;
            computeTimer.Start();
        }
    }
}

int Pathfinding::MovementCost(int x, int y) 
{
    int ret = -1;

    if ((x >= 0) && (x < map->GetWidth()) && (y >= 0) && (y < map->GetHeight()))
    {
        int gid = layerNav->Get(x, y);
        if (gid == highCostGid) {
            ret = 5;
        }
        else ret = 1;
    }

    return ret;
}

void Pathfinding::ComputePath(int x, int y)
{
    // L12: TODO 2: Follow the breadcrumps to goal back to the origin
    // at each step, add the point into "pathTiles" (it will then draw automatically)

    //Clear the pathTiles list
    pathTiles.clear();
    // Save tile position received and stored in current tile
    Vector2D currentTile = Vector2D((float)x, (float)y);
    // Add the current tile to the pathTiles list (is the first element in the path)
    pathTiles.push_back(currentTile);
    // Find the position of the current tile in the visited list
    int index = Find(visited, currentTile);

    // while currentTile and breadcrumbs[index] are different it means we are not in the origin
    while ((index >= 0) && (currentTile != breadcrumbs[index]))
    {
        // update the current tile to the previous tile in the path
        currentTile = breadcrumbs[index];
        // Add the current tile to the pathTiles list
        pathTiles.push_back(currentTile);
        // Find the position of the current tile in the visited list
        index = Find(visited, currentTile);
    }

    finished = true;
    objectiveFound = true;
}

void Pathfinding::SetEnemyType(EnemyType type)
{
    currentType = type;
}

int Pathfinding::Find(std::vector<Vector2D> vector,Vector2D elem)
{
    int index = 0;
    bool found = false;
    for (const auto& e : vector) {
        if (e == elem) {
            found = true;
            break;
        }
        index++;
    }

    if(found) return index;
	else return -1;

}