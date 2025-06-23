#include "Crosshair.h"
#include "Game.h" // Will need Game instance for mouse position, or pass mouse pos

Crosshair::Crosshair(const std::string& textureId, int x, int y, int w, int h)
    : Sprite(textureId, x, y, w, h, false, 1, 0) {
    m_type = GameObjectType::CROSSHAIR;
    // Texture loading will be handled by Game class and TextureManager
}

void Crosshair::update(float deltaTime) {
    // The actual positioning based on mouse coordinates will be handled
    // in Game::update() and then setPosition will be called on the crosshair instance.
    // So, this update might be minimal unless the crosshair has its own animation.
    // For now, let Sprite::update handle basic animation if any.
    Sprite::update(deltaTime);
}
