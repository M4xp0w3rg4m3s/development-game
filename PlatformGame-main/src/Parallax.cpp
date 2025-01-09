#include "Parallax.h"

#include "Entity.h"
#include "Sprite.h"
#include "Engine.h"
#include "Textures.h"
#include "Log.h"

Parallax::Parallax()
{
}

bool Parallax::ChangeTextures()
{
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

bool Parallax::Start()
{
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

bool Parallax::Update(float dt)
{

	layer1.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX) * layer1.speed;
	if (abs(layer1.offset) > sizeW) {
		layer1.offset = 0;
	}
	layer2.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX) * layer2.speed;
	if (abs(layer2.offset) > sizeW) {
		layer2.offset = 0;
	}
	layer3.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX) * layer3.speed;
	if (abs(layer3.offset) > sizeW) {
		layer3.offset = 0;
	}
	layer4.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX) * layer4.speed;
	if (abs(layer4.offset) > sizeW) {
		layer4.offset = 0;
	}
	layer5.offset += ((float)Engine::GetInstance().render->camera.x - previousCamX) * layer5.speed;
	if (abs(layer5.offset) > sizeW) {
		layer5.offset = 0;
	}

	SDL_Rect rect = { 0, 0, sizeW, sizeH };


	Engine::GetInstance().render->DrawTexture(layer1.texture, -(int)Engine::GetInstance().render->camera.x + (int)layer1.offset, (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer1.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer1.offset - sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer1.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer1.offset + sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);


	Engine::GetInstance().render->DrawTexture(layer2.texture, -(int)Engine::GetInstance().render->camera.x + (int)layer2.offset, (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer2.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer2.offset - sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer2.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer2.offset + sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);


	Engine::GetInstance().render->DrawTexture(layer3.texture, -(int)Engine::GetInstance().render->camera.x + (int)layer3.offset, (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer3.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer3.offset - sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer3.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer3.offset + sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);


	Engine::GetInstance().render->DrawTexture(layer4.texture, -(int)Engine::GetInstance().render->camera.x + (int)layer4.offset, (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer4.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer4.offset - sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer4.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer4.offset + sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);


	Engine::GetInstance().render->DrawTexture(layer5.texture, -(int)Engine::GetInstance().render->camera.x + (int)layer5.offset, (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer5.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer5.offset - sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);
	Engine::GetInstance().render->DrawTexture(layer5.texture, -(int)Engine::GetInstance().render->camera.x + (int)(layer5.offset + sizeW), (int)Engine::GetInstance().render->camera.y, &rect, (SDL_RendererFlip)0);

	previousCamX = (float)Engine::GetInstance().render->camera.x;

	return true;
}
