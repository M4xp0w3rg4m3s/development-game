
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Scene.h"
#include "Physics.h"
#include "Boulder.h"
#include "Item.h"
#include "EntityManager.h"

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{
    for (const auto anim : tileAnimator) {
        delete anim.second;
    }
    tileAnimator.clear();
}

// Called before render is available
bool Map::Awake(pugi::xml_node config)
{
    name = "map";
    LOG("Loading Map Parser");
    bool ret = true;

    return ret;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    if (mapLoaded) {

        for (const auto anim : tileAnimator) {
            anim.second->Update();
        }

        for (const auto& mapLayer : mapData.layers) {
            //Check if the property Draw exist get the value, if it's true draw the lawyer
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {

                        //Get the gid from tile
                        int gid = mapLayer->Get(i, j);
                        //Check if the gid is different from 0 - some tiles are empty
                        if (gid != 0) {
                            TileSet* tileSet = GetTilesetFromTileId(gid);
                            if (tileSet != nullptr) {
                                if (tileAnimator.find(gid) != tileAnimator.end()) {
                                    gid = tileAnimator.at(gid)->GetCurrentFrame().extraData;
                                }
                                //Get the Rect from the tileSetTexture;
                                SDL_Rect tileRect = tileSet->GetRect(gid);
                                //Get the screen coordinates from the tile coordinates
                                Vector2D mapCoord = MapToWorld(i, j);
                                //Draw the texture
                                Engine::GetInstance().render->DrawTexture(tileSet->texture, (int)(mapCoord.getX()), (int)(mapCoord.getY()), &tileRect);
                            }
                        }
                    }
                }
            }
        }
    }
    return ret;
}

TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = nullptr;

    for (const auto& tileset : mapData.tilesets) {
        if (gid >= tileset->firstgid && gid < (tileset->firstgid + tileset->tilecount)) {
            set = tileset;
            break;
        }
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
	}
	mapData.tilesets.clear();

    // clean up all layer data
    for(const auto& layer:mapData.layers)
	{
		delete layer;
	}
    mapData.layers.clear();

    for (const auto& collider : colliders)
    {
        Engine::GetInstance().physics->DeletePhysBody(collider);
    }
    colliders.clear();

    for (const auto& object : objects)
    {
        Engine::GetInstance().entityManager->DeleteEntity(object);
    }
    objects.clear();

    for (const auto& checkpoint : checkpoints)
    {
        if (checkpoint != nullptr)
        {
            delete checkpoint;
        }
    }
    checkpoints.clear();

    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName, bool doCallsToObjects)
{
    bool ret = false;
    mapLoaded = ret;
    // Assigns the name of the map file and the path
    mapName = fileName;
    mapPath = path;
    std::string mapPathName = mapPath + mapName;
    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        ret = false;
    }
    else {

        // LoadMap to load the map properties
        // retrieve the paremeters of the <map> node and store the into the mapData struct
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tilewidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileheight = mapFileXML.child("map").attribute("tileheight").as_int();

        //LoadTileSet function to load the tileset properties

        //Iterate the Tileset
        for (xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            //Load Tileset attributes
            TileSet* tileSet = new TileSet();
            tileSet->firstgid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tilewidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileheight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tilecount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();

            //Load the tileset image
            std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

            //Load tileset anims
            for (xml_node tileAnimated = tilesetNode.child("tile"); tileAnimated != NULL; tileAnimated = tileAnimated.next_sibling("tile")) {
                int gid = tileAnimated.attribute("id").as_int()+1;
                bool isLoop = tileAnimated.child("properties").find_child_by_attribute("property", "name", "Loop").attribute("value").as_bool();
                for (xml_node animationData = tileAnimated.child("animation"); animationData != NULL; animationData = animationData.next_sibling("animation")) {
                    Sprite* animation = new Sprite(tileSet->texture);
                    animation->SetNumberAnimations(1);
                    for (xml_node animationFrame = animationData.child("frame"); animationFrame != NULL; animationFrame = animationFrame.next_sibling("frame")) {
                        int frameGid = animationFrame.attribute("tileid").as_int()+1;
                        animation->AddKeyFrame(0, {0,0,0,0}, frameGid);
                        animation->SetAnimationDelay(0, animationFrame.attribute("duration").as_int());
                    }
                    animation->SetAnimation(0);
                    animation->SetLoop(isLoop);
                    tileAnimator.emplace(gid, animation);
                }
            }

            mapData.tilesets.push_back(tileSet);
        }

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L07: TODO 4: Implement the load of a single layer 
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L09: TODO 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Iterate over all the tiles and assign the values in the data array
            for (xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            //add the layer to the map
            mapData.layers.push_back(mapLayer);
        }


        // Create a function here to load and create the colliders from the map
        // Formula --> CreateRectangle(x + width/2, y + height/2, width, height, STATIC);

        // In case of memory leaks make a list to store the collider pointers and then free them at the end of the code

        for (xml_node objectGroup = mapFileXML.child("map").child("objectgroup"); objectGroup != NULL; objectGroup = objectGroup.next_sibling("objectgroup")) {
            std::string groupName = objectGroup.attribute("name").as_string();
            for (xml_node objectNode = objectGroup.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
                int x = objectNode.attribute("x").as_int();
                int y = objectNode.attribute("y").as_int();
                int h = objectNode.attribute("height").as_int();
                int w = objectNode.attribute("width").as_int();
                if (groupName == "Collisions") {
                    PhysBody* collider = Engine::GetInstance().physics.get()->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
                    collider->ctype = ColliderType::PLATFORM;
                    colliders.emplace_back(collider);
                }
                else if (groupName == "Kill") {
                    PhysBody* collider = Engine::GetInstance().physics.get()->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
                    collider->ctype = ColliderType::KILL;
                    colliders.emplace_back(collider);
                }
                else if (groupName == "Objects") {
                    std::string objType = objectNode.child("properties").child("property").attribute("value").as_string();
                    if (objType == "Boulder") {
                        Boulder* boulder = (Boulder*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOULDER, doCallsToObjects);
                        if (doCallsToObjects)
                            boulder->SetNewPos({ (float)x,(float)y });
                        else
                            boulder->position = Vector2D((float)x, (float)y);
                        objects.emplace_back(boulder);
                    }
                }
                else if (groupName == "Checkpoints") {
                    int checkpointOrder = objectNode.child("properties").find_child_by_attribute("name", "Order").attribute("value").as_int();
                    int checkpointLevel = objectNode.child("properties").find_child_by_attribute("name", "Level").attribute("value").as_int();

                    Checkpoint* currentCheckpoint = new Checkpoint{ { (float)x, (float)y }, checkpointLevel, checkpointOrder, false };
                    checkpoints.emplace_back(currentCheckpoint);
                }
            }
        }

        ret = true;

        // LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", fileName.c_str());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tilewidth, mapData.tileheight);

            LOG("Tilesets----");

            //iterate the tilesets
            for (const auto& tileset : mapData.tilesets) {
                LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstgid);
                LOG("tile width : %d tile height : %d", tileset->tilewidth, tileset->tileheight);
                LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
            }

            LOG("Layers----");

            for (const auto& layer : mapData.layers) {
                LOG("id : %d name : %s", layer->id, layer->name.c_str());
                LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
            }
        }
        else {
            LOG("Error while parsing map file: %s", mapPathName.c_str());
        }

        if (mapFileXML) mapFileXML.reset();

    }

    mapLoaded = ret;
    return ret;
}

// Translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    ret.setX((float)(x * mapData.tilewidth));
    ret.setY((float)(y * mapData.tileheight));

    return ret;
}

Vector2D Map::WorldToMap(int x, int y) {

    Vector2D ret(0, 0);

    ret.setX((float)(x / mapData.tilewidth));
    ret.setY((float)(y / mapData.tileheight));

    return ret;
}

bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
    }

    return ret;
}

MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
        if (layer->properties.GetProperty("Navigation") != NULL &&
            layer->properties.GetProperty("Navigation")->value) {
            return layer;
        }
    }

    return nullptr;
}

Properties::Property* Properties::GetProperty(const char* name)
{
    for (const auto& property : propertyList) {
        if (property->name == name) {
            return property;
        }
    }

    return nullptr;
}
