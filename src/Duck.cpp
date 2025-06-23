#include "Duck.h"
#include "Game.h" // For getRandomRange and game boundaries
#include "GameConfig.h" // For screen boundaries
#include <iostream> // For debugging

Duck::Duck(const std::string& baseTextureId, int x, int y, int w, int h,
           int frameCount, int animSpeed, bool isEnemy)
    : Sprite(baseTextureId, x, y, w, h, true, frameCount, animSpeed), // Assuming base is a spritesheet
      m_state(DuckState::FLYING),
      m_isEnemy(isEnemy),
      m_isTrulyDead(false),
      m_animFrame(0),
      m_animTimer(0),
      m_animDelayTicks(animSpeed > 0 ? animSpeed : 15), // Pygame default was 15 frames pause
      m_shotDisplayTime(5), // Pygame showed shot image for 5 frames before falling
      m_shotTimer(0) {

    m_type = isEnemy ? GameObjectType::ENEMY_DUCK : GameObjectType::NORMAL_DUCK;
    loadInitialDirection();
    // Derived classes will call setAnimation(DuckState::FLYING)
}

void Duck::loadInitialDirection() {
    // Ensure dx and dy are not both zero
    do {
        m_velocity.x = static_cast<float>(Game::Instance()->getRandomRange(-4, 4));
        m_velocity.y = static_cast<float>(Game::Instance()->getRandomRange(-4, 4));
    } while (m_velocity.x == 0 && m_velocity.y == 0);
}

void Duck::update(float deltaTime) {
    // deltaTime is ~16ms for 60FPS.
    // m_animDelayTicks is in terms of game loop ticks (frames).
    // Pygame logic: self.pause +=1; if self.pause >= self.animDelay: self.frame +=1; self.pause = 0

    m_animTimer++; // Acts like Pygame's 'pause'

    switch (m_state) {
        case DuckState::FLYING:
            m_position += m_velocity; // deltaTime is already factored if velocity is units/sec
                                      // Pygame used pixels per frame, so direct add is fine.
            checkBounds();
            // Animation update handled by derived class via setAnimation or Sprite's own update if simple sheet
            break;
        case DuckState::SHOT:
            m_shotTimer++;
            if (m_shotTimer >= m_shotDisplayTime) {
                m_state = DuckState::FALLING;
                m_velocity.x = 0; // Stop horizontal movement
                m_velocity.y = 3; // Fall speed from Pygame (pixels per frame)
                setAnimation(DuckState::FALLING); // Switch to falling animation
                m_animTimer = 0; // Reset animation timer for falling anim
            }
            break;
        case DuckState::FALLING:
            m_position += m_velocity; // Fall down
            // Animation update handled by derived class or Sprite's own update
            if (m_position.y > SCREEN_HEIGHT) { // Fell off screen
                m_isTrulyDead = true;
                m_isActive = false; // Stop drawing and updating
            }
            break;
    }

    // Base sprite update for collider and potentially simple animation if m_bUseSpriteSheet is true
    // and m_animationSpeed is set in Sprite constructor (which it is by Duck constructor)
    // However, duck animations are more complex (multiple sheets/sequences).
    // We will handle specific frame updates in derived setAnimation methods or by directly setting m_currentFrame.
    // For now, let's rely on derived classes to manage m_currentFrame based on m_animTimer and m_animDelayTicks.
    Sprite::update(deltaTime); // Updates collider
}


void Duck::shoot() {
    if (m_state == DuckState::FLYING) {
        m_state = DuckState::SHOT;
        m_isDead = true; // From Sprite class, indicates it's been hit
        m_shotTimer = 0;
        m_animTimer = 0; // Reset animation timer for shot display
        setAnimation(DuckState::SHOT); // Switch to "shot" sprite
        // Sound is played by Game class
    }
}

void Duck::checkBounds() {
    bool directionChanged = false;
    // Screen bounds from GameConfig (0 to SCREEN_WIDTH, 0 to FOREGROUND_Y_POS - DUCK_FLY_AREA_BOTTOM)
    // Pygame bounds were 0 to 640, and 0 to 301 for ducks.
    const int DUCK_FLY_AREA_BOTTOM = FOREGROUND_Y_POS - m_height; // So they fly above foreground

    if (m_position.x < 0) {
        m_position.x = 0;
        m_velocity.x = static_cast<float>(Game::Instance()->getRandomRange(1, 4)); // Must move right
        m_velocity.y = static_cast<float>(Game::Instance()->getRandomRange(-4, 4));
        if(m_velocity.y == 0) m_velocity.y = Game::Instance()->getRandomRange(1,2) * (rand()%2 == 0 ? 1: -1); // ensure some y if needed
        directionChanged = true;
    } else if (m_position.x + m_width > SCREEN_WIDTH) {
        m_position.x = static_cast<float>(SCREEN_WIDTH - m_width);
        m_velocity.x = static_cast<float>(Game::Instance()->getRandomRange(-4, -1)); // Must move left
        m_velocity.y = static_cast<float>(Game::Instance()->getRandomRange(-4, 4));
         if(m_velocity.y == 0) m_velocity.y = Game::Instance()->getRandomRange(1,2) * (rand()%2 == 0 ? 1: -1);
        directionChanged = true;
    }

    if (m_position.y < 0) {
        m_position.y = 0;
        m_velocity.y = static_cast<float>(Game::Instance()->getRandomRange(1, 4)); // Must move down
        // Keep current dx or get a new one if stuck
        if (m_velocity.x == 0) m_velocity.x = static_cast<float>(Game::Instance()->getRandomRange(1,2) * (rand()%2 == 0 ? 1: -1));
        directionChanged = true;
    } else if (m_position.y > DUCK_FLY_AREA_BOTTOM) {
        m_position.y = static_cast<float>(DUCK_FLY_AREA_BOTTOM);
        m_velocity.y = static_cast<float>(Game::Instance()->getRandomRange(-4, -1)); // Must move up
        if (m_velocity.x == 0) m_velocity.x = static_cast<float>(Game::Instance()->getRandomRange(1,2) * (rand()%2 == 0 ? 1: -1));
        directionChanged = true;
    }

    // Ensure velocity is not zero after boundary correction (though above logic tries to ensure non-zero)
    if (m_velocity.x == 0 && m_velocity.y == 0) {
        loadInitialDirection(); // Get a completely new direction
        directionChanged = true;
    }


    if (directionChanged) {
        setAnimation(DuckState::FLYING); // Update animation based on new direction (e.g., flip)
    }
}

// This was in Pygame Duck, might not be needed if boundary hits always give new valid velocities
void Duck::changeDirectionRandomly() {
    // This was called in Pygame if changeDelay was met.
    // For now, relying on boundary hits to change direction.
    // Could add a timer to randomly change direction mid-flight if desired.
}
