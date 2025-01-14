#include "Sprite.h"
#include "Engine.h"
#include "Render.h"
#include "Textures.h"

Sprite::Sprite(SDL_Texture*texture)
{
    img = texture;
    current_anim = -1;
    current_frame = 0;
    animFinished = false;
}
Sprite::~Sprite()
{
    Release();
}
void Sprite::SetNumberAnimations(int num)
{
    animations.clear();
    animations.resize(num);
}
void Sprite::SetAnimationDelay(int id, int delay)
{
    if (id >= 0 && id < animations.size())
    {
        animations[id].delay = delay;
    }
}
void Sprite::AddKeyFrame(int id, const SDL_Rect& rect)
{
    if (id >= 0 && id < animations.size())
    {
        animations[id].frames.push_back({rect,-1});
    }
}
void Sprite::AddKeyFrame(int id, const SDL_Rect& rect, int extraData)
{
    if (id >= 0 && id < animations.size())
    {
        animations[id].frames.push_back({ rect,extraData });
    }
}
void Sprite::SetAnimation(int id)
{
    if (id >= 0 && id < animations.size())
    {
        last_frame = -1;
        current_anim = id;
        current_frame = 0;
        timer.Start();
    }
}
int Sprite::GetAnimation()
{
    return current_anim;
}

Frame Sprite::GetCurrentFrame()
{
    return animations[current_anim].frames[current_frame];
}

int Sprite::GetCurrentFrame_int()
{
    return current_frame;
}

int Sprite::GetLastFrame_int()
{
    return last_frame;
}

// Cambiar y pasar la id de la animación
void Sprite::SetLoop(bool _loop)
{
    animations[current_anim].loop = _loop;
}

void Sprite::Update()
{
    if (current_anim == -1)
    {
        last_frame = -1;
        return;
    }


    last_frame = current_frame;
    if (timer.ReadMSec() >= animations[current_anim].delay)
    {
        
        //Only automatic animation mode advances next frame
        if (animations[current_anim].loop)
        {
            current_frame++;
            if (current_frame >= animations[current_anim].frames.size()) {

                current_frame = 0;
                last_frame = -1;
            }
            timer.Start();
        }
        else {
            current_frame++;
            if (current_frame >= animations[current_anim].frames.size()) {
                
                current_frame = current_frame - 1;
                animFinished = true;
                last_frame = -1;
            }
            timer.Start();
        }

    }
}

void Sprite::Draw(int x, int y) const
{
    DrawTint(x, y, {255,255,255,255});
}
void Sprite::Draw(int x, int y, int offsetX, int offsetY) const
{
    DrawTint(x + offsetX * (player_dir==LEFT ? -1 : 1), y + offsetY, { 255,255,255,255 });
}
void Sprite::DrawTint(int x, int y, const SDL_Color& col) const
{
    if (current_anim >= 0 && current_anim < animations.size())
    {
        SDL_Rect rect = animations[current_anim].frames[current_frame].rect;
        if (player_dir == direction::RIGHT) {
            Engine::GetInstance().render->DrawTexture(img, x, y, &rect,(SDL_RendererFlip)0);
        }
        if (player_dir == direction::LEFT) {
            Engine::GetInstance().render->DrawTexture(img, x, y, &rect, (SDL_RendererFlip)1);
        }
    }
}
void Sprite::LookRight()
{
    player_dir = direction::RIGHT;
}
void Sprite::LookLeft()
{
    player_dir = direction::LEFT;
}
bool Sprite::IsLookingRight()
{
    if (player_dir == direction::RIGHT)
    {
        return true;
    }
    return false;
}
bool Sprite::IsLookingLeft()
{
    if (player_dir == direction::LEFT)
    {
        return true;
    }
    return false;
}
direction Sprite::GetPlayerDir()
{
    return player_dir;
}
void Sprite::Release()
{
    //Release each animation's frames
    for (auto& animation : animations)
    {
        animation.frames.clear();
    }
    //Clear the animations vector
    animations.clear();
}
