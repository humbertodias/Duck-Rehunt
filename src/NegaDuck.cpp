#include "NegaDuck.h"
#include "TextureManager.h"
#include "GameConfig.h" // For asset paths
#include <iostream>

// Static member initialization
std::vector<std::string> NegaDuck::s_textureIDs_Nega_Flying;
std::string NegaDuck::s_textureID_Nega_Falling;
std::string NegaDuck::s_textureID_Standard_Shot; // NegaDuck uses standard shot image
bool NegaDuck::s_assetsLoaded_Nega = false;

// Dimensions from inspecting DarkwingFlap.gif, DarkwingSp.gif, and the standard DuckShot.gif
const int NEGA_DUCK_FLY_W = 48; // Approx from DarkwingFlap.gif
const int NEGA_DUCK_FLY_H = 46;
const int NEGA_DUCK_FALL_W = 48; // Approx from DarkwingSp.gif (used for falling)
const int NEGA_DUCK_FALL_H = 48;
// Using standard shot dimensions from NormalDuck or define here if different context
// const int STANDARD_DUCK_SHOT_W = 48; (defined in NormalDuck.cpp, reuse or ensure consistency)
// const int STANDARD_DUCK_SHOT_H = 42;


NegaDuck::NegaDuck(int x, int y)
    : Duck("nega_duck_fly1", x, y, NEGA_DUCK_FLY_W, NEGA_DUCK_FLY_H, 2, 15, true) { // true for isEnemy
    if (!s_assetsLoaded_Nega) {
        std::cerr << "NegaDuck assets not loaded! Call NegaDuck::loadSharedAssets() first." << std::endl;
    }
    m_flyingAnimTextureIDs = s_textureIDs_Nega_Flying;
    m_shotTextureID_Nega = s_textureID_Standard_Shot; // Uses standard shot before unique fall
    m_fallingTextureID_Nega = s_textureID_Nega_Falling;

    setAnimation(DuckState::FLYING);
}

void NegaDuck::loadSharedAssets(SDL_Renderer* pRenderer) {
    if (s_assetsLoaded_Nega) return;

    Uint8 r = 0, g = 0, b = 255; // Blue color key for NegaDuck assets (Darkwing series)

    // Flying Animation Frames
    TextureManager::Instance()->load(NEGA_DUCK_FLAP1_IMG, "nega_duck_fly1", pRenderer, r, g, b);
    TextureManager::Instance()->load(NEGA_DUCK_FLAP2_IMG, "nega_duck_fly2", pRenderer, r, g, b);
    s_textureIDs_Nega_Flying = {"nega_duck_fly1", "nega_duck_fly2"};

    // Falling Frame (DarkwingSp.gif)
    // Pygame NegaDuck anidead(): self.anim = negaDuckImg[0] (which is DarkwingSp.gif)
    // But for the initial shot, it uses duckImg[2][0] (standard shot)
    TextureManager::Instance()->load(NEGA_DUCK_SP_IMG, "nega_duck_falling_sp", pRenderer, r, g, b);
    s_textureID_Nega_Falling = "nega_duck_falling_sp";

    // NegaDuck uses the standard "shot" image initially. Ensure it's loaded (e.g. by NormalDuck::loadAssets or load it here if not guaranteed)
    // For simplicity, we assume DUCK_SHOT_IMG is loaded as "normal_duck_shot" by NormalDuck::loadAssets()
    // If NormalDuck assets are not loaded first, this might be an issue.
    // A more robust system would have a global asset loader or ensure dependencies.
    // For now, let's assume "normal_duck_shot" is available.
    s_textureID_Standard_Shot = "normal_duck_shot"; // This ID must match what NormalDuck loads for its shot
    // If it's not loaded, we might need to load DUCK_SHOT_IMG here with its own ID for NegaDuck's use.
    // Let's add a check or ensure it's loaded if not already by TextureManager.
    if (!TextureManager::Instance()->getTexture(s_textureID_Standard_Shot)) {
         std::cout << "Standard shot texture not found by NegaDuck, attempting to load..." << std::endl;
         // Green color key for normal duck shot image: 136,216,0
         TextureManager::Instance()->load(DUCK_SHOT_IMG, "normal_duck_shot", pRenderer, 136, 216, 0);
         // This relies on NormalDuck also using "normal_duck_shot" as its ID.
    }


    s_assetsLoaded_Nega = true;
    std::cout << "NegaDuck assets loaded." << std::endl;
}


void NegaDuck::setAnimation(DuckState state) {
    m_state = state;
    m_animFrame = 0;

    switch (state) {
        case DuckState::FLYING:
            m_textureID = m_flyingAnimTextureIDs[0];
            m_width = NEGA_DUCK_FLY_W;
            m_height = NEGA_DUCK_FLY_H;
            m_frameCount = static_cast<int>(m_flyingAnimTextureIDs.size());
            break;
        case DuckState::SHOT:
            // NegaDuck briefly shows the standard shot sprite
            m_textureID = m_shotTextureID_Nega;
            m_width = NORMAL_DUCK_SHOT_W; // Using dimensions from NormalDuck's shot sprite
            m_height = NORMAL_DUCK_SHOT_H;
            m_frameCount = 1;
            break;
        case DuckState::FALLING:
            // After the brief "shot" display, NegaDuck falls with its special sprite
            m_textureID = m_fallingTextureID_Nega;
            m_width = NEGA_DUCK_FALL_W;
            m_height = NEGA_DUCK_FALL_H;
            m_frameCount = 1; // DarkwingSp.gif is a single frame
            break;
    }
    m_collider.w = m_width;
    m_collider.h = m_height;
}

void NegaDuck::update(float deltaTime) {
    Duck::update(deltaTime); // Base class handles movement, state transitions (SHOT -> FALLING), boundaries

    if (m_animTimer >= m_animDelayTicks) {
        m_animTimer = 0;
        m_animFrame++;

        if (m_state == DuckState::FLYING) {
            if (static_cast<size_t>(m_animFrame) >= m_flyingAnimTextureIDs.size()) {
                m_animFrame = 0;
            }
            m_textureID = m_flyingAnimTextureIDs[m_animFrame];
        }
        // SHOT state is a single frame, handled by base transition to FALLING
        // FALLING state for NegaDuck is a single frame (DarkwingSp.gif), so no animation frame change needed here.
        // If DarkwingSp was an animation, that logic would go here.
    }

    // Set flip based on horizontal velocity for flying state
    if (m_state == DuckState::FLYING) {
        if (m_velocity.x < 0) {
            m_flip = SDL_FLIP_HORIZONTAL;
        } else {
            m_flip = SDL_FLIP_NONE;
        }
    } else {
         m_flip = SDL_FLIP_NONE; // No flipping for shot/fall NegaDuck sprite
    }
}
