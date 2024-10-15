#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Timer.h"

struct Animation
{
    int delay;
    bool loop = true;
    std::vector<SDL_Rect> frames;
};
enum direction{ RIGHT,LEFT};
class Sprite
{
public:
    Sprite(SDL_Texture *texture);
    ~Sprite();

    void SetNumberAnimations(int num);
    void SetAnimationDelay(int id, int delay);
    void AddKeyFrame(int id, const SDL_Rect& rect);
    void SetAnimation(int id);
    int GetAnimation();
    void SetLoop(bool _loop);

    void Update();

    void Draw(int x, int y) const;
    void Draw(int x, int y, int offsetX, int offsetY) const;
    void DrawTint(int x, int y, const SDL_Color& col) const;

    void LookRight();
    void LookLeft();

    void Release();

private:
    int current_anim;
    int current_frame;
    direction player_dir;

    SDL_Texture*img;
    std::vector<Animation> animations;
    Timer timer;

};

