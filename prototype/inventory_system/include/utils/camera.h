#pragma once

#include "entity/entity.h"
#ifndef CAMERA_H
#define CAMERA_H

/** 

    Camera is attached to a "Player or Person" Object

    This is just in case we ever wanna inspect a villager or zombie and we need to set the camera
    to that object.

    In Something like godot or unity the camera in a 2D scene is represented as a rectangle

**/

// Make Sure to include player in implimentation file cuz it'll be a circular import
class Player;


class Camera
{
private:
    Entity *entity;

    float x;
    float y;
    float width;
    float height;
    float zoom;

public:
    Camera(Entity *entity = nullptr);

    // Getters
    float getX();
    float getY();
    float getWidth();
    float getHeight();
    float getZoom();
    // Setters
    void setX(float x);
    void setY(float y);
    void setWidth(float width);
    void setHeight(float height);
    void setZoom(float zoom);

    void update(int width = 0, int height = 0, float mapScale = 1.0f);
    /** 
        This is just in the chance that the user wants to detach from the entity
        and attach to another, maybe for a dialog or something idk yet
    **/
    void attachEntity(Entity *entity);
    void detachEntity();
};


#endif // !CAMERA_H
