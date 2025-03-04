#include "debug_gui.h"


static int numDeleted = 0;
static std::string oldOnboardingSpritePath = "";
static std::string onboardingSpritePath = "";
static SDL_Texture *onboardingTexture = nullptr;
static bool createdOnbroardingSurface = false;

static int scaleXColors[3] = { 255, 0, 0 };    // Red For X
static int scaleYColors[3] = { 0, 255, 0 };    // Green For Y

static bool showFrameXColors = false;
static bool showFrameYColors = false;

static std::vector<int> activeSpriteGrids(allErrorCodes.size(), 0);
static std::vector<int> activeSpriteRename(allErrorCodes.size(), 0);
static std::vector<std::string> renameBuffers;


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
            // resize the rename buffers
            if (renameBuffers.size() != entity->getSprite()->getSpritePaths().size())
            {
                renameBuffers.resize(entity->getSprite()->getSpritePaths().size());
            }
            ImGui::Text("Current Sprites: %lu", entity->getSprite()->getSpritePaths().size());
            ImGui::Separator();

            std::vector<Sprites::Sprite> spritePaths = entity->getSprite()->getSpritePaths();

            static int selected = -1;
            static bool showDeletePopup = false;
            static float currentSpritesZoom = 1.255f;

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
                // capture image position after rendering it onto the screen
                ImVec2 imagePos = ImGui::GetItemRectMin(); // Get the top-left of the drawn image

                // =====================================================================================================================
                // Displaying The Grid On Top of the Sprite
                // =====================================================================================================================
                bool checked = activeSpriteGrids[i];  // Convert int to bool
                if (checked)
                {
                    ImDrawList* drawList = ImGui::GetWindowDrawList();

                    int numFramesX = entity->getSprite()->getSpritePaths()[i].numFramesX;
                    int numFramesY = entity->getSprite()->getSpritePaths()[i].numFramesY;

                    float cellWidth  = imageSize.x / numFramesX; // Width of each frame 
                    float cellHeight = imageSize.y / numFramesY; // Height of each frame

                    // Draw vertical lines
                    for (int i = 1; i < numFramesX; i++) {
                        float x = imagePos.x + i * cellWidth;
                        drawList->AddLine(ImVec2(x, imagePos.y), ImVec2(x, imagePos.y + imageSize.y), IM_COL32(255, 255, 255, 255));
                    }

                    // Draw horizontal lines
                    for (int j = 1; j < numFramesY; j++) {
                        float y = imagePos.y + j * cellHeight;
                        drawList->AddLine(ImVec2(imagePos.x, y), ImVec2(imagePos.x + imageSize.x, y), IM_COL32(255, 255, 255, 255));
                    }
                }


                ImGui::NextColumn();

                // the path will be hella long we wanna only get the last /...... the ......
                size_t lastSlash = spritePaths[i].path.find_last_of('/');
                std::string name =  spritePaths[i].path.substr(lastSlash + 1);

                // =====================================================================================================================
                // Display the name (scaled)
                // =====================================================================================================================
                bool rename = activeSpriteRename[i];
                if (rename)
                {
                    if (renameBuffers[i].empty()) {
                        renameBuffers[i] = name; // Initialize buffer with the current name
                    }
                    char buffer[256];
                    strncpy(buffer, renameBuffers[i].c_str(), sizeof(buffer));
                    buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

                    if (ImGui::InputText(("##Rename" + std::to_string(i)).c_str(), buffer, IM_ARRAYSIZE(buffer))) {
                        renameBuffers[i] = buffer; // Persist the new name
                    }
                } else {
                    ImGui::Text("%s", name.c_str());
                    // add a small collapsing header to show the full path of the sprite
                }
                ImGui::SameLine();
                if (!rename)
                {
                    // =====================================================================================================================
                    // Displaying Rename if the user isnt renaming
                    // =====================================================================================================================
                    if (ImGui::Button((" Rename## " + std::to_string(i)).c_str(), ImVec2(50 * currentSpritesZoom, 20 * currentSpritesZoom)))
                    {
                        activeSpriteRename[i] = 1;
                    }
                } 
                else 
                {
                    // =====================================================================================================================
                    // Displaying Cancel if the user is renaming
                    // =====================================================================================================================
                    if (ImGui::Button((" Cancel## " + std::to_string(i)).c_str(), ImVec2(50 * currentSpritesZoom, 20 * currentSpritesZoom)))
                    {
                        activeSpriteRename[i] = 0;
                    }
                    ImGui::SameLine();
                    // =====================================================================================================================
                    // Displaying Done if the user is renaming
                    // =====================================================================================================================
                    // TODO: Fix this its not doning right and not saving the name fully yet
                    if (ImGui::Button((" Done## " + std::to_string(i)).c_str(), ImVec2(50 * currentSpritesZoom, 20 * currentSpritesZoom)))
                    {
                        activeSpriteRename[i] = 0;
                        // make sure its not empty
                        if (!renameBuffers[i].empty()) 
                        {
                            std::string oldName = spritePaths[i].path;
                            std::string newName = renameBuffers[i];

                            // Construct new full path
                            std::string newPath = spritePaths[i].path.substr(0, lastSlash + 1) + newName;
                
                            // Update the sprite object
                            entity->getSprite()->changeSpritePathName(i, newPath);
                            if (!changeFileName(oldName, newPath))
                            {
                                // change everything back to how it was
                                entity->getSprite()->changeSpritePathName(i, oldName);
                            } else {
                                if (!saveSpriteConfig(entity->getSprite(), newPath, i))
                                {
                                    entity->getSprite()->changeSpritePathName(i, oldName);
                                }
                            }
                        }
                    }
                }
                if (!rename)
                {
                    ImGui::SameLine();
                    // =====================================================================================================================
                    // Displaying Delete if the user isnt renaming
                    // =====================================================================================================================
                    if (ImGui::Button((" Delete## " + std::to_string(i)).c_str(), ImVec2(50 * currentSpritesZoom, 20 * currentSpritesZoom)))
                    {
                        selected = i;
                        showDeletePopup = true;
                    }
                    // =====================================================================================================================
                    // Displaying Full Path if the user isnt renaming
                    // =====================================================================================================================
                    if (ImGui::TreeNode("Full Path"))
                    {
                        ImGui::PushTextWrapPos(0.0f);  // Prevents wrapping and clipping
                        ImGui::Text("%s", spritePaths[i].path.c_str());
                        ImGui::PopTextWrapPos();       // Restore wrapping behavior
                        ImGui::TreePop();

                    }
                    ImGui::Spacing();
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
                    // add a checkbox to allow the user to show the grid on the sprite
                    ImGui::SameLine();
                }

                // =====================================================================================================================
                // Displaying Active Grids
                // =====================================================================================================================
                if (ImGui::Checkbox(("Show Grid##" + std::to_string(i)).c_str(), &checked)) {
                    activeSpriteGrids[i] = checked; // Convert bool to int
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
