#include "Game.h"
#include "GameConfig.h" // For SCREEN_WIDTH, SCREEN_HEIGHT, FPS
#include <iostream>

int main(int argc, char* argv[]) {
    Uint32 frameStart;
    int frameTime;

    if (Game::Instance()->init("Duck Rehunt SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, false)) {
        std::cout << "Game initialized successfully!" << std::endl;

        while (Game::Instance()->isRunning()) {
            frameStart = SDL_GetTicks();

            Game::Instance()->handleEvents();
            // Pass deltaTime to update if needed for frame-independent movement
            // float deltaTime = frameTime / 1000.0f; // Convert ms to seconds
            // For now, using fixed tick rate logic from Pygame (updates tied to FPS)
            Game::Instance()->update(FRAME_DELAY / 1000.0f); // deltaTime can be fixed if game logic assumes fixed ticks
            Game::Instance()->render();

            frameTime = SDL_GetTicks() - frameStart;

            if (FRAME_DELAY > frameTime) {
                SDL_Delay(FRAME_DELAY - frameTime);
            }
        }
    } else {
        std::cerr << "Game failed to initialize." << std::endl;
        // Game::Instance()->clean() will be called by Game's destructor if init fails partially
        // but if init fails very early, direct cleanup might be needed or rely on OS.
        // The current Game::init handles cleanup on failure.
        return -1;
    }

    std::cout << "Exiting game normally." << std::endl;
    Game::Instance()->clean();
    // delete Game::Instance(); // If s_pInstance was new'd directly without unique_ptr management for singleton

    return 0;
}
