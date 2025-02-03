#include "start_game.h"
#include <SDL_render.h>

void StartGame::start_screen()
{
    this->loadStartButtonTexture();
    this->loadArrowButtonTextures();
    this->loadCharacterTextures();

    while(this->keep_window_open)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e)){ this->handleEvent(e); }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    
        this->renderStartButton();
        this->renderArrowButtons();
        this->renderCharacterSelection();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void StartGame::renderCharacterSelection()
{
    this->character_rect.x = this->character_x;
    this->character_rect.y = this->character_y;
    this->character_rect.w = this->character_width;
    this->character_rect.h = this->character_height;

    Uint32 now = SDL_GetTicks();
    if (now - this->lastFrameTime > this->frameDelay)
    {
        this->sprites[this->selected_character].currentFrame = 
        (this->sprites[this->selected_character].currentFrame + 1) % this->sprites[this->selected_character].frameCount;
        this->lastFrameTime = now;
    }
    // Define the source rectangle if using a sprite sheet (assuming each sprite is 30x30)
    SDL_Rect srcRect = { this->sprites[this->selected_character].currentFrame * 30, 0, 30, 45 };
    // Destination rectangle based on your camera offset calculations
    // Render the sprite with the flip
    SDL_RenderCopyEx(this->renderer, this->character_textures[this->selected_character], &srcRect, &this->character_rect, 0.0, NULL, SDL_FLIP_NONE);
    /*SDL_RenderCopy(this->renderer, this->character_textures[this->selected_character], NULL, &character_rect);*/
}

void StartGame::renderArrowButtons()
{
    this->forward_arrow_rect.x = this->forward_arrow_x;
    this->forward_arrow_rect.y = this->forward_arrow_y;
    this->forward_arrow_rect.w = this->forward_arrow_width;
    this->forward_arrow_rect.h = this->forward_arrow_height;
    SDL_RenderCopy(this->renderer, this->forward_arrow_texture, NULL, &forward_arrow_rect);
    
    this->backward_arrow_rect.x = this->backward_arrow_x;
    this->backward_arrow_rect.y = this->backward_arrow_y;
    this->backward_arrow_rect.w = this->backward_arrow_width;
    this->backward_arrow_rect.h = this->backward_arrow_height;
    SDL_RenderCopy(this->renderer, this->backward_arrow_texture, NULL, &backward_arrow_rect);
}
void StartGame::renderStartButton()
{
    // Show The Whole Start Button

    this->start_button_rect.x = this->start_button_x;
    this->start_button_rect.y = this->start_button_y;
    this->start_button_rect.w = this->start_button_width;
    this->start_button_rect.h = this->start_button_height;

    SDL_RenderCopy(this->renderer, this->start_screen_texture, NULL, &start_button_rect);
}

