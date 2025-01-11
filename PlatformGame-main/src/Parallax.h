#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include <box2d/box2d.h>
#include <string>
#include <vector>

struct Layer {
	SDL_Texture* texture = nullptr;
	float speed = 0.0f;
	float offset = 0.0f;
};

class Parallax : public Module {
public:
	Parallax();
	bool Start();
	bool Update(float dt);
	bool ChangeTextures();

	std::vector<Layer> layers;
	float previousCamX;
	int sizeW = 1421;  // Example default width
	int sizeH = 480;  // Example default height

	std::string textureName1;
	std::string textureName2;
	std::string textureName3;
	std::string textureName4;
	std::string textureName5;
};
