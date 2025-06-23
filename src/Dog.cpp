#include "Dog.h"
#include "TextureManager.h"
#include "GameConfig.h"
#include "Game.h" // For spawning ducks and getting score/duck count
#include "NormalDuck.h"
#include "NegaDuck.h"
#include <iostream>

// Static member initialization
std::string Dog::s_textureIdle;
std::string Dog::s_textureWithDuck;
bool Dog::s_assetsLoaded = false;

// Dimensions from DogIdle.gif and DogDuck.gif
const int DOG_IDLE_W = 70; // Approx
const int DOG_IDLE_H = 60;
const int DOG_DUCK_W = 70; // Approx
const int DOG_DUCK_H = 100;


Dog::Dog(int initialX, int initialY)
    : Sprite(s_textureWithDuck, initialX, initialY, DOG_DUCK_W, DOG_DUCK_H, false, 1, 0),
      m_verticalSpeed(2.0f), // Initial speed from Pygame
      m_targetYSpawning(DOG_RELEASE_Y_TARGET),
      m_targetYRetreating(DOG_RETREAT_Y_END),
      m_releaseDelay(60), // Pygame didn't have an explicit delay like this, release was instant.
                          // This could be a short pause at the top. Let's make it small for now.
      m_releaseTimer(0),
      m_negaDuckCounterThreshold(6), // Pygame: self.negaCounter >= 6 (also checked duck count)
      m_negaDuckSpawnCounter(0) {

    if (!s_assetsLoaded) {
        std::cerr << "Dog assets not loaded! Call Dog::loadSharedAssets() first." << std::endl;
    }
    m_type = GameObjectType::DOG;
    setState(DogState::SPAWNING); // Start by moving up
}

void Dog::loadSharedAssets(SDL_Renderer* pRenderer) {
    if (s_assetsLoaded) return;
    Uint8 r = 0, g = 0, b = 255; // Blue color key

    TextureManager::Instance()->load(DOG_IDLE_IMG, "dog_idle", pRenderer, r, g, b);
    s_textureIdle = "dog_idle";
    TextureManager::Instance()->load(DOG_DUCK_IMG, "dog_with_duck", pRenderer, r, g, b);
    s_textureWithDuck = "dog_with_duck";

    s_assetsLoaded = true;
    std::cout << "Dog assets loaded." << std::endl;
}

void Dog::setState(DogState newState) {
    m_state = newState;
    m_releaseTimer = 0; // Reset release timer when state changes

    switch (m_state) {
        case DogState::SPAWNING:
            m_textureID = s_textureWithDuck;
            m_width = DOG_DUCK_W;
            m_height = DOG_DUCK_H;
            m_velocity.y = -m_verticalSpeed; // Move up
            break;
        case DogState::RELEASING:
            m_textureID = s_textureIdle;
            m_width = DOG_IDLE_W;
            m_height = DOG_IDLE_H;
            m_velocity.y = 0; // Stop vertical movement
            break;
        case DogState::RETREATING:
            m_textureID = s_textureWithDuck; // Pygame uses DogDuck.gif for retreat too (dogImg[1])
            m_width = DOG_DUCK_W;
            m_height = DOG_DUCK_H;
            m_velocity.y = m_verticalSpeed; // Move down
            break;
    }
    m_collider.w = m_width;
    m_collider.h = m_height;
}


