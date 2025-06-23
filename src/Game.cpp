#include "Game.h"
#include "TextureManager.h"
// #include "src/GameConfig.h" // No longer needed here, Game.h includes GameConfig.h
#include <iostream>
#include <random> // For std::mt19937

Game* Game::s_pInstance = nullptr;

Game* Game::Instance() {
    if (s_pInstance == nullptr) {
        s_pInstance = new Game();
    }
    return s_pInstance;
}

Game::Game() : m_bRunning(false), m_pWindow(nullptr), m_pRenderer(nullptr), m_score(0),
               m_pScoreFont(nullptr), m_pScoreTexture(nullptr),
               m_pMusic(nullptr), m_pGunshotSound(nullptr), m_pQuackSound(nullptr) {
    // Initialize random number generator
    std::random_device rd;
    m_rng.seed(rd());
}

Game::~Game() {
    // clean() should be called before destruction
    // Ensure clean is called if not already
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_image
    // Removed IMG_INIT_GIF as it caused issues and might not be available/needed for these assets
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Create Window
    m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags | SDL_WINDOW_SHOWN);
    if (m_pWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Create Renderer
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(m_pWindow);
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255); // Default to black

    m_bRunning = true;
    SDL_ShowCursor(SDL_DISABLE); // Hide system cursor

    loadAssets();
    updateScoreDisplay(); // Initial score display

    return true;
}

void Game::loadAssets() {
    // Load Textures
    TextureManager::Instance()->load(BACKGROUND_IMG, "background", m_pRenderer);
    // The foreground in Pygame has a color key (0,0,255) blue
    TextureManager::Instance()->load(FOREGROUND_IMG, "foreground", m_pRenderer, 0, 0, 255);
    TextureManager::Instance()->load(CROSSHAIR_IMG, "crosshair", m_pRenderer); // Assuming crosshair has alpha or needs a key if not
    TextureManager::Instance()->load(GUNSHOT_IMG, "gunshot_flash", m_pRenderer); // Assuming specific color key if needed, or alpha

    // Create background and foreground sprites
    m_background = std::make_unique<Sprite>("background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    m_foreground = std::make_unique<Sprite>("foreground", 0, FOREGROUND_Y_POS, SCREEN_WIDTH, 179); //Approx height of foreground
    // TODO: Get actual height of foreground image if necessary, or ensure it's loaded properly.
    // For now, assuming the foreground image is SCREEN_WIDTH x (SCREEN_HEIGHT - FOREGROUND_Y_POS)
    // The original foreground is 640x179.

    // Load Sounds
    m_pMusic = Mix_LoadMUS(MUSIC_SND);
    if (!m_pMusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
    } else {
        Mix_PlayMusic(m_pMusic, -1); // Loop indefinitely
    }
    m_pGunshotSound = Mix_LoadWAV(GUNSHOT_SND);
    if (!m_pGunshotSound) std::cerr << "Failed to load gunshot sound: " << Mix_GetError() << std::endl;

    m_pQuackSound = Mix_LoadWAV(QUACK_SND);
    if (!m_pQuackSound) std::cerr << "Failed to load quack sound: " << Mix_GetError() << std::endl;


    // Load Font for Score
    m_pScoreFont = TTF_OpenFont(FONT_FILE, 20);
    if (!m_pScoreFont) {
        std::cerr << "Failed to load font: " << FONT_FILE << " - " << TTF_GetError() << std::endl;
    }
    m_scoreColor = {255, 255, 255, 255}; // White
}


void Game::updateScoreDisplay() {
    if (!m_pScoreFont) return;

    if (m_pScoreTexture) {
        SDL_DestroyTexture(m_pScoreTexture);
        m_pScoreTexture = nullptr;
    }

    std::string scoreText = std::to_string(m_score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_pScoreFont, scoreText.c_str(), m_scoreColor);
    if (textSurface) {
        m_pScoreTexture = SDL_CreateTextureFromSurface(m_pRenderer, textSurface);
        m_scoreRect.x = 540; // Position from Pygame
        m_scoreRect.y = 410; // Position from Pygame
        m_scoreRect.w = textSurface->w;
        m_scoreRect.h = textSurface->h;
        SDL_FreeSurface(textSurface);
    } else {
        std::cerr << "Failed to render score text: " << TTF_GetError() << std::endl;
    }
}


void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_bRunning = false;
                break;
            case SDL_MOUSEMOTION:
                m_mousePosition.x = static_cast<float>(event.motion.x);
                m_mousePosition.y = static_cast<float>(event.motion.y);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // Play gunshot sound
                    if(m_pGunshotSound) Mix_PlayChannel(-1, m_pGunshotSound, 0);
                    // Handle shooting logic (collision detection with ducks) - TODO
                    // Create gunshot flash - TODO
                }
                break;
            default:
                break;
        }
    }
}

void Game::update(float deltaTime) {
    // Update game objects - TODO
    // m_crosshair->setPosition(m_mousePosition.x - crosshair_width/2, m_mousePosition.y - crosshair_height/2);
    // Update ducks, dog, flashes
    // Check for collisions
    // Garbage collect dead ducks/flashes
}

void Game::render() {
    SDL_RenderClear(m_pRenderer);

    // Draw background
    if (m_background) m_background->draw(m_pRenderer);

    // Draw game objects (ducks, dog) - TODO

    // Draw foreground
    if (m_foreground) m_foreground->draw(m_pRenderer);

    // Draw score
    if (m_pScoreTexture) {
        SDL_RenderCopy(m_pRenderer, m_pScoreTexture, nullptr, &m_scoreRect);
    }

    // Draw gunshot flashes - TODO
    // Draw crosshair - TODO (at m_mousePosition)

    SDL_RenderPresent(m_pRenderer);
}

void Game::clean() {
    std::cout << "Cleaning game..." << std::endl;

    // Clear game objects
    m_background.reset();
    m_foreground.reset();
    // m_ducks.clear();
    // m_dog.reset();
    // m_crosshair.reset();
    // m_flashes.clear();


    // Clean up Textures from TextureManager
    TextureManager::Instance()->clearTexture("background");
    TextureManager::Instance()->clearTexture("foreground");
    TextureManager::Instance()->clearTexture("crosshair");
    TextureManager::Instance()->clearTexture("gunshot_flash");
    // Add other texture IDs used (ducks, dog etc.)
    TextureManager::Instance()->clean(); // Final cleanup of the manager itself if needed, or just clear map

    // Free sounds
    if (m_pMusic) Mix_FreeMusic(m_pMusic);
    if (m_pGunshotSound) Mix_FreeChunk(m_pGunshotSound);
    if (m_pQuackSound) Mix_FreeChunk(m_pQuackSound);
    m_pMusic = nullptr;
    m_pGunshotSound = nullptr;
    m_pQuackSound = nullptr;

    // Free font and score texture
    if (m_pScoreTexture) SDL_DestroyTexture(m_pScoreTexture);
    if (m_pScoreFont) TTF_CloseFont(m_pScoreFont);
    m_pScoreTexture = nullptr;
    m_pScoreFont = nullptr;

    // Destroy Renderer and Window
    if (m_pRenderer) SDL_DestroyRenderer(m_pRenderer);
    if (m_pWindow) SDL_DestroyWindow(m_pWindow);
    m_pRenderer = nullptr;
    m_pWindow = nullptr;

    // Shutdown SDL subsystems
    TTF_Quit();
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();

    m_bRunning = false;
}

int Game::getRandomRange(int min, int max) {
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(m_rng);
}
