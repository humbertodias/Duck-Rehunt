#ifndef SPRITE_H
#define SPRITE_H

#include "TextureManager.h"
#include "Vector2D.h"
#include "GameConfig.h" // For SCREEN_WIDTH etc. if needed for bounds
#include <SDL2/SDL.h>
#include <string>
#include <vector> // For animation frames

enum class GameObjectType {
    NONE,
    PLAYER, // Not used in this game
    ENEMY_DUCK,
    NORMAL_DUCK,
    DOG,
    CROSSHAIR,
    GUNFLASH,
    SCENERY
};


class Sprite {
public:
    Sprite(const std::string& textureId, int x, int y, int w, int h, bool useSheet = false, int frameCount = 1, int animSpeed = 0)
        : m_position(static_cast<float>(x), static_cast<float>(y)), // Matches declaration order
          m_velocity(0,0), m_acceleration(0,0), // Initialize these too
          m_width(w), m_height(h),
          m_textureID(textureId),
          m_flip(SDL_FLIP_NONE),
          // m_collider is initialized below
          m_bUseSpriteSheet(useSheet),
          m_currentFrame(0),
          m_currentRow(0),
          m_frameCount(frameCount),
          m_animationSpeed(animSpeed),
          m_type(GameObjectType::NONE),
          m_isDead(false),
          m_isActive(true) {
        m_collider.x = static_cast<int>(m_position.x);
        m_collider.y = static_cast<int>(m_position.y);
        m_collider.w = m_width;
        m_collider.h = m_height;
    }

    virtual ~Sprite() {}

    virtual void loadAssets(SDL_Renderer* pRenderer) {
        // Base class might not load directly, or could load a default/placeholder
        // For this game, specific sprites will handle their asset loading.
    }

    virtual void update(float deltaTime) {
        if (m_bUseSpriteSheet && m_frameCount > 1 && m_animationSpeed > 0) {
            m_currentFrame = int(((SDL_GetTicks() / m_animationSpeed) % m_frameCount));
        }
        // Update collider position
        m_collider.x = static_cast<int>(m_position.x);
        m_collider.y = static_cast<int>(m_position.y);
    }

    virtual void draw(SDL_Renderer* pRenderer) {
        if (!m_isActive) return;

        if (m_bUseSpriteSheet) {
            TextureManager::Instance()->drawFrame(m_textureID,
                                                 static_cast<int>(m_position.x),
                                                 static_cast<int>(m_position.y),
                                                 m_width, m_height,
                                                 m_currentRow, m_currentFrame,
                                                 pRenderer, 0.0, m_flip);
        } else {
            TextureManager::Instance()->draw(m_textureID,
                                             static_cast<int>(m_position.x),
                                             static_cast<int>(m_position.y),
                                             m_width, m_height,
                                             pRenderer, m_flip);
        }
    }

    virtual void onCollision(/* Sprite* other */) {
        // To be implemented by derived classes
    }

    const SDL_Rect& getCollider() const { return m_collider; }
    const Vector2D& getPosition() const { return m_position; }
    void setPosition(float x, float y) {
        m_position.x = x;
        m_position.y = y;
        m_collider.x = static_cast<int>(x);
        m_collider.y = static_cast<int>(y);
    }
    void setPosition(const Vector2D& pos) {
        m_position = pos;
        m_collider.x = static_cast<int>(pos.x);
        m_collider.y = static_cast<int>(pos.y);
    }


    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    GameObjectType getType() const { return m_type; }

    bool isDead() const { return m_isDead; }
    void setDead(bool dead) { m_isDead = dead; }
    bool isActive() const { return m_isActive; }
    void setActive(bool active) { m_isActive = active; }

protected:
    Vector2D m_position;
    Vector2D m_velocity;
    Vector2D m_acceleration; // Not used yet, but good for physics

    int m_width;
    int m_height;

    std::string m_textureID;
    SDL_RendererFlip m_flip;
    SDL_Rect m_collider; // For collision detection

    // Animation
    bool m_bUseSpriteSheet;
    int m_currentFrame;
    int m_currentRow; // If sprite sheet has multiple rows of animations
    int m_frameCount;
    int m_animationSpeed; // Milliseconds per frame

    GameObjectType m_type;
    bool m_isDead;   // General flag, e.g., for ducks being shot
    bool m_isActive; // General flag for whether the object should be updated/drawn
};

#endif // SPRITE_H
