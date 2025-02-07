#pragma once

#ifndef UI_H
#define UI_H


#include <iostream>
#include <vector>
#include <string>
#include "configs.h"
#include "SDL2/SDL.h"
#include <SDL_render.h>
#include <SDL_image.h>
#include "sprite.h"

class UI {
private:
    static std::vector<int> getCharacterIndices(const std::string& input);
public:
    /** For Start Scree **/
    static void renderTextAtPosition(SDL_Renderer* renderer,SDL_Texture* texture, std::vector<SDL_Rect> fonts,std::string text, int textX, int textY, int font_width, int font_height, int scale, bool isCentered = false, float spaceDistance = -1);
};


#endif // UI_H
