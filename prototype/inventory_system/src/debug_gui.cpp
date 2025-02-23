// DebugGUI.cpp
#include "debug_gui.h"
#include "comfy_lib.h"
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


void DebugGUI::addDebugLog(const std::string& log, bool copyClipboard ,const std::string& label)
{
    std::string toAdd = "";
    if (label != "")
    {
        toAdd += "(" + label + ") ";
    }
    toAdd += log;

    const std::string timestamp = getTimeStamp();
    guiValues.debugLogs.emplace_back(timestamp + " | " + toAdd, copyClipboard);
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
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    // =====================================================================================================================
    // Mouse Position
    // =====================================================================================================================
    ImGui::Text("(%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);

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
            for (size_t i = 0; i < guiValues.debugLogs.size(); ++i)
            {
                const auto &[logText, copy] = guiValues.debugLogs[i];
                // If the log should be copied, show the button
                ImGui::TextWrapped("%s", logText.c_str());
                if (copy)
                {
                    std::string buttonText = "Copy##" + std::to_string(i);
                    if (ImGui::Button(buttonText.c_str()))
                    {
                        // Use SDL clipboard for other platforms
                        if (SDL_SetClipboardText(logText.c_str()) == 0)
                        {
                            std::cout << "✅ Successfully copied!" << std::endl;
                        }
                        else
                    {
                            std::cerr << "❌ SDL Clipboard Error: " << SDL_GetError() << std::endl;
                        }
                        addDebugLog("Copied to clipboard", false);
                    }
                }         
            }
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Map Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Map"))
        {
            // =====================================================================================================================
            // Display Map Name
            // =====================================================================================================================
            // trim just till the last slash
            size_t lastSlash = guiValues.mapName.find_last_of('/');
            std::string name =  guiValues.mapName.substr(lastSlash + 1);
            ImGui::Text("Map Name: %s", name.c_str());

            // =====================================================================================================================
            // Change Map
            // =====================================================================================================================
            if (ImGui::Button("Change Map (New Json)"))
            {
                std::string path;
                #if defined(__APPLE__)
                    char buffer[1024]; // Buffer to store path
                    FILE* pipe = popen("osascript -e 'POSIX path of (choose file with prompt \"Select a Project Folder:\")'", "r");
                    if (pipe) {
                        if(fgets(buffer, sizeof(buffer), pipe))
                        {
                            buffer[strcspn(buffer, "\n")] = 0;
                            path = buffer;
                        }
                        pclose(pipe);
                    }
                #endif
                    if (!path.empty()) {
                        loadMapConfigs(path);
                    }
            }
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Layers Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Layers"))
        {
            // =====================================================================================================================
            // Toggle Layers
            // =====================================================================================================================
            if (ImGui::Button("Toggle Layers"))
            {
                guiValues.showLayerInfo = !guiValues.showLayerInfo;
            }
            ImGui::SameLine();
            ImGui::Text(guiValues.showLayerInfo ? "On" : "Off");
            ImGui::Checkbox("Color for different Layers", &guiValues.colorForDifferentLayer);

            // =====================================================================================================================
            // Layer Visibility
            // =====================================================================================================================
            if (ImGui::TreeNode("Layer Visibility"))
            {
                for (size_t i = 0; i < guiValues.layerInfo.size(); i++)
                {
                    std::string buttonLabel = (guiValues.layerInfo[i] ? "🟢 " : "🔴 ") + std::to_string(i);
                    if (ImGui::Button(buttonLabel.c_str()))
                    {
                        guiValues.layerInfo[i] = !guiValues.layerInfo[i];
                    }

                    if (i % 4 != 3) ImGui::SameLine(); // Arrange in rows of 4
                }
                ImGui::TreePop();
            }
            ImGui::Separator();

            // =====================================================================================================================
            // Layer Settings
            // =====================================================================================================================
            ImGui::Text(
                guiValues.currentMouseLayer == -1 
                ? "Not Set/Implimented Yet"
                : "Current Mouse Layer: %d",
                guiValues.currentMouseLayer
            );
            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Textures Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Textures"))
        {
            // =====================================================================================================================
            // Image of Tile
            // =====================================================================================================================
            if (guiValues.currentTileTexture)
            {
                ImGui::Image((ImTextureID)guiValues.currentTileTexture, ImVec2(100, 100));
            }
            // =====================================================================================================================
            // Texture Name
            // =====================================================================================================================
            ImGui::Text(
                guiValues.currentTextureName == "" 
                ? "Not Set/Implimented Yet"
                : "Texture: %s",
                guiValues.currentTextureName.c_str()
            );
            // =====================================================================================================================
            // Texture Settings
            // =====================================================================================================================
            ImGui::Checkbox("Color for different textures", &guiValues.colorForDifferentTexture);
            if (guiValues.colorForDifferentTexture)
            {
                guiValues.showLayerInfo = true;
            }
            // =====================================================================================================================
            // Mouse Tile
            // =====================================================================================================================
            ImGui::Text(
                guiValues.currentMouseTileX == -1 || guiValues.currentMouseTileY == -1 
                ? "Not Set/Implimented Yet"
                : "Current Mouse Tile: (%d, %d)",
                guiValues.currentMouseTileX,
                guiValues.currentMouseTileY
            );
            ImGui::Checkbox("Show Grid Over Texture", &guiValues.drawGridOverTexture);

            ImGui::EndTabItem();
        }

        // =====================================================================================================================
        // Player Tab
        // =====================================================================================================================
        if(ImGui::BeginTabItem("Player"))
        {
            if (guiValues.player) 
            {
                // Main Player Info
                ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f),"Player Status");
                ImGui::Separator();

                // =====================================================================================================================
                // Player Position
                // =====================================================================================================================
                ImGui::Text("(%.1f, %.1f)", guiValues.player->getX(),guiValues.player->getY());

                // =====================================================================================================================
                // Player Stats
                // =====================================================================================================================
                // never rlly thought I would need this lol till i got stuck in a collision wall
                float playerX = guiValues.player->getX();
                float playerY = guiValues.player->getY();
                ImGui::SliderFloat("X", &playerX, 0, 2000);
                ImGui::SliderFloat("Y", &playerY, 0, 2000);
                guiValues.player->setX(playerX);
                guiValues.player->setY(playerY);


                // =====================================================================================================================
                // Player State
                // =====================================================================================================================
                switch(guiValues.player->getState())
                {
                    case Player::PlayerState::COLLIDING:
                        ImGui::Text("Colliding");
                        break;
                    case Player::PlayerState::IDLE:
                        ImGui::Text("Idle");
                        break;
                    case Player::PlayerState::WALKING:
                        ImGui::Text("Walking");
                        break;
                    case Player::PlayerState::ATTACK:
                        ImGui::Text("Attacking");
                        break;
                    default:
                        ImGui::Text("Unknown State");
                        break;
                }
                ImGui::Separator();


                // =====================================================================================================================
                // Show Player Collision status
                // =====================================================================================================================
                if (guiValues.player->getCollision())
                {
                    ImGui::Text("Collision Set");
                }
                ImGui::Separator();

                // =====================================================================================================================
                // Show Player Camera Status
                // =====================================================================================================================
                if (guiValues.player->getCamera())
                {
                    ImGui::Text("Camera Set");
                }
                ImGui::Separator();

                // =====================================================================================================================
                // Player Stats
                // =====================================================================================================================
                if (ImGui::CollapsingHeader("Stats")) 
                {
                    int health = guiValues.player->getHealth();
                    int maxHealth = guiValues.player->getMaxHealth();
                    int level = guiValues.player->getLevel();
                    int experience = guiValues.player->getExperience();

                    // =====================================================================================================================
                    // Health Progress Bar
                    // =====================================================================================================================
                    float healthPercentage = (float)health / (float)maxHealth;
                    ImGui::ProgressBar(healthPercentage, ImVec2(200, 20), "Health");
                    ImGui::SameLine();
                    ImGui::Text("(%d/%d)", health, maxHealth);

                    // =====================================================================================================================
                    // Experience Progress Bar
                    // =====================================================================================================================
                    ImGui::ProgressBar((float)experience / 100.0f, ImVec2(200, 20), "EXP");
                    ImGui::SameLine();
                    ImGui::Text("Level %d", level);

                    // =====================================================================================================================
                    // Input fields for precise adjustments
                    // =====================================================================================================================
                    ImGui::InputInt("Health", &health);
                    ImGui::InputInt("Level", &level);
                    ImGui::InputInt("Experience", &experience);

                    // Apply changes back to the player
                    guiValues.player->setHealth(health);
                    guiValues.player->setLevel(level);
                    guiValues.player->setExperience(experience);

                    // =====================================================================================================================
                    // Save Button
                    // =====================================================================================================================
                    if (ImGui::Button("Save"))
                    {
                        // Need To Implement
                        // Player should have a save function
                    }
                }

                if (ImGui::CollapsingHeader("Collision"))
                {
                    // Wanna display Collision Information we will get this instance through the player
                    Collision *collision = guiValues.player->getCollision();

                    bool showCollision = collision->getShowingCollision();
                    SDL_Color collisionColor = collision->getCollisionColor();
                    float collisionWidth = collision->getWidth();
                    float collisionHeight = collision->getHeight();

                    float xOffset = collision->getXOffset();
                    float yOffset = collision->getYOffset();

                    // =====================================================================================================================
                    // Allow Showing Collision 
                    // =====================================================================================================================
                    ImGui::Checkbox("Show Collision", &showCollision);
                    ImGui::Separator();

                    // =====================================================================================================================
                    // TileMap Information
                    // =====================================================================================================================
                    if (guiValues.player->getTileMap()->maxTileCount &&
                        guiValues.player->getTileMap()->tileHeight &&
                        guiValues.player->getTileMap()->tileWidth &&
                        !guiValues.player->getTileMap()->layers.empty()
                    )
                    {
                        ImGui::Text("Player Has Recieved TileMap");
                    }

                    // =====================================================================================================================
                    // Color Picker for Collision
                    // =====================================================================================================================
                    // float color[4] = {collisionColor.r, collisionColor.g, collisionColor.b, collisionColor.a};

                    // =====================================================================================================================
                    // Collision Size
                    // =====================================================================================================================
                    ImGui::SliderFloat("Width", &collisionWidth, 0, 2000);
                    ImGui::SliderFloat("Height", &collisionHeight, 0, 2000);
                    ImGui::Separator();
                    // =====================================================================================================================
                    // Collision Offset
                    // =====================================================================================================================
                    ImGui::SliderFloat("X Offset", &xOffset, 0, 2000);
                    ImGui::SliderFloat("Y Offset", &yOffset, 0, 2000);

                    // =====================================================================================================================
                    // Save Button
                    // =====================================================================================================================
                    if (ImGui::Button("Save"))
                    {
                        // Need To Implement
                        // Collision should have a save function
                        saveCollisionConfigs(collision);
                    }

                    // =====================================================================================================================
                    // Apply Changes
                    // =====================================================================================================================
                    collision->setShowingCollision(showCollision);
                    collision->setWidth(collisionWidth);
                    collision->setHeight(collisionHeight);
                    collision->setXOffset(xOffset);
                    collision->setYOffset(yOffset);
                    // collision.setCollisionColor({(Uint8)color[0], (Uint8)color[1], (Uint8)color[2], (Uint8)color[3]});

                    // We also wanna control the collision box
                }

                if (ImGui::CollapsingHeader("Movement")) 
                {
                    float maxSpeed = guiValues.player->getMaxSpeed();
                    float acceleration = guiValues.player->getAcceleration();
                    float friction = guiValues.player->getFriction();
                    float velocityX = guiValues.player->getVelocityX();
                    float velocityY = guiValues.player->getVelocityY();

                    // =====================================================================================================================
                    // Input fields for precise control
                    // =====================================================================================================================
                    ImGui::InputFloat("Acceleration", &acceleration);
                    ImGui::InputFloat("Max Speed", &maxSpeed);
                    ImGui::InputFloat("Friction", &friction);

                    // =====================================================================================================================
                    // Show current velocity
                    // =====================================================================================================================
                    ImGui::Text("Velocity: (%.2f, %.2f)", velocityX, velocityY);

                    // Apply changes
                    guiValues.player->setAcceleration(acceleration);
                    guiValues.player->setMaxSpeed(maxSpeed);
                    guiValues.player->setFriction(friction);
                }

                // =====================================================================================================================
                // Camera Settings
                // =====================================================================================================================
                if (ImGui::CollapsingHeader("Camera Control"))
                {
                    float x     = guiValues.player->getCamera()->getX();
                    float y     = guiValues.player->getCamera()->getY();
                    float w     = guiValues.player->getCamera()->getWidth();
                    float h     = guiValues.player->getCamera()->getHeight();
                    float zoom  = guiValues.player->getCamera()->getZoom();

                    // =====================================================================================================================
                    // Input fields for precise control
                    // =====================================================================================================================
                    ImGui::InputFloat("X##", &x);
                    ImGui::InputFloat("Y##", &y);
                    ImGui::InputFloat("Width##", &w);
                    ImGui::InputFloat("Height##", &h);
                    ImGui::InputFloat("Zoom##", &zoom);

                    // Apply changes
                    guiValues.player->getCamera()->setX(x);
                    guiValues.player->getCamera()->setY(y);
                    guiValues.player->getCamera()->setWidth(w);
                    guiValues.player->getCamera()->setHeight(h);
                    guiValues.player->getCamera()->setZoom(zoom);

                    // =====================================================================================================================
                    // Save button
                    // =====================================================================================================================
                    if (ImGui::Button("Save"))
                    {
                        // Need To Implement
                        // Camera should have a save function
                    }
                }

                // =====================================================================================================================
                // Future: Collapsible section for Sprite Control
                // =====================================================================================================================
                renderEntitySpriteOptions(guiValues.player, renderer);
              }
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
            ImGui::BeginChild("SpritesChild", ImVec2(0, 300), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

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
