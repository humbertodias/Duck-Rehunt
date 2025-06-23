#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "Sprite.h"
#include "GameConfig.h" // For asset path, though Game will handle loading

class Crosshair : public Sprite {
public:
    Crosshair(const std::string& textureId, int x, int y, int w, int h);

    void update(float deltaTime) override; // To follow the mouse
    // Draw is inherited from Sprite
};

#endif // CROSSHAIR_H
