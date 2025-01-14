#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Parallax.h"
#include <box2d/box2d.h>
#include <string>
#include <vector>

class ParallaxTwoPoints : public Module {
public:
    ParallaxTwoPoints();
    ~ParallaxTwoPoints() {}
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    bool ChangeTextures();

    void SetRange(float pointA, float pointB);

    std::vector<Layer> layers;
    float previousCamX;
    int sizeW = 960;
    int sizeH = 480;

    std::string textureName1;
    std::string textureName2;
    std::string textureName3;
    std::string textureName4;
    std::string textureName5;

    float Xa = 0.0f; // Start of parallax range
    float Xb = 1000.0f; // End of parallax range
};
