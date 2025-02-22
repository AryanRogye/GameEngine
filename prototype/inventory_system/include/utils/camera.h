#pragma once

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
    Player* player;

    float x;
    float y;
    float width;
    float height;
    float zoom;

public:
    Camera(Player *player);

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
};


#endif // !CAMERA_H
