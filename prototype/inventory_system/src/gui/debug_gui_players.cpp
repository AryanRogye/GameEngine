#include "debug_gui.h"
#include "TSDL.h"

void DebugGUI::renderPlayerInfo(SDL_Renderer* renderer)
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
}
