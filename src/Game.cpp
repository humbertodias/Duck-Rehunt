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

    // Load Crosshair texture - Pygame crosshair is 30x30
    // The Crosshair.gif from Pygame has a blue background (0,0,255) which needs to be color-keyed.
    if (TextureManager::Instance()->load(CROSSHAIR_IMG, "crosshair", m_pRenderer, 0, 0, 255)) {
        m_crosshair = std::make_unique<Crosshair>("crosshair", 0, 0, 30, 30);
    } else {
        std::cerr << "Failed to load crosshair texture or create crosshair object." << std::endl;
    }

    // Load GunFlash texture - Pygame Gunshot.gif is 100x100 with black background (0,0,0)
    if (!TextureManager::Instance()->load(GUNSHOT_IMG, "gunflash", m_pRenderer, 0, 0, 0)) {
        std::cerr << "Failed to load gunflash texture." << std::endl;
    }

    // Load Duck Assets
    NormalDuck::loadSharedAssets(m_pRenderer);
    NegaDuck::loadSharedAssets(m_pRenderer);

    // Load Dog Assets
    Dog::loadSharedAssets(m_pRenderer);

    // Create Dog instance
    // Initial position from Pygame: random.randrange(45,595), 440
    int initialDogX = getRandomRange(45, SCREEN_WIDTH - 45 - DOG_DEFAULT_WIDTH);
    m_dog = std::make_unique<Dog>(initialDogX, DOG_SPAWN_Y_START);


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
                    playSound("gunshot");

                    // Create gunshot flash
                    // Pygame flash lasts 5 frames. FPS is 60.
                    // Pygame Gunshot.gif is 100x100
                    m_gunFlashes.push_back(std::make_unique<GunFlash>("gunflash",
                                                                     static_cast<int>(m_mousePosition.x),
                                                                     static_cast<int>(m_mousePosition.y),
                                                                     100, 100, 5));

                    // Handle shooting logic (collision detection with ducks)
                    SDL_Point mouseP = {static_cast<int>(m_mousePosition.x), static_cast<int>(m_mousePosition.y)};
                    int scoreFromThisShot = 0;
                    // Iterate in reverse to handle potential removals or multiple overlapping sprites correctly,
                    // though for now, we don't remove immediately on shot.
                    for (auto it = m_ducks.rbegin(); it != m_ducks.rend(); ++it) {
                        Duck* duck = it->get(); // Get raw pointer for interaction
                        if (duck && !duck->isDead() && duck->isActive()) { // Check if duck is active and not already shot
                            if (SDL_PointInRect(&mouseP, &duck->getCollider())) {
                                duck->shoot(); // Duck handles its state change to SHOT/FALLING
                                if (duck->isEnemy()) {
                                    scoreFromThisShot -= 50; // NegaDuck
                                } else {
                                    scoreFromThisShot += 5;  // NormalDuck
                                }
                                // Pygame had a multiplier for multiple ducks hit at once.
                                // For now, we process one hit duck per click event iteration.
                                // If multiple ducks overlap, this loop will find the "topmost" one first if rendered last.
                                // To hit multiple, we'd need to not break and sum up scores.
                                // For simplicity, let's assume one primary target per click for now.
                                // If we want to allow multiple hits, remove the 'break'.
                                // However, Pygame's pointCollide would get all, then multiply.
                                // Let's stick to one effective hit per click for now, which is simpler.
                                break;
                            }
                        }
                    }
                    if (scoreFromThisShot != 0) {
                        m_score += scoreFromThisShot;
                        updateScoreDisplay();
                    }
                }
                break;
            default:
                break;
        }
    }
}

void Game::update(float deltaTime) {
    // Update crosshair position
    if (m_crosshair) {
        // Center the crosshair on the mouse cursor
        float crosshairWidth = static_cast<float>(m_crosshair->getWidth());
        float crosshairHeight = static_cast<float>(m_crosshair->getHeight());
        m_crosshair->setPosition(m_mousePosition.x - crosshairWidth / 2.0f,
                                 m_mousePosition.y - crosshairHeight / 2.0f);
        m_crosshair->update(deltaTime);
    }

    // Update GunFlashes and remove expired ones
    for (auto it = m_gunFlashes.begin(); it != m_gunFlashes.end(); /* increment in loop */) {
        (*it)->update(deltaTime);
        if ((*it)->isExpired()) {
            it = m_gunFlashes.erase(it); // Erase and get next valid iterator
        } else {
            ++it;
        }
    }

    // Update Ducks and remove dead/off-screen ones
    for (auto it = m_ducks.begin(); it != m_ducks.end(); /* increment in loop */) {
        (*it)->update(deltaTime);
        if ((*it)->isTrulyDead()) { // isTrulyDead when fallen off screen
            it = m_ducks.erase(it);
        } else {
            ++it;
        }
    }

    // Update Dog
    if (m_dog) {
        m_dog->update(deltaTime);
    }

    // Check for collisions (shooting) - will be in handleEvents or a dedicated collision step
    // Garbage collect dead ducks/flashes
}

void Game::render() {
    SDL_RenderClear(m_pRenderer);

    // Draw background
    if (m_background) m_background->draw(m_pRenderer);

    // Draw Ducks
    for (const auto& duck : m_ducks) {
        duck->draw(m_pRenderer);
    }
    if (m_dog) {
        m_dog->draw(m_pRenderer);
    }

    // Draw foreground
    if (m_foreground) m_foreground->draw(m_pRenderer);

    // Draw score
    if (m_pScoreTexture) {
        SDL_RenderCopy(m_pRenderer, m_pScoreTexture, nullptr, &m_scoreRect);
    }

    // Draw gunshot flashes
    for (const auto& flash : m_gunFlashes) {
        flash->draw(m_pRenderer);
    }

    // Draw crosshair (should be on top of most things)
    if (m_crosshair) {
        m_crosshair->draw(m_pRenderer);
    }

    SDL_RenderPresent(m_pRenderer);
}

void Game::clean() {
    std::cout << "Cleaning game..." << std::endl;

    // Clear game objects
    m_background.reset();
    m_foreground.reset();
    m_crosshair.reset();
    m_gunFlashes.clear();
    m_ducks.clear();
    m_dog.reset();


    // Clean up Textures from TextureManager
    TextureManager::Instance()->clearTexture("background");
    TextureManager::Instance()->clearTexture("foreground");
    TextureManager::Instance()->clearTexture("crosshair");
    TextureManager::Instance()->clearTexture("gunflash"); // Corrected from gunshot_flash to gunflash
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

void Game::spawnDuck(std::unique_ptr<Duck> duck) {
    if (duck) {
        m_ducks.push_back(std::move(duck));
        // Play quack sound - moved to Dog class as per Pygame logic (quack on release)
        // if(m_pQuackSound) Mix_PlayChannel(-1, m_pQuackSound, 0);
    }
}

void Game::playSound(const std::string& soundID) {
    if (soundID == "quack" && m_pQuackSound) {
        Mix_PlayChannel(-1, m_pQuackSound, 0);
    } else if (soundID == "gunshot" && m_pGunshotSound) {
        Mix_PlayChannel(-1, m_pGunshotSound, 0);
    }
    // Add more sounds here if needed
}

int Game::getDuckCount() const {
    return m_ducks.size();
}

int Game::getRandomRange(int min, int max) {
    if (min > max) std::swap(min, max);
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(m_rng);
}
