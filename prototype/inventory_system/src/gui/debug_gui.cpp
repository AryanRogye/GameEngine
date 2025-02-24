// DebugGUI.cpp
#include "debug_gui.h"
#include "imgui.h"
#include "utils/collision.h"
#include "TSDL.h"
#include "utils/sprite.h"
#include <SDL_render.h>
#include <SDL_surface.h>
#include <cstddef>
#include <type_traits>

DebugGUI::GUIValues DebugGUI::guiValues;
static std::string oldOnboardingSpritePath = "";
static std::string onboardingSpritePath = "";
static int numDeleted = 0;
static SDL_Texture *onboardingTexture = nullptr;
static bool createdOnbroardingSurface = false;

static int scaleXColors[3] = { 255, 0, 0 };    // Red For X
static int scaleYColors[3] = { 0, 255, 0 };    // Green For Y

static bool showFrameXColors = false;
static bool showFrameYColors = false;

void DebugGUI::showSelectedSDLTexture(SDL_Texture* texture, int x, int y, int w, int h)
{
    if (!texture) return;

    SDL_Renderer* renderer = SDL_GetRenderer(SDL_GetWindowFromID(1));
    if (!renderer) return;

    // Cleanup previous texture
    if (guiValues.currentTileTexture) 
    {
        SDL_DestroyTexture(guiValues.currentTileTexture);
        guiValues.currentTileTexture = nullptr;
    }

    // Create a sub-texture to store the portion
    SDL_Texture* subTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    SDL_SetRenderTarget(renderer, subTexture);

    SDL_Rect srcRect = {x, y, w, h};
    SDL_Rect destRect = {0, 0, w, h};
    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);

    SDL_SetRenderTarget(renderer, nullptr);

    // Store the new texture for ImGui
    guiValues.currentTileTexture = subTexture;
}
void DebugGUI::ApplyGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f; // Rounded corners
    style.WindowRounding = 10.0f;
    style.FramePadding = ImVec2(5, 5);
    style.ItemSpacing = ImVec2(8, 8);

    ImGui::StyleColorsDark(); // Base theme
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.6f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.7f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.2f, 0.4f, 0.8f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.5f, 0.9f, 1.0f);
}
void DebugGUI::Init(SDL_Window* window, SDL_Renderer* renderer)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    ApplyGuiStyle();

    io.Fonts->AddFontFromFileTTF("/Library/Fonts/SF-Pro.ttf", 16.0f);
    ImGui_ImplSDLRenderer2_CreateFontsTexture();
}