void Dog::update(float deltaTime) {
    // Update speed based on score (from Pygame logic)
    int currentScore = Game::Instance()->getScore();
    if (currentScore >= 300) {
        m_verticalSpeed = 5.0f;
    } else if (currentScore >= 100) {
        m_verticalSpeed = 4.0f;
    } else {
        m_verticalSpeed = 2.0f;
    }
    // Update velocity if state is moving
    if (m_state == DogState::SPAWNING) m_velocity.y = -m_verticalSpeed;
    else if (m_state == DogState::RETREATING) m_velocity.y = m_verticalSpeed;


    m_position += m_velocity; // Pygame movement was pixels per frame

    switch (m_state) {
        case DogState::SPAWNING:
            if (m_position.y <= m_targetYSpawning) {
                m_position.y = static_cast<float>(m_targetYSpawning); // Snap to position
                setState(DogState::RELEASING);
            }
            break;
        case DogState::RELEASING:
            m_releaseTimer++;
            // Pygame released duck immediately after reaching top and changing sprite
            // For now, let's make it instant release after 1 frame in this state.
            if (m_releaseTimer >= 1) {
                // Spawn duck logic
                // Pygame: if len(dSprites.sprites()) > 6 and self.negaCounter >= 6: NegaDuck
                // For now, let's simplify: use negaDuckSpawnCounter
                // We need access to the number of active ducks from Game instance.
                // This part of logic might be better handled in Game itself, triggered by Dog.
                // Or Dog queries Game. For now, Dog makes the decision.

                m_negaDuckSpawnCounter++;
                int currentDuckCount = Game::Instance()->getDuckCount();

                // Match Pygame's spawn: rect.centerx + 60, rect.center (for y)
                // Dog's current m_position is top-left.
                float dogCenterX = m_position.x + static_cast<float>(m_width) / 2.0f;
                float dogCenterY = m_position.y + static_cast<float>(m_height) / 2.0f; // Pygame used dog's center, not top for Y.
                                                                      // However, duck was positioned at dogHand.rect.center,
                                                                      // then its rect.centerx += 60.
                                                                      // Duck __init__ took dogHand, then self.rect.center = dogHand.rect.center; self.rect.centerx +=60
                                                                      // This means the duck's center is placed at dog's center, then shifted.
                                                                      // So, spawn the duck such that its CENTER will be at (dogCenterX + 60, dogCenterY).

                int spawnDuckAtX = static_cast<int>(dogCenterX + 60);
                int spawnDuckAtY = static_cast<int>(dogCenterY);
                // The Duck constructor takes top-left. We need to adjust spawnDuckAtX/Y
                // so that the duck's center ends up at these coords.
                // This will be handled by the Duck constructor if it centers itself, or we pass top-left.
                // Our Duck base constructor takes top-left x,y.
                // Let's assume the duck width/height for a typical flying duck (e.g., NORMAL_DUCK_FLAT_W/H)
                // This is getting complex. Pygame sets duck.rect.center.
                // Simpler: spawn relative to dog's current position.
                // Pygame: self.rect.center = (dogHand.rect.center) -> self.rect.centerx += 60
                // So, duck's center is (dog_center_x + 60, dog_center_y)
                // If duck is W, H, its top-left to achieve this is (dog_center_x + 60 - W/2, dog_center_y - H/2)

                // For now, let's use a simpler consistent offset from dog's visual release point.
                // The DogDuck.gif shows duck appearing from right side of dog.
                int spawnX = static_cast<int>(m_position.x + m_width - 20); // Near dog's right side
                int spawnY = static_cast<int>(m_position.y + 20);          // A bit down from dog's top

                if (currentDuckCount > 6 && m_negaDuckSpawnCounter >= m_negaDuckCounterThreshold) {
                     Game::Instance()->spawnDuck(std::make_unique<NegaDuck>(spawnX, spawnY));
                     m_negaDuckSpawnCounter = 0; // Reset counter
                } else {
                     Game::Instance()->spawnDuck(std::make_unique<NormalDuck>(spawnX, spawnY));
                }

                // Play quack sound
                Game::Instance()->playSound("quack");

                setState(DogState::RETREATING);
            }
            break;
        case DogState::RETREATING:
            if (m_position.y > m_targetYRetreating) { // Dog's top edge goes below target
                reset(); // Reset for next spawn cycle
            }
            break;
    }
    Sprite::update(deltaTime); // Update collider
}

void Dog::reset() {
    m_position.x = static_cast<float>(Game::Instance()->getRandomRange(45, SCREEN_WIDTH - m_width - 45));
    m_position.y = static_cast<float>(DOG_SPAWN_Y_START);
    setState(DogState::SPAWNING);
    // m_negaDuckSpawnCounter is not reset here, it's a persistent counter for NegaDuck spawn.
}
