#include "GunFlash.h"

GunFlash::GunFlash(const std::string& textureId, int x, int y, int w, int h, int lifeTimeFrames)
    : Sprite(textureId, x, y, w, h, false, 1, 0),
      m_lifeTimeFrames(lifeTimeFrames), m_framesLived(0) {
    m_type = GameObjectType::GUNFLASH;
    // Center the flash on the given x,y position
    m_position.x -= static_cast<float>(w) / 2.0f;
    m_position.y -= static_cast<float>(h) / 2.0f;
    // Update collider after adjusting position
    m_collider.x = static_cast<int>(m_position.x);
    m_collider.y = static_cast<int>(m_position.y);
}

void GunFlash::update(float deltaTime) {
    Sprite::update(deltaTime); // Handle any base sprite update logic (like animation if it were a spritesheet)
    m_framesLived++;
}

bool GunFlash::isExpired() const {
    return m_framesLived >= m_lifeTimeFrames;
}
