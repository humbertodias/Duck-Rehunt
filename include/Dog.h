#ifndef DOG_H
#define DOG_H

#include "Sprite.h"
#include <string>

enum class DogState {
    SPAWNING, // Moving up from bottom
    RELEASING, // At the top, about to release duck(s)
    RETREATING // Moving down
};

class Dog : public Sprite {
public:
    Dog(int initialX, int initialY);
    ~Dog() override = default;

    void update(float deltaTime) override;
    void reset(); // Reset dog to initial position and state

    static void loadSharedAssets(SDL_Renderer* pRenderer);

private:
    DogState m_state;
    float m_verticalSpeed; // Current speed, can change based on score
    int m_targetYSpawning; // Y position to reach before releasing
    int m_targetYRetreating; // Y position to reach before resetting and spawning again

    int m_releaseDelay; // Counter for delay before releasing duck
    int m_releaseTimer;

    int m_negaDuckCounterThreshold; // From Pygame: if len(dSprites.sprites()) > 6 and self.negaCounter >= 6
    int m_negaDuckSpawnCounter;     // Counts releases to determine NegaDuck spawn

    void setState(DogState newState);

    static std::string s_textureIdle;    // DogIdle.gif
    static std::string s_textureWithDuck; // DogDuck.gif (used for spawning/retreating)
    static bool s_assetsLoaded;
};

#endif // DOG_H
