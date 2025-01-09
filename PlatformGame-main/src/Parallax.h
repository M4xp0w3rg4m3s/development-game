#pragma once

#include "Module.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include <box2d/box2d.h>
#include <string>

class Parallax : public Module {
public:

	Parallax();
	~Parallax() {}

	struct Layer {

		float offset;
		float speed;
		SDL_Texture* texture;
	};

	bool ChangeTextures();
	
	bool Start();
	bool Update(float dt);

public:

	Layer layer1{0,0,nullptr};
	std::string textureName1;
	Layer layer2{ 0,0,nullptr };
	std::string textureName2;
	Layer layer3{ 0,0,nullptr };
	std::string textureName3;
	Layer layer4{ 0,0,nullptr };
	std::string textureName4;
	Layer layer5{ 0,0,nullptr };
	std::string textureName5;

	int sizeW = 1421;
	int sizeH = 480;

	float previousCamX = 0;
};
