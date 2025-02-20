#pragma once

#ifndef CAMERA_H
#define CAMERA_H

/** 

    Camera is attached to a "Player or Person" Object

    This is just in case we ever wanna inspect a villager or zombie and we need to set the camera
    to that object.

**/

// Make Sure to include player in implimentation file cuz it'll be a circular import
class Player;


class Camera
{
private:
    Player* player;
public:
    Camera(Player *player);
};


#endif // !CAMERA_H
