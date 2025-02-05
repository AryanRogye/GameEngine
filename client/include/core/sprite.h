#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include <string>
struct Sprite
{
    std::string name;
    std::string path;
    int currentFrame;
    int frameCount;
};

#endif // SPRITE_H
