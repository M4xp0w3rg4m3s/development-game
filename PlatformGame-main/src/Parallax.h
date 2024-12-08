#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include <box2d/box2d.h>
#include "Sprite.h"
#include "Engine.h"
#include "Textures.h"
#include "Log.h"


class Parallax : public Module {
public:

	Parallax() {
	}

	~Parallax() {

	}

	struct Layer {

		float offset;
		float speed;
		SDL_Texture* texture;
	};

	bool ChangeTextures() {
		layer1.texture = Engine::GetInstance().textures->Load(textureName1.c_str());
		layer1.speed = 0;
		layer1.offset = 0;

		layer2.texture = Engine::GetInstance().textures->Load(textureName2.c_str());
		layer2.speed = 0.1f;
		layer2.offset = 0;

		layer3.texture = Engine::GetInstance().textures->Load(textureName3.c_str());
		layer3.speed = 0.3f;
		layer3.offset = 0;

		layer4.texture = Engine::GetInstance().textures->Load(textureName4.c_str());
		layer4.speed = 0.5f;
		layer4.offset = 0;

		layer5.texture = Engine::GetInstance().textures->Load(textureName5.c_str());
		layer5.speed = 0.7f;
		layer5.offset = 0;

		return true;

		return true;
	}

	bool Start() {
		layer1.texture = Engine::GetInstance().textures->Load(textureName1.c_str());
		layer1.speed = 0;
		layer1.offset = 0;

		layer2.texture = Engine::GetInstance().textures->Load(textureName2.c_str());
		layer2.speed = 0.1f;
		layer2.offset = 0;

		layer3.texture = Engine::GetInstance().textures->Load(textureName3.c_str());
		layer3.speed = 0.3f;
		layer3.offset = 0;

		layer4.texture = Engine::GetInstance().textures->Load(textureName4.c_str());
		layer4.speed = 0.5f;
		layer4.offset = 0;

		layer5.texture = Engine::GetInstance().textures->Load(textureName5.c_str());
		layer5.speed = 0.7f;
		layer5.offset = 0;

		return true;
	}

	bool Update(float dt) {

		layer1.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX ) * layer1.speed;
		if (abs(layer1.offset) > sizeW) {
			layer1.offset = 0;
		}
		layer2.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX)* layer2.speed;
		if (abs(layer2.offset) > sizeW) {
			layer2.offset = 0;
		}
		layer3.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX)* layer3.speed;
		if (abs(layer3.offset) > sizeW) {
			layer3.offset = 0;
		}
		layer4.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX)* layer4.speed;
		if (abs(layer4.offset) > sizeW) {
			layer4.offset = 0;
		}
		layer5.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX)* layer5.speed;
		if (abs(layer5.offset) > sizeW) {
			layer5.offset = 0;
		}

		SDL_Rect rect = { 0, 0, sizeW, sizeH};

		
		Engine::GetInstance().render->DrawTexture(layer1.texture, -(float)Engine::GetInstance().render->camera.x + layer1.offset, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer1.texture, -(float)Engine::GetInstance().render->camera.x + layer1.offset - sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer1.texture, -(float)Engine::GetInstance().render->camera.x + layer1.offset + sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		
		
		Engine::GetInstance().render->DrawTexture(layer2.texture, -(float)Engine::GetInstance().render->camera.x + layer2.offset, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer2.texture, -(float)Engine::GetInstance().render->camera.x + layer2.offset - sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer2.texture, -(float)Engine::GetInstance().render->camera.x + layer2.offset + sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		
		
		Engine::GetInstance().render->DrawTexture(layer3.texture, -(float)Engine::GetInstance().render->camera.x + layer3.offset, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer3.texture, -(float)Engine::GetInstance().render->camera.x + layer3.offset - sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer3.texture, -(float)Engine::GetInstance().render->camera.x + layer3.offset + sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		
		
		Engine::GetInstance().render->DrawTexture(layer4.texture, -(float)Engine::GetInstance().render->camera.x + layer4.offset, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer4.texture, -(float)Engine::GetInstance().render->camera.x + layer4.offset - sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer4.texture, -(float)Engine::GetInstance().render->camera.x + layer4.offset + sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		
		
		Engine::GetInstance().render->DrawTexture(layer5.texture, -(float)Engine::GetInstance().render->camera.x + layer5.offset, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer5.texture, -(float)Engine::GetInstance().render->camera.x + layer5.offset - sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		Engine::GetInstance().render->DrawTexture(layer5.texture, -(float)Engine::GetInstance().render->camera.x + layer5.offset + sizeW, (float)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
		
		previousCamX = (float)Engine::GetInstance().render->camera.x;

		return true;
	}

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
