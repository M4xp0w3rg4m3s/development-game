#include "Parallax.h"
#include "Entity.h"
#include "Sprite.h"
#include "Engine.h"
#include "Textures.h"
#include "Log.h"

Parallax::Parallax() : previousCamX(0.0f) {
    layers.resize(5);
}

bool Parallax::ChangeTextures() {
    std::vector<std::string> textureNames = { textureName1, textureName2, textureName3, textureName4, textureName5 };
    std::vector<float> speeds = { 0.0f, 0.1f, 0.3f, 0.5f, 0.7f };

    for (size_t i = 0; i < layers.size(); ++i) {
        layers[i].texture = Engine::GetInstance().textures->Load(textureNames[i].c_str());
        layers[i].speed = speeds[i];
        layers[i].offset = 0.0f;
    }
    return true;
}

bool Parallax::Start() {
    return ChangeTextures();
}

bool Parallax::Update(float dt) {
    auto& engine = Engine::GetInstance();
    float cameraX = static_cast<float>(engine.render->camera.x);
    float deltaCamX = cameraX - previousCamX;
    SDL_Rect rect = { 0, 0, sizeW, sizeH };

    for (auto& layer : layers) {
        layer.offset += deltaCamX * layer.speed;
        if (fabs(layer.offset) > sizeW) {
            layer.offset = 0.0f;
        }
        for (int i = -1; i <= 1; ++i) {
            engine.render->DrawTexture(layer.texture, -(int)cameraX + (int)(layer.offset + i * sizeW), (int)engine.render->camera.y, &rect, SDL_FLIP_NONE);
        }
    }

    previousCamX = cameraX;
    return true;
}

bool Parallax::CleanUp()
{
    LOG("CleanUp Parallax");
    for (const auto layer : layers)
    {
        Engine::GetInstance().textures.get()->UnLoad(layer.texture);
    }

    return true;
}
