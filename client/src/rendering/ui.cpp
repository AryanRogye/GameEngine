#include "rendering/ui.h"

#include "SDL2/SDL.h"
#include <SDL_render.h>
#include <SDL_image.h>

void UI::renderTextAtPosition(SDL_Renderer* renderer,SDL_Texture* texture, std::vector<SDL_Rect> fonts,std::string text, int textX, int textY, int font_width, int font_height, int scale, bool isCentered)
{
    // We Need to Find The Index of Each Character in the Font by actual character
    std::vector<int> indices = getCharacterIndices(text);
    
    int x = textX;
    int y = textY;
    // Calculate total width of the text
    int total_width = 0;
    for (int index : indices)
    {
        if (index != -1 && index != -2)
        {
            total_width += font_width * scale;
        }
        else if (index == -1)
        {
            total_width += font_width * scale;
        }
    }
    
    if (isCentered)
    {
        x = (WIDTH - total_width) / 2;
    }
    
    for (int index : indices)
    {
        if (index == -1)
        {
            x += font_width * scale;
            continue;
        }
        if (index == -2)
        {
            continue;
        }
        SDL_Rect srcRect = fonts[index];
        SDL_Rect destRect = {x, y, font_width * scale, font_height * scale};
        Sprite::renderSprite(renderer, texture, srcRect, destRect);
        x += font_width * scale;
    }
}

std::vector<int> UI::getCharacterIndices(const std::string& input) 
{
    std::vector<int> indices;

    // Updated character set based on the font sprite sheet
    std::string characterSet = "abcdefghijklmnopqrstuvwxyz1234567890#,.!?:*%()+-/\\=><áàãâçêéèíóú'\"";

    for (char ch : input) {
        char lowerCh = std::tolower(ch); // Convert to lowercase

        size_t index = characterSet.find(lowerCh);
        if (index != std::string::npos) {
            indices.push_back(static_cast<int>(index)); // Valid character
        } else if (ch == ' ') {
            indices.push_back(-1); // Handle spaces manually
        } else {
            indices.push_back(-2); // Unknown character placeholder
        }
    }
    return indices;
}
