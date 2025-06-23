#ifndef DUCK_H
#define DUCK_H

#include "Sprite.h"
#include <vector>
#include <string>

enum class DuckState {
    FLYING,
    SHOT, // Initial state after being shot, before falling
    FALLING
};

class Duck : public Sprite {
public:
    Duck(const std::string& baseTextureId, int x, int y, int w, int h,
         int frameCount, int animSpeed, bool isEnemy);
    virtual ~Duck() {}

    virtual void update(float deltaTime) override;

    virtual void shoot(); // Call when duck is shot

    bool isEnemy() const { return m_isEnemy; }
    bool isShot() const { return m_state == DuckState::SHOT; }
    bool isFalling() const { return m_state == DuckState::FALLING; }
    bool isTrulyDead() const { return m_isTrulyDead; } // For removal after falling off screen

    // To be overridden by derived classes for specific animations
    virtual void setAnimation(DuckState state) = 0;

protected:
    Vector2D m_velocity; // dx, dy from Pygame
    DuckState m_state;
    bool m_isEnemy;
    bool m_isTrulyDead; // Flag for when it's completely off-screen and dead

    // Animation related - might be specialized in derived classes or use a common system
    int m_animFrame;
    int m_animTimer; // General timer for animation frame progression
    int m_animDelayTicks; // Delay between animation frames (in game ticks or ms)
                          // Pygame used 'pause' which incremented each frame, and 'animDelay' as threshold.
                          // So, if FPS is 60, animDelay = 15 means 15/60 = 0.25 seconds per anim frame.

    int m_shotDisplayTime; // How long to show the "shot" sprite before falling
    int m_shotTimer;

    // Boundary checks and direction changes
    void checkBounds();
    void changeDirectionRandomly();
    void loadInitialDirection();

    // Specific animation textures to be managed by derived classes
    // Example:
    // std::vector<std::string> m_flyingAnimFrames_textureIDs;
    // std::string m_shotTextureID;
    // std::vector<std::string> m_fallingAnimFrames_textureIDs;
};

#endif // DUCK_H
