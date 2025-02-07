#include "rendering/ui.h"

#include "SDL2/SDL.h"
#include <SDL_render.h>
#include <SDL_image.h>

void UI::renderTextAtPosition(SDL_Renderer* renderer,SDL_Texture* texture, std::vector<SDL_Rect> fonts,std::string text, int textX, int textY, int font_width, int font_height, int scale, bool isCentered, float spaceDistance)
{
    if(!renderer)
    {
        std::cout << "(UI Text) Error: Renderer is NULL" << std::endl;
        return;
    }
    if(!texture)
    {
        std::cout << "(UI Text) Error: Texture is NULL" << std::endl;
        return;
    }
    if(spaceDistance < 0)
    {
        spaceDistance = scale;
    }
    // Verify Fonts Vector is not empty
    if (fonts.empty())
    {
        std::cout << "Error: Font Vector is Empty" << std::endl;
        return;
    }
    // We Need to Find The Index of Each Character in the Font by actual character
    std::vector<int> indices = getCharacterIndices(text);
    int x = textX;
    int y = textY;
    // Calculate total width of the text
    int total_width = 0;
    std::string characterSet = "abcdefghijklmnopqrstuvwxyz1234567890#,.!?:*%()+-/\\=><áàãâçêéèíóú'\"[]";
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
            x += font_width * spaceDistance;
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
    // Keep original character set but we'll convert input to lowercase
    std::string characterSet = "abcdefghijklmnopqrstuvwxyz1234567890[]#,.!?:*%()+-/\\=><áàãâçêéèíóú'\"";
    
    for (char ch : input) {
        // Convert to lowercase before checking
        char lowerCh = std::tolower(ch);
        size_t index = characterSet.find(lowerCh);
        if (index != std::string::npos) {
            indices.push_back(static_cast<int>(index));
        } else if (ch == ' ' || ch == '_') {  // Treat underscore as space
            indices.push_back(-1); 
        } else if (ch == '/') {  // Handle path separator
            indices.push_back(characterSet.find('/')); // Use forward slash from character set
        } else {
            std::cout << "Unhandled character: '" << ch << "' (ASCII: " << (int)ch << ")" << std::endl;
            indices.push_back(-2);
        }
    }
    return indices;
}
