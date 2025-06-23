#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory> // For smart pointers
#include <random> // For std::mt19937
#include "Sprite.h" // Forward declare or include fully if member types are Sprite
#include "GameConfig.h"


class Game {
public:
    static Game* Instance();

    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void loadAssets();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void clean();

    bool isRunning() const { return m_bRunning; }
    void quitGame() { m_bRunning = false; }

    SDL_Renderer* getRenderer() const { return m_pRenderer; }
    int getScore() const { return m_score; }

    // Random number generator helper
    int getRandomRange(int min, int max);


private:
    Game();
    ~Game();
    Game(const Game&);
    Game& operator=(const Game&);

    bool m_bRunning;
    SDL_Window* m_pWindow;
    SDL_Renderer* m_pRenderer;

    static Game* s_pInstance;

    // Game Objects
    std::unique_ptr<Sprite> m_background;
    std::unique_ptr<Sprite> m_foreground;
    // std::vector<std::unique_ptr<Duck>> m_ducks; // Example for later
    // std::unique_ptr<Dog> m_dog;
    // std::unique_ptr<Crosshair> m_crosshair;
    // std::vector<std::unique_ptr<GunFlash>> m_flashes;

    // Score
    int m_score;
    TTF_Font* m_pScoreFont;
    SDL_Color m_scoreColor;
    SDL_Texture* m_pScoreTexture;
    SDL_Rect m_scoreRect;
    void updateScoreDisplay();

    // Sounds
    Mix_Music* m_pMusic;
    Mix_Chunk* m_pGunshotSound;
    Mix_Chunk* m_pQuackSound;

    // Mouse state
    Vector2D m_mousePosition;

    // Random number generation
    std::mt19937 m_rng;
};

#endif // GAME_H
