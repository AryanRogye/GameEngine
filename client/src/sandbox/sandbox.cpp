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

SandBox::~SandBox() { this->cleanup(); }

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
        this->renderCloseWindow();
        this->renderOpenFile();
        this->renderFiles();
        this->renderFileClicked();
        this->loadTileSet();

        SDL_RenderPresent(this->renderer);
        SDL_Delay(16);
    }
    this->cleanup();
}

void SandBox::loadTileSet()
{
    // Basically We Will Close The File System Window
    if (tileSetloaded)
    {
        this->showFiles = false;
        // Wel Will In Turn Dispplay The Image Here
        fileConfirmedRect = { 
            this->openFileButton.x, this->openFileButton.y + this->openFileButton.h + 10,
            300, 300
        };
        SDL_RenderCopy(this->renderer, this->fileClickedTexture, NULL, &this->fileConfirmedRect);
        // Draw THe Pixel Size For the User to confirm on the screen
        this->pixelWidthRect = {
            this->openFileButton.x, this->openFileButton.y + this->openFileButton.h + 10 + 300 + 10,
            150, 30
        };
        this->pixelHeightRect = {
            this->openFileButton.x + 150, this->openFileButton.y + this->openFileButton.h + 10 + 300 + 10,
            150, 30
        };
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(this->renderer, &this->pixelWidthRect);
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(this->renderer, &this->pixelHeightRect);

        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            std::to_string(this->pixel_width), 
            this->pixelWidthRect.x + 5,
            this->pixelWidthRect.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            std::to_string(this->pixel_height), 
            this->pixelHeightRect.x + 5,
            this->pixelHeightRect.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
        // Once Confirmed we dont need to show the rest
        if (this->confirmed) return;
        /** 
            For Width Should Half of the pixel width
        **/
        this->decreasePixelWidth = {
            this->pixelWidthRect.x,
            this->pixelWidthRect.y + 2 + 30,
            150/2, 30
        };
        this->increasePixelWidth = {
            this->pixelWidthRect.x + 150/2,
            this->pixelWidthRect.y + 2 + 30,
            150/2, 30
        };
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(this->renderer, &decreasePixelWidth);
        SDL_RenderFillRect(this->renderer, &increasePixelWidth);
        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            "-", 
            decreasePixelWidth.x + 5,
            decreasePixelWidth.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            "+", 
            increasePixelWidth.x + 5,
            increasePixelWidth.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
        // Calc width from the start 
        int w = this->pixelWidthRect.x + 2* (150/2);
        this->decreasePixelHeight = {
            w, this->pixelHeightRect.y + 2 + 30,
            150/2, 30
        };
        
        this->increasePixelHeight = {
            w + 150/2, this->pixelHeightRect.y + 2 + 30,
            150/2, 30
        };
        // Draw Red
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(this->renderer, &increasePixelHeight);
        SDL_RenderFillRect(this->renderer, &decreasePixelHeight);
        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            "-", 
            decreasePixelHeight.x + 5,
            decreasePixelHeight.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            "+", 
            increasePixelHeight.x + 5,
            increasePixelHeight.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
        // Draw Confirm Pixel Size
        this->confirmPixelSize = {
            this->pixelWidthRect.x, this->pixelWidthRect.y + 2 + 30 + 30,
            300, 30
        };
        // Make Purple
        SDL_SetRenderDrawColor(this->renderer, 255, 0, 255, 255);
        SDL_RenderFillRect(this->renderer, &confirmPixelSize);
        UI::renderTextAtPosition(
            this->renderer, 
            this->font_texture, 
            this->fonts, 
            "Confirm", 
            confirmPixelSize.x + 5,
            confirmPixelSize.y + 2,
            FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1
        );
    }
}

void SandBox::renderFileClicked()
{
    if (!this->askUserToConfirmFile) return;
    int w, h;
    SDL_QueryTexture(this->fileClickedTexture, NULL, NULL, &w, &h);
    this->fileClickedRect = { file_clicked_x, file_clicked_y, w * 2, h * 2 };
    SDL_RenderCopy(this->renderer, this->fileClickedTexture, NULL, &this->fileClickedRect);

    /** Render Confirm Options As Well **/
    this->confirmFileButton = { this->confirm_file_button_x, (file_clicked_y + h*2) + 10, w*2, this->image_file_button_height };
    this->cancelFileButton  = { this->cancel_file_button_x , (file_clicked_y + h*2) + 20 + this->image_file_button_height, w*2, this->image_file_button_height };

    if (!this->showConfirmAndCancel) return;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(this->renderer, &this->confirmFileButton);
    UI::renderTextAtPosition(this->renderer, this->font_texture, this->fonts, "Confirm", this->confirmFileButton.x + 5, this->confirmFileButton.y + 2, FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1);
    SDL_SetRenderDrawColor(this->renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, &this->cancelFileButton);
    UI::renderTextAtPosition(this->renderer, this->font_texture, this->fonts, "Cancel", this->cancelFileButton.x + 5, this->cancelFileButton.y + 2, FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1);
}

