#pragma once

#include <SDL_rect.h>
#ifndef START_GAME_H
#define START_GAME_H

#include <iostream>
#include <configs.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "rendering/sprite.h"
#include "rendering/ui.h"
#include "shared/game.h"


class StartGame
{
private:
    std::string currentPath;
    std::vector<Sprite> sprites;
    std::vector<Sprite> runSprites;
    SDL_Window *window;
    SDL_Renderer *renderer;

    // Charecter Selection
    Uint32 lastFrameTime = 0;
    const Uint32 frameDelay = 180;

    std::vector<SDL_Rect> fonts;
    SDL_Texture* font_texture;

    // Welcome Text
    SDL_Rect welcome_text_rect;
    std::string welcome_text = "Welcome To Dusk Watch";

    // Total width of the text: Number of characters * width of each character (scaled)
    int total_text_width = welcome_text.length() * FONT_WIDTH * FONT_SCALE; 

    // Calculate the starting x position to center the text
    int welcome_text_x = 0;
    int welcome_text_y = 100;

    int selected_character = 0;
    std::vector<SDL_Texture *> character_textures;
    SDL_Rect character_rect;
    int character_width = PLAYER_WIDTH * 2;
    int character_height = PLAYER_HEIGHT * 2;
    int character_x = (WIDTH / 2 - character_width / 2);
    int character_y = (HEIGHT / 2 - character_height / 2) - 50;

    // Start Button
    SDL_Texture *start_screen_texture;
    SDL_Rect start_button_rect;
    int start_button_width = 200;
    int start_button_height = 100;
    int start_button_x = WIDTH / 2 -  start_button_width / 2;
    int start_button_y = (HEIGHT / 2 - start_button_height / 2) + 100;
    // Forward Arrow Button
    SDL_Texture *forward_arrow_texture;
    SDL_Rect forward_arrow_rect;
    int forward_arrow_width = 64;
    int forward_arrow_height = 64;
    int forward_arrow_x = (WIDTH / 2 - forward_arrow_width / 2) + 80;
    int forward_arrow_y = (HEIGHT / 2 - forward_arrow_height / 2)- 50;
    // Backward Arrow Button
    SDL_Texture *backward_arrow_texture;
    SDL_Rect backward_arrow_rect;
    int backward_arrow_width = 64;
    int backward_arrow_height = 64;
    int backward_arrow_x = (WIDTH / 2 - backward_arrow_width / 2) - 80;
    int backward_arrow_y = (HEIGHT / 2 - backward_arrow_height / 2) - 50;
    // Keep Window Open
    bool keep_window_open;

    bool checkButtonClicked(SDL_Rect buttonRect, int mouseX, int mouseY);
public:
    StartGame();
    void start_screen();
    void initWindow();
    void initRenderer();
    void handleEvent(SDL_Event e);


    void loadStartButtonTexture();
    void loadArrowButtonTextures();
    void loadCharacterTextures();
    void loadFont();

    void renderStartButton();
    void renderArrowButtons();
    void renderCharacterSelection();
    void renderWelcomeText();
    void start();
};

#endif