void StartGame::loadCharacterTextures()
{
    for (auto& character : this->sprites)
    {
        SDL_Surface* character_surface = IMG_Load(character.path.c_str());
        if(!character_surface)
        {
            std::cout << "Failed to load character image" << std::endl;
            return;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        SDL_Texture* character_texture = SDL_CreateTextureFromSurface(this->renderer, character_surface);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        SDL_FreeSurface(character_surface);
        if (!character_texture)
        {
            std::cout << "Failed to create texture from surface" << std::endl;
            this->keep_window_open = false;
            return;
        }
        this->character_textures.push_back(character_texture);
    }
}

void StartGame::loadArrowButtonTextures()
{
    // Both The Arrow Buttons are in the same image 60x30 pixels
    // One is 60x30 and the other is 60x30
    SDL_Surface* arrow_buttons_surface = IMG_Load((this->currentPath + "../../Assets/UI/arrow_buttons.png").c_str());
    if(!arrow_buttons_surface)
    {
        std::cout << "Failed to load arrow buttons image" << std::endl;
        return;
    }
    // Define Source Rectangles
    SDL_Rect srcForward = { 0, 0, 30, 30 };
    SDL_Rect srcBackward = { 30, 0, 30, 30 };

    SDL_Surface* forwardSurface = SDL_CreateRGBSurface(0, srcForward.w, srcForward.h,
        arrow_buttons_surface->format->BitsPerPixel,
        arrow_buttons_surface->format->Rmask,
        arrow_buttons_surface->format->Gmask,
        arrow_buttons_surface->format->Bmask,
        arrow_buttons_surface->format->Amask
    );
    SDL_BlitSurface(arrow_buttons_surface, &srcForward, forwardSurface, NULL);
    SDL_Surface* backwardSurface = SDL_CreateRGBSurface(0, srcBackward.w, srcBackward.h,
        arrow_buttons_surface->format->BitsPerPixel,
        arrow_buttons_surface->format->Rmask,
        arrow_buttons_surface->format->Gmask,
        arrow_buttons_surface->format->Bmask,
        arrow_buttons_surface->format->Amask
    );
    SDL_BlitSurface(arrow_buttons_surface, &srcBackward, backwardSurface, NULL);
    // Convert The New Surfaces into Textures
    this->forward_arrow_texture = SDL_CreateTextureFromSurface(this->renderer, forwardSurface);
    this->backward_arrow_texture = SDL_CreateTextureFromSurface(this->renderer, backwardSurface);
    SDL_FreeSurface(arrow_buttons_surface);
    SDL_FreeSurface(forwardSurface);
    SDL_FreeSurface(backwardSurface);
}
void StartGame::loadStartButtonTexture()
{
    SDL_Surface* start_button_surface = IMG_Load((this->currentPath + "../../Assets/UI/start_button.png").c_str());
    if(!start_button_surface)
    {
        std::cout << "Failed to load start button image" << std::endl;
        return;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    this->start_screen_texture = SDL_CreateTextureFromSurface(this->renderer, start_button_surface);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_FreeSurface(start_button_surface);
    if (!this->start_screen_texture)
    {
        std::cout << "Failed to create texture from surface" << std::endl;
        this->keep_window_open = false;
        return;
    }
}

void StartGame::handleEvent(SDL_Event e)
{
    if (e.type == SDL_QUIT) {
        this->keep_window_open = false;
        // Also Exit The Game
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        exit(0);
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
    
        if(this->checkButtonClicked(this->start_button_rect, mouseX, mouseY))
        {
            std::cout << "Start Button Clicked" << std::endl;
            this->keep_window_open = false;
            // Close This Window And Everything Related To It
            SDL_DestroyRenderer(this->renderer);
            SDL_DestroyWindow(this->window);
            SDL_Quit();
        }
        if(this->checkButtonClicked(this->forward_arrow_rect, mouseX, mouseY))
        {
            std::cout << "Forward Arrow Clicked" << std::endl;
            this->selected_character = (this->selected_character + 1) % this->sprites.size();
        }
        if(this->checkButtonClicked(this->backward_arrow_rect, mouseX, mouseY))
        {
            std::cout << "Backward Arrow Clicked" << std::endl;
            this->selected_character = (this->selected_character - 1) % this->sprites.size();
        }
    }
}

bool StartGame::checkButtonClicked(SDL_Rect buttonRect, int mouseX, int mouseY)
{
    return mouseX >= buttonRect.x && mouseX <= (buttonRect.x + buttonRect.w) &&
        mouseY >= buttonRect.y && mouseY <= (buttonRect.y + buttonRect.h);
}


StartGame::StartGame() {
    // Get Current File Path
    this->currentPath = __FILE__;
    this->currentPath = this->currentPath.substr(0, this->currentPath.find_last_of("/\\") + 1);
    
    this->character_textures = std::vector<SDL_Texture*>();
    this->sprites = {
        {"Girl", this->currentPath + "../../Assets/char_idle2.png", 0, 7},
        {"Boy" , this->currentPath + "../../Assets/char_idle1.png", 0, 5},
    };

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to Initialize SDL2 Library" << std::endl;
        return;
    }
    this->initWindow();
    this->initRenderer();
    this->keep_window_open = true;
}

void StartGame::initWindow()
{
    this->window = SDL_CreateWindow(
        "Start Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!this->window)
    {
        printf("Failed to make window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
}

void StartGame::initRenderer()
{
    this->renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if(!this->renderer)
    {
        printf("Failed To Get the surface from the window");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
}