void SandBox::renderCloseWindow()
{
    // We Just Wanna Show A Very Small Button in the bottom left corner to clsoe out of the window
    this->closeWindowButton.x = this->c_window_button_x;
    this->closeWindowButton.y = this->c_window_button_y;
    this->closeWindowButton.w = this->c_window_button_width;
    this->closeWindowButton.h = this->c_window_button_height;

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(this->renderer, &this->closeWindowButton);

    UI::renderTextAtPosition(this->renderer, this->font_texture, this->fonts, "X", this->closeWindowButton.x + 5, this->closeWindowButton.y + 2, FONT_WIDTH+3, FONT_HEIGHT+2, 2, false, 1);
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
        if (this->checkButtonClicked(this->closeWindowButton, mouseX, mouseY))
        {
            this->isRunning = false;
        }
        if (this->checkButtonClicked(this->confirmFileButton, mouseX, mouseY))
        {
            this->fileConfirmedRect = this->fileClickedRect;
            this->askUserToConfirmFile = false;
            this->showConfirmAndCancel = false;
            this->tileSetloaded = true;
        }
        if (this->checkButtonClicked(this->cancelFileButton, mouseX, mouseY))
        {
            this->askUserToConfirmFile = false;
            this->showConfirmAndCancel = false;
        }
        // We need to check if the mouse is in any of the file rects
        if (this->showFiles) 
        {
            this->handleFileClicked(mouseX, mouseY);
        }


        // Check For The Pixel Width and Height Buttons and change the values
        if (this->checkButtonClicked(this->decreasePixelWidth, mouseX, mouseY))
        {
            this->pixel_width = std::max(this->min, this->pixel_width - 1);
        }
        // Check for increase pixel width
        if (this->checkButtonClicked(this->increasePixelWidth, mouseX, mouseY))
        {
            this->pixel_width = std::min(this->max, this->pixel_width + 1);
        }
        // Check For The Pixel Height Buttons and change the values
        if (this->checkButtonClicked(this->decreasePixelHeight, mouseX, mouseY))
        {
            this->pixel_height = std::max(this->min, this->pixel_height - 1);
        }
        // Check for increase pixel Height
        if (this->checkButtonClicked(this->increasePixelHeight, mouseX, mouseY))
        {
            this->pixel_height = std::min(this->max, this->pixel_height + 1);
        }
        if (this->checkButtonClicked(this->confirmPixelSize, mouseX, mouseY))
        {
            // We Wont Allow Pixel Size To get Changed Again 
            std::cout << "Pixel Size Confirmed: " << this->pixel_width << "x" << this->pixel_height << std::endl;
            this->confirmed = true;
        }
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

/** 
This Function Handles THe Rendering Of The Image of the file Clicked
**/
void SandBox::renderFileClicked(std::string fileName)
{
    std::cout << "File Clicked: " << fileName << std::endl;
    /** 
        Basically This is alot but we have to load in the surface and then load in the texture
        for every file that the user clicks on
    **/
    /** 
        first make srue that the file is an image file
    **/
    SDL_Surface* surface = IMG_Load((this->currentPath + "../../Assets/" + fileName).c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return;
    }
    this->fileClickedTexture = SDL_CreateTextureFromSurface(this->renderer, surface);
    if (!this->fileClickedTexture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    SDL_FreeSurface(surface);
    /** 
    This let the program know that the texture is loaded and to ask
    the user if they want to open the file
    **/
    this->askUserToConfirmFile = true;
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
    if (this->showFiles || this->askUserToConfirmFile) {
        this->showFiles = false;
        this->askUserToConfirmFile = false;
        this->showConfirmAndCancel = false;
        this->tileSetloaded = false;
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
        this->showConfirmAndCancel = true;
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
    /*SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);*/

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
