#include "utils/camera.h"


Camera::Camera(Entity *entity)
: entity(entity) {

    if (entity)
    {
        this->x = this->entity->getX();
        this->y = this->entity->getY();
    } 
    else 
    {
        this->x = 0;
        this->y = 0;
    }

    this->width = 800;
    this->height = 600;
    this->zoom = 1.0;
}

/** 

This Function is used to follow the attached entity

**/
void Camera::update(int width, int height, float gameScale)
{
    if (!this->entity) return;

    // we wanna follow the entity so we need to update the camera position

    // so in most cases the width and the height is the viewport size

    // Get entity's center position (assuming entity position is top-left)
    float entityCenterX = this->entity->getX() + (this->entity->getWidth() / 2);
    float entityCenterY = this->entity->getY() + (this->entity->getHeight() / 2);
    
    // Calculate camera position to center the entity
    // Divide by gameScale if your entity positions are in scaled world coordinates
    this->x = entityCenterX - (width / (2 * gameScale));
    this->y = entityCenterY - (height / (2 * gameScale));

    // dont allow camera to go out of the world
    if (this->x < 0) this->x = 0;
    if (this->y < 0) this->y = 0;
    if (this->x > 800 - width) this->x = 800 - width;
    if (this->y > 600 - height) this->y = 600 - height;
}

/** 

    Check Function Definition in utils/camera.h for more details
    on how this is used

**/
void Camera::attachEntity(Entity *entity)
{
    this->entity = entity;
}
void Camera::detachEntity()
{
    this->entity = nullptr;
}


// Getters
float Camera::getX() { return this->x; }
float Camera::getY() { return this->y; }
float Camera::getWidth() { return this->width; }
float Camera::getHeight() { return this->height; }
float Camera::getZoom() { return this->zoom; }
// Setters
void Camera::setX(float x) { this->x = x; }
void Camera::setY(float y) { this->y = y; }
void Camera::setWidth(float width) { this->width = width; }
void Camera::setHeight(float height) { this->height = height; }
void Camera::setZoom(float zoom) { this->zoom = zoom; }
