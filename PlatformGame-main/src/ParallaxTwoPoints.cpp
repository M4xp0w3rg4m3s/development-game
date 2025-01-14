#include "ParallaxTwoPoints.h"
#include "Entity.h"
#include "Sprite.h"
#include "Engine.h"
#include "Textures.h"
#include "Log.h"

ParallaxTwoPoints::ParallaxTwoPoints() : previousCamX(0.0f) {
    layers.resize(5);
}

bool ParallaxTwoPoints::ChangeTextures() {
    std::vector<std::string> textureNames = { textureName1, textureName2, textureName3, textureName4, textureName5 };
    std::vector<float> speeds = { 0.0f, 0.1f, 0.3f, 0.5f, 0.7f };

    for (size_t i = 0; i < layers.size(); ++i) {
        layers[i].texture = Engine::GetInstance().textures->Load(textureNames[i].c_str());
        layers[i].speed = speeds[i];
        layers[i].offset = 0.0f;
    }
    return true;
}

void ParallaxTwoPoints::SetRange(float pointA, float pointB)
{
    Xa = pointA;
    Xb = pointB;
}

bool ParallaxTwoPoints::Start() {
    return ChangeTextures();
}

bool ParallaxTwoPoints::Update(float dt) {
    auto& engine = Engine::GetInstance();
    float cameraX = static_cast<float>(engine.render->camera.x);
    float deltaCamX = cameraX - previousCamX;
    SDL_Rect rect = { 0, 0, sizeW, sizeH };

    for (auto& layer : layers) {
        layer.offset += deltaCamX * layer.speed;

        // Wrap offset when it exceeds texture size
        if (fabs(layer.offset) > sizeW) {
            layer.offset = 0.0f;
        }

        for (int i = -1; i <= 1; ++i) {
            float drawX = -(int)cameraX + (int)(layer.offset + i * sizeW);

            // Check if the image overlaps with [Xa, Xb]
            if (drawX + sizeW >= Xa && drawX <= Xb) {
                SDL_Rect clippedRect = rect;

                // Adjust the clipped rectangle to render only the visible portion before Xb
                if (drawX + sizeW > Xb) {
                    clippedRect.w = (int)(Xb - drawX); // Clip width to end at Xb
                }

                // Draw the texture using the clipped rectangle
                engine.render->DrawTexture(layer.texture, (int)drawX, (int)engine.render->camera.y, &clippedRect, SDL_FLIP_NONE);
            }
        }
    }

    previousCamX = cameraX;
    return true;
}

bool ParallaxTwoPoints::CleanUp() {
    LOG("CleanUp Parallax");
    for (const auto layer : layers) {
        Engine::GetInstance().textures.get()->UnLoad(layer.texture);
    }

    return true;
}
