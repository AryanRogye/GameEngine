#include "sandbox/sandbox.h"

bool SandBox::sdlInitialized = false;
namespace fs = std::filesystem;

SandBox::SandBox()
{
    if (!sdlInitialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }
        sdlInitialized = true;
    }
    this->currentPath = __FILE__;
    this->currentPath = currentPath.substr(0, currentPath.find_last_of("/\\") + 1);

    this->initWindow();
    this->initRenderer();
    this->isRunning = false;
    this->loadFont();
}

SandBox::~SandBox()
{
    this->cleanup();
}

void SandBox::cleanup()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void SandBox::loadSandBox()
{
    this->isRunning = true;
    SDL_Event e;
    while (this->isRunning)
    {
        while(SDL_PollEvent(&e)){ 
            this->handleEvent(e); 
            if (!this->isRunning) break;
        }
        if (!this->isRunning) break;

        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
        SDL_RenderClear(this->renderer);

        // Show Box
        this->renderOpenFile();
        this->renderFiles();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }
    this->cleanup();
}

void SandBox::renderOpenFile()
{
    this->openFileButton.x = this->o_file_button_x;
    this->openFileButton.y = this->o_file_button_y;
    this->openFileButton.w = this->o_file_button_width;
    this->openFileButton.h = this->o_file_button_height;

    SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &this->openFileButton);
    
    // Make Whie Text
    /*SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);*/
    UI::renderTextAtPosition(this->renderer, this->font_texture, this->fonts, "Open File", this->openFileButton.x + 5, this->openFileButton.y + 10, FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1);
}

void SandBox::handleEvent(SDL_Event e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;

        if (this->checkButtonClicked(this->openFileButton, mouseX, mouseY))
        {
            this->handleOpenFileButtonClick();
        }

        // We need to check if the mouse is in any of the file rects
        this->handleFileClicked(mouseX, mouseY);
    }
    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            std::cout << "SDL_WINDOWEVENT_CLOSE event received." << std::endl;
            this->isRunning = false;
        }
    }
}

void SandBox::handleFileClicked(int mouseX, int mouseY)
{
    if (this->showFiles) 
    {
        for (const auto& rect : this->fileRects) {
            if (this->checkButtonClicked(rect, mouseX, mouseY)) {
                int startY = this->openFileButton.y + this->openFileButton.h + 10;
                for (const auto& [fileName, _] : fileMap) {
                    if (rect.y == startY) {
                        this->renderFileClicked(fileName);
                        break;
                    }
                    startY += 30;
                }
            }
        }
    }
}

void SandBox::renderFileClicked(std::string fileName)
{
    std::cout << "File Clicked: " << fileName << std::endl;
}

void SandBox::renderFiles()
{
    if (!this->showFiles) return;
    int startY = this->openFileButton.y + this->openFileButton.h + 10;
    int startX = this->openFileButton.x + 5;

    this->fileRects.resize(this->fileMap.size());
    
    // Debug print for Hello text
    
    int i = 0;
    for (const auto& [fileName, _] : fileMap) {
        if (i >= this->fileRects.size()) break;

        // Convert filename to lowercase for rendering
        std::string lowerFileName = fileName;
        std::transform(lowerFileName.begin(), lowerFileName.end(), lowerFileName.begin(), ::tolower);
        
        this->fileRects.at(i).x = startX;
        this->fileRects.at(i).y = startY;
        this->fileRects.at(i).w = this->openFileButton.w - 10;
        this->fileRects.at(i).h = 20;

        SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(this->renderer, &this->fileRects.at(i));
        UI::renderTextAtPosition(this->renderer, this->font_texture, this->fonts, lowerFileName, startX, startY+2, FONT_WIDTH + 1, FONT_HEIGHT + 1, 1, false, 1);

        
        /*SDL_RenderDrawRect(this->renderer, &this->fileRects.at(i));*/
        
        startY += 30;
        i++;
    }
}

/** Need To Open All The Files **/
void SandBox::handleOpenFileButtonClick()
{
    if (this->showFiles) {
        this->showFiles = false;
        this->fileRects.clear();
        return;
    }
    std::string fol = this->currentPath + "../../Assets/";
    try {
        if (!fs::exists(fol)) {
            std::cerr << "Directory does not exist: " << fol << std::endl;
            return;
        }
        for (const auto& entry : fs::recursive_directory_iterator(fol)) {
            if (fs::is_regular_file(entry.status())) {
                std::string relativePath = entry.path().lexically_relative(fol).string();
                this->fileMap[relativePath] = 'f';
            }
        }
        this->showFiles = true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

}

bool SandBox::checkButtonClicked(SDL_Rect buttonRect, int mouseX, int mouseY)
{
    return mouseX >= buttonRect.x && mouseX <= (buttonRect.x + buttonRect.w) &&
    mouseY >= buttonRect.y && mouseY <= (buttonRect.y + buttonRect.h);
}

void SandBox::initWindow()
{
    this->window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!this->window)
    {
        printf("Failed To Make SandBox Window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
}

void SandBox::initRenderer()
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

extern "C" SandBox* createSandBox()
{
    return new SandBox();
}

extern "C" void runSandBox(void* instance)
{
    SandBox* sandbox = static_cast<SandBox*>(instance);
    sandbox->loadSandBox();
    delete sandbox;  // Add this line to properly clean up
}

void SandBox::loadFont()
{
    /** Font Is Just a Sprite Sheet so we can just use the sprite class **/
    Sprite::fillRectVector(this->fonts,128,128, 8,7);
    std::cout << "Number of font rectangles: " << this->fonts.size() << std::endl;
    
    /** Load Into the texture **/
    std::cout << "FontPath : " << this->currentPath + "../../Assets/font.png" << std::endl;
    Texture::loadTexture(this->currentPath + "../../Assets/font.png", &this->font_texture, this->renderer);
    if (!this->font_texture) {
        std::cerr << "Failed to load font texture: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Font texture loaded successfully!" << std::endl;
        int w, h;
        SDL_QueryTexture(this->font_texture, NULL, NULL, &w, &h);
        std::cout << "Font texture dimensions: " << w << "x" << h << std::endl;
    }
}
