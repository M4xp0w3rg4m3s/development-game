#pragma once

#include "Module.h"
#include <list>
#include <box2d/box2d.h>
#include <vector>
#include <unordered_map>
#include "Sprite.h"

class PhysBody;
class Entity;

struct Properties
{
    struct Property
    {
        std::string name;
        bool value; //We assume that we are going to work only with bool for the moment
    };

    std::list<Property*> propertyList;

    ~Properties()
    {
        for (const auto& property : propertyList)
        {
            delete property;
        }

        propertyList.clear();
    }

    // L09: DONE 7: Method to ask for the value of a custom property
    Property* GetProperty(const char* name);

};

struct MapLayer
{
    // L07: TODO 1: Add the info to the MapLayer Struct
    int id;
    std::string name;
    int width;
    int height;
    std::vector<int> tiles;
    Properties properties;

    // L07: TODO 6: Short function to get the gid value of i,j
    int Get(int i, int j) const
    {
        return tiles[(j * width) + i];
    }
};

// Create a struct needed to hold the information to Map node
struct TileSet
{
    int firstgid;
    std::string name;
    int tilewidth;
    int tileheight;
    int spacing;
    int margin;
    int tilecount;
    int columns;

    SDL_Texture* texture;

    // Implement the method that receives the gid and returns a Rect

    SDL_Rect GetRect(unsigned int gid) {
        SDL_Rect rect = { 0 };

        int relativeIndex = gid - firstgid;
        rect.w = tilewidth;
        rect.h = tileheight;
        rect.x = margin + (tilewidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileheight + spacing) * (relativeIndex / columns);

        return rect;
    }
};

struct MapData
{
    int width;
    int height;
    int tilewidth;
    int tileheight;
    std::list<TileSet*> tilesets;

    //Add a list/array of layers to the map
    std::list<MapLayer*> layers;
};

// Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!

class Map : public Module
{
public:

    Map();

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake(pugi::xml_node config);

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(std::string path, std::string mapFileName, bool doCallsToObjects=false);

    //Create a method that translates x,y coordinates from map positions to world positions
    Vector2D MapToWorld(int x, int y) const;

    // Method WorldToMap to obtain  map coordinates from screen coordinates
    Vector2D WorldToMap(int x, int y);

    // L09: TODO 2: Implement function to the Tileset based on a tile id
    TileSet* GetTilesetFromTileId(int gid) const;

    // L09: TODO 6: Load a group of properties 
    bool LoadProperties(xml_node& node, Properties& properties);

    std::unordered_map<int, Sprite*> tileAnimator;

    struct TileAnimationData
    {
        
    };

    int GetWidth() {
        return mapData.width;
    }

    int GetHeight() {
        return mapData.height;
    }

    int GetTileWidth() {
        return mapData.tilewidth;
    }

    int GetTileHeight() {
        return mapData.tileheight;
    }

    MapLayer* GetNavigationLayer();

    struct Checkpoint
    {
        Vector2D pos = {-1,-1};
        int level = -1;
        int order = -1;
        bool activated = false;
    };

    std::vector<Checkpoint*> checkpoints;

public: 
    std::string mapName;
    std::string mapPath;

private:
    // L05: DONE 1: Declare a variable data of the struct MapData
    MapData mapData;
    bool mapLoaded;

    b2Body* mapBody;

    std::vector<PhysBody*> colliders;
    std::vector<Entity*> objects;
};