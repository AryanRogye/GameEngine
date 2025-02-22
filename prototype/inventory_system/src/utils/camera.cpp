#include "utils/camera.h"
#include "entity/player.h"


Camera::Camera(Player* player)
: player(player) {

    this->x = this->player->getX();
    this->y = this->player->getY();

    this->width = 800;
    this->height = 600;
    this->zoom = 1.0;
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