void DebugGUI::Render(SDL_Renderer* renderer)
{
    if (!guiValues.toggleGui) return;

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    ImGui::Begin("Debug Window", nullptr);
    // =====================================================================================================================
    // Header
    // =====================================================================================================================
    ImGui::BeginChild("Header", ImVec2(0, 150), true);
    // =====================================================================================================================
    // FPS Display
    // =====================================================================================================================
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5)); // Adjust spacing
    ImGui::Columns(2);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::NextColumn();

    // =====================================================================================================================
    // Monitor Display
    // =====================================================================================================================
    ImGui::Text("Monitor Resolution: %dx%d", guiValues.monitorWidth, guiValues.monitorHeight);
    ImGui::Columns(1);
    ImGui::PopStyleVar(); // Reset spacing

    // =====================================================================================================================
    // Mouse Position
    // =====================================================================================================================
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5)); // Adjust spacing
    ImGui::Columns(2);
    ImGui::Text("(%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
    ImGui::NextColumn();

    // =====================================================================================================================
    // Vsync Information
    // =====================================================================================================================
    ImGui::Text("Vsync: %s", guiValues.vsync ? "Enabled" : "Disabled");
    ImGui::Columns(1);
    ImGui::PopStyleVar(); // Reset spacing

    // =====================================================================================================================
    // Map Scale Slider
    // =====================================================================================================================
    ImGui::SliderFloat("Map Scale", guiValues.mapScale, 1.0f, 10.0f);

    // =====================================================================================================================
    // Recompile & Restart Button
    // =====================================================================================================================
    if (ImGui::Button("Recompile & Restart"))
    {
        std::cout << "🔄 Recompiling & Restarting..." << std::endl;
        system("./../compile.sh &"); // Adjust this command as needed
        exit(0);
    }
    ImGui::EndChild();

    // =====================================================================================================================
    // Debug Tabs
    // =====================================================================================================================
    if (ImGui::BeginTabBar("Debug Tabs"))
    {
        // =====================================================================================================================
        // Log Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Logs"))
        {
            renderLogs();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Map Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Map"))
        {
            renderMapInfo();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Layers Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Layers"))
        {
            renderLayerInfo();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Textures Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Textures"))
        {
            renderTexturesInfo();
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Player Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Player"))
        {
            renderPlayerInfo(renderer);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

/** 

Right now this is a player but as we add on more "human" or "person" objects itll be derived from a base class
we will read that base class cuz the base class is what is gonna have the sprite later on but tbh i'm too lazy too
implement that right now so we will just use the player class for now

**/
// TODO Impliment this for each sprite that the user already has in the path or one that was just added to it
// make sure that we check hte resize of this 
static std::vector<int> activeSpriteGrids(allErrorCodes.size(), 0);
void DebugGUI::renderEntitySpriteOptions(Entity *entity, SDL_Renderer* renderer) {
    if (ImGui::CollapsingHeader("Sprite Control")) 
    {
        // =====================================================================================================================
        // Show The Current Sprites That are in
        // =====================================================================================================================
        if (entity->getSprite())
        {
            ImGui::Text("Current Sprites");
            ImGui::Separator();

            std::vector<Sprites::Sprite> spritePaths = entity->getSprite()->getSpritePaths();

            static int selected = -1;
            static bool showDeletePopup = false;
            static float currentSpritesZoom = 1.0f;
            // =====================================================================================================================
            // Add Slider zoom controls here
            // =====================================================================================================================
            ImGui::SliderFloat("Zoom", &currentSpritesZoom, 0.1f, 10.0f);
            if (currentSpritesZoom < 0.1f) currentSpritesZoom = 0.1f;

            // Push a style for a rounded frame and a subtle background color
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
            float childWidth = 500 * currentSpritesZoom;
            ImGui::BeginChild("SpritesChild", ImVec2(childWidth, 300), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);

            // =====================================================================================================================
            // Display the sprite paths
            // =====================================================================================================================
            for (size_t i = 0; i < spritePaths.size(); i++)
            {
                ImGui::Columns(2, ("SpriteColumns##" + std::to_string(i)).c_str(), false);
                ImGui::SetColumnWidth(0, 120.0f * currentSpritesZoom); // Scale column width

                // =====================================================================================================================
                // Display the image
                // =====================================================================================================================
                ImVec2 imageSize = ImVec2(100 * currentSpritesZoom, 100 * currentSpritesZoom); // Scale images
                ImGui::Image((ImTextureID)entity->getSprite()->getTextures(i), imageSize);
                ImGui::NextColumn();

                // the path will be hella long we wanna only get the last /...... the ......
                size_t lastSlash = spritePaths[i].path.find_last_of('/');
                std::string name =  spritePaths[i].path.substr(lastSlash + 1);

                // =====================================================================================================================
                // Display the name and delete button (scaled)
                // =====================================================================================================================
                ImGui::Text("%s", name.c_str());
                ImGui::SameLine();
                if (ImGui::Button(("Delete##" + std::to_string(i)).c_str(), ImVec2(50 * currentSpritesZoom, 20 * currentSpritesZoom)))
                {
                    selected = i;
                    showDeletePopup = true;
                }

                // =====================================================================================================================
                // Displaying Width and Height
                // =====================================================================================================================
                if (
                    entity->getSprite()->getSpritePaths()[i].width && 
                    entity->getSprite()->getSpritePaths()[i].height
                )
                {
                    // Show The Width and allow the user to change it
                    ImGui::Text("Width: %d", entity->getSprite()->getSpritePaths()[i].width);
                    ImGui::SameLine();
                    ImGui::Text("Height: %d", entity->getSprite()->getSpritePaths()[i].height);
                }
                // =====================================================================================================================
                // Displaying Number of Frames X and Y
                // =====================================================================================================================
                if (
                    entity->getSprite()->getSpritePaths()[i].numFramesX &&
                    entity->getSprite()->getSpritePaths()[i].numFramesY
                )
                {
                    // Show The Number of Frames X and allow the user to change it
                    ImGui::Text("Frames X: %d", entity->getSprite()->getSpritePaths()[i].numFramesX);
                    ImGui::SameLine();
                    ImGui::Text("Frames Y: %d", entity->getSprite()->getSpritePaths()[i].numFramesY);
                }

                ImGui::Columns(1); // Reset column layout
                ImGui::Separator();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();
            // =====================================================================================================================
            // Delete Popup (Not a real delete till you save it)
            // =====================================================================================================================
            if (showDeletePopup)
            {
                ImGui::OpenPopup("Confirm Delete");
            }
            if (ImGui::BeginPopupModal("Confirm Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("If You Delete this sprite you have to save it");
                if (ImGui::Button("Ok I get it!"))
                {
                    entity->getSprite()->removeSpritePath(selected);
                    selected = -1;
                    showDeletePopup = false;
                    ImGui::CloseCurrentPopup();
                    numDeleted++;
                }
                ImGui::SameLine();
                if (ImGui::Button("Nah I dont wanna delete it"))
                {
                    selected = -1;
                    showDeletePopup = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        // =====================================================================================================================
        // Displaying the currently loading sprite
        // =====================================================================================================================
        if(!createdOnbroardingSurface && !onboardingSpritePath.empty())
        {
            // Creating the surface
            SDL_Surface *surface = IMG_Load(onboardingSpritePath.c_str());
            if (!surface)
            {
                SM_WARN("Failed to create surface: {0}");
                return;
            }
            // Creating the texture
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture)
            {
                SM_WARN("Failed to create texture: {0}");
                SDL_FreeSurface(surface);
                return;
            }
            onboardingTexture = texture;
            SDL_FreeSurface(surface);
        }
        // =====================================================================================================================
        // Checking if the image is loaded or not if it is then display it
        // =====================================================================================================================
        static float zoom = 1.0f;
        static ImVec2 panOffset = ImVec2(0.0f, 0.0f); // Panning offset

        // This is for the frame width and height
        static int framesX = 0;
        static int framesY = 0;

        if (!onboardingSpritePath.empty() && onboardingTexture) 
        {
            // =====================================================================================================================
            // Table Structure
            // =====================================================================================================================
            if (ImGui::BeginTable("Onboarding Sprite", 2, ImGuiTableFlags_Borders)) {
                // Set Up Columns
                float columnWidth = ImGui::GetWindowWidth() / 2.0f;

                ImGui::TableSetupColumn("Image", ImGuiTableColumnFlags_WidthFixed, columnWidth);
                ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                // First Column (Image)
                ImGui::TableNextColumn();

                // =====================================================================================================================
                // Mouse input for dragging (panning)
                // =====================================================================================================================
                ImGuiIO& io = ImGui::GetIO();
                if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                    panOffset.x += io.MouseDelta.x; // Move left/right
                    panOffset.y += io.MouseDelta.y; // Move up/down
                }

                // =====================================================================================================================
                // Get the scaled image size
                // =====================================================================================================================
                ImVec2 imageSize = ImVec2(100 * zoom, 100 * zoom);

                // =====================================================================================================================
                // Push offset to move the image based on panning
                // =====================================================================================================================
                ImVec2 imagePos = ImGui::GetCursorScreenPos(); // Get image position on screen
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + panOffset.x, ImGui::GetCursorPos().y + panOffset.y));

                // =====================================================================================================================
                // Display the zoomed and pannable image
                // =====================================================================================================================
                ImGui::Image((ImTextureID)onboardingTexture, imageSize);
                // set value to true
                createdOnbroardingSurface = true;

                // Slider to control the zoom
                ImGui::SliderFloat("Zoom", &zoom, 0.1f, 10.0f);
                if (zoom < 0.1f) zoom = 0.1f;


                // =====================================================================================================================
                // Draw Grid Lines Over Texture based on width and height user selects
                // =====================================================================================================================
                // 0 width and height is ok that means that the user doesnt want to see a grid
                // and thats ok cuz for example me I use strips and I wont ever need to use the height portion
                if (framesX > 0 || framesY > 0) {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    float cellWidth = imageSize.x / framesX;   // Width of each frame
                    float cellHeight = imageSize.y / framesY; // Height of each frame

                    // Draw vertical lines
                    for (int i = 1; i < framesX; i++) {
                        float x = imagePos.x + i * cellWidth;
                        if (showFrameXColors)
                        {
                            drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y), IM_COL32(int(scaleXColors[0]), int(scaleXColors[1]), int(scaleXColors[2]), 255));
                        } else {
                            drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y), IM_COL32(255, 255, 255, 255));
                        }
                    }

                    // Draw horizontal lines
                    for (int j = 1; j < framesY; j++) {
                        float y = imagePos.y + j * cellHeight;
                        if (showFrameYColors)
                        {
                            drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y), IM_COL32(scaleYColors[0], scaleYColors[1], scaleYColors[2], 255));
                        } else {
                            drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y), IM_COL32(255, 255, 255, 255));
                        }
                    }
                }

                ImGui::TableNextColumn();
                ImGui::Spacing();

                // Image Info Column
                SDL_Point size;
                SDL_QueryTexture(onboardingTexture, NULL, NULL, &size.x, &size.y);
                ImGui::Text("Image Size: %d x %d", size.x, size.y);

                if (ImGui::BeginTable("Frame Config", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame))
                {
                    // =====================================================================================================================
                    // Column Headers
                    // =====================================================================================================================
                    ImGui::TableSetupColumn("Setting");
                    ImGui::TableSetupColumn("Value");
                    ImGui::TableSetupColumn("Options");
                    ImGui::TableHeadersRow();

                    // =====================================================================================================================
                    // Width Options (Frame X)
                    // =====================================================================================================================
                    ImGui::TableNextColumn();
                    ImGui::Text("Frames X:");

                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-1);
                    // =====================================================================================================================
                    // Display of Frame X
                    // =====================================================================================================================
                    ImGui::SliderInt("##framesX", &framesX, 0, 100);

                    static float floatColorX[3] = {
                        (float)scaleXColors[0] / 255.0f,
                        (float)scaleXColors[1] / 255.0f,
                        (float)scaleXColors[2] / 255.0f
                    };

                    ImGui::TableNextColumn();
                    // =====================================================================================================================
                    // Color Picker for Frame X
                    // =====================================================================================================================
                    if (ImGui::ColorEdit3("X Color", floatColorX, ImGuiColorEditFlags_NoInputs)) {
                        scaleXColors[0] = static_cast<int>(floatColorX[0] * 255.0f);
                        scaleXColors[1] = static_cast<int>(floatColorX[1] * 255.0f);
                        scaleXColors[2] = static_cast<int>(floatColorX[2] * 255.0f);
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox("Show##frameX", &showFrameXColors);

                    // =====================================================================================================================
                    // Height Options (Frame Y)
                    // =====================================================================================================================
                    ImGui::TableNextColumn();
                    ImGui::Text("Frames Y:");

                    ImGui::TableNextColumn();
                    ImGui::SetNextItemWidth(-1);
                    // =====================================================================================================================
                    // Display of Frame Y
                    // =====================================================================================================================
                    ImGui::SliderInt("##framesY", &framesY, 0, 100);

                    static float floatColorY[3] = {
                        (float)scaleYColors[0] / 255.0f,
                        (float)scaleYColors[1] / 255.0f,
                        (float)scaleYColors[2] / 255.0f
                    };

                    ImGui::TableNextColumn();
                    // =====================================================================================================================
                    // Color Picker for Frame Y
                    // =====================================================================================================================
                    if (ImGui::ColorEdit3("Y Color", floatColorY, ImGuiColorEditFlags_NoInputs)) {
                        scaleYColors[0] = static_cast<int>(floatColorY[0] * 255.0f);
                        scaleYColors[1] = static_cast<int>(floatColorY[1] * 255.0f);
                        scaleYColors[2] = static_cast<int>(floatColorY[2] * 255.0f);
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox("Show##frameY", &showFrameYColors);

                    ImGui::EndTable();
                }
                // =====================================================================================================================
                // Save Button (Have it extend to whatever the width of the table is)
                // =====================================================================================================================
                if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth()/2, 0)))
                {

                }
                // =====================================================================================================================
                // End Of Table
                // =====================================================================================================================

                ImGui::EndTable();
            }
        }

        // =====================================================================================================================
        // Load Sprites Button
        // =====================================================================================================================
        bool showLoadPopup = false;
        if (onboardingSpritePath.empty())
        {
            if (ImGui::Button("Load Sprite")) 
            {
                // we wanna open the file up
                std::string path;
                #if defined(__APPLE__)
                    char buffer[1024]; // Buffer to store path
                    FILE *pipe = popen("osascript -e 'POSIX path of (choose file "
                                    "with prompt \"Select a Project Folder:\")'",
                                    "r");
                    if (pipe) 
                    {
                        if (fgets(buffer, sizeof(buffer), pipe)) 
                        {
                            buffer[strcspn(buffer, "\n")] = 0;
                            path = buffer;
                        }
                        pclose(pipe);
                    }
                #endif
                // This is to set the onboarding sprite to a value that we can
                // later on use to decide what we wanna do with it
                if (!path.empty()) 
                {
                    onboardingSpritePath = path;
                    showLoadPopup = true;
                    createdOnbroardingSurface = false;
                }
            }
        }

        if (onboardingSpritePath != oldOnboardingSpritePath)
        {
            oldOnboardingSpritePath = onboardingSpritePath;
        }

        // =====================================================================================================================
        // Displaying Loaded Path
        // =====================================================================================================================
        if (!onboardingSpritePath.empty())
        {
            ImGui::Text("Sprite Path: %s", onboardingSpritePath.c_str());
        }
        // =====================================================================================================================
        // save button
        // =====================================================================================================================
        if (ImGui::Button("Save Sprite?")) 
        {

            // checks to make sure no empty values are passed


            // Query texture to get size and stuff
            if (onboardingTexture && !onboardingSpritePath.empty())
            {
                SDL_Point size;
                SDL_QueryTexture(onboardingTexture, NULL, NULL, &size.x, &size.y);


                // Attempt to create to a sprite object
                Sprites::Sprite sprite;
                sprite.path = onboardingSpritePath;
                sprite.numFramesX = framesX;
                sprite.numFramesY = framesY;
                sprite.width = size.x;
                sprite.height = size.y;
                sprite.currentFrame = 0;
                sprite.texture = onboardingTexture;

                entity->getSprite()->addSprite(sprite);
            }

            // TODO : Need to also pass in a sprite object instead of just the path to save the sprite
            // I want the user to be able to have the ability to either configure the sprite now or later
            // but cannot apply to the entity until its fully configured
            saveSpritesConfigs(entity->getSprite());
            onboardingSpritePath = "";
            numDeleted = 0;
        }
        ImGui::Text("Ready To Delete %d Sprites", numDeleted);

        // =====================================================================================================================
        // Popup to remind to save if wanna load
        // =====================================================================================================================
        if (showLoadPopup)
        {
            ImGui::OpenPopup("Confirm Load");
        }
        if (ImGui::BeginPopupModal("Confirm Load", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("If You Load this sprite you have to save it");
            if (ImGui::Button("Ok Bro Stop Telling Me!"))
            {
                showLoadPopup = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void DebugGUI::SetMapName(const std::string& mapName) { guiValues.mapName = mapName; }
void DebugGUI::SetPlayer(Player* player) { guiValues.player = player; }
void DebugGUI::setMapScale(float *scale) { guiValues.mapScale = scale; }
