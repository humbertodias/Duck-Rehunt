#include "NormalDuck.h"
#include "TextureManager.h"
#include "GameConfig.h" // For asset paths
#include <iostream>

// Initialize static members
std::vector<std::string> NormalDuck::s_textureIDs_UD;
std::vector<std::string> NormalDuck::s_textureIDs_Flat;
std::string NormalDuck::s_shotTextureID_Normal;
std::vector<std::string> NormalDuck::s_fallingTextureIDs_Normal;
bool NormalDuck::s_assetsLoaded = false;

// Dimensions from inspecting DuckUD1.gif, DuckF1.gif, DuckShot.gif, DuckFall.gif
const int NORMAL_DUCK_UD_W = 42; // Approx from DuckUD1.gif
const int NORMAL_DUCK_UD_H = 48;
const int NORMAL_DUCK_FLAT_W = 50; // Approx from DuckF1.gif
const int NORMAL_DUCK_FLAT_H = 40;
// NORMAL_DUCK_SHOT_W and H are now in GameConfig.h/cpp
const int NORMAL_DUCK_FALL_W = 32; // Approx from DuckFall.gif
const int NORMAL_DUCK_FALL_H = 48;


NormalDuck::NormalDuck(int x, int y)
    : Duck("normal_duck_ud1", x, y, NORMAL_DUCK_UD_W, NORMAL_DUCK_UD_H, 3, 15, false) { // Default to UD animation
    if (!s_assetsLoaded) {
        std::cerr << "NormalDuck assets not loaded! Call NormalDuck::loadSharedAssets() first." << std::endl;
        // Potentially throw an exception or handle error
    }
    // Copy static texture IDs to member variables for easier access (optional, could use static directly)
    m_animTextureIDs_UD = s_textureIDs_UD;
    m_animTextureIDs_Flat = s_textureIDs_Flat;
    m_shotTextureID = s_shotTextureID_Normal;
    m_fallingTextureIDs = s_fallingTextureIDs_Normal;

    setAnimation(DuckState::FLYING); // Set initial animation
}

void NormalDuck::loadSharedAssets(SDL_Renderer* pRenderer) {
    if (s_assetsLoaded) return;

    Uint8 r = 136, g = 216, b = 0; // Green color key for normal ducks

    // UD Animation Frames (Up/Down movement)
    TextureManager::Instance()->load(DUCK_UD1_IMG, "normal_duck_ud1", pRenderer, r, g, b);
    TextureManager::Instance()->load(DUCK_UD2_IMG, "normal_duck_ud2", pRenderer, r, g, b);
    TextureManager::Instance()->load(DUCK_UD3_IMG, "normal_duck_ud3", pRenderer, r, g, b);
    s_textureIDs_UD = {"normal_duck_ud1", "normal_duck_ud2", "normal_duck_ud3"};

    // Flat Animation Frames (Horizontal movement)
    TextureManager::Instance()->load(DUCK_F1_IMG, "normal_duck_f1", pRenderer, r, g, b);
    TextureManager::Instance()->load(DUCK_F2_IMG, "normal_duck_f2", pRenderer, r, g, b);
    TextureManager::Instance()->load(DUCK_F3_IMG, "normal_duck_f3", pRenderer, r, g, b);
    s_textureIDs_Flat = {"normal_duck_f1", "normal_duck_f2", "normal_duck_f3"};

    // Shot Frame
    TextureManager::Instance()->load(DUCK_SHOT_IMG, "normal_duck_shot", pRenderer, r, g, b);
    s_shotTextureID_Normal = "normal_duck_shot";

    // Falling Animation Frames
    TextureManager::Instance()->load(DUCK_FALL_IMG, "normal_duck_fall", pRenderer, r, g, b);
    // For flipped falling, we'll use SDL_FLIP_HORIZONTAL with the same texture.
    // Or load a pre-flipped one if preferred, but SDL_FLIP is easier.
    s_fallingTextureIDs_Normal = {"normal_duck_fall"}; // Only one frame in Pygame, then flipped

    s_assetsLoaded = true;
    std::cout << "NormalDuck assets loaded." << std::endl;
}

void NormalDuck::setAnimation(DuckState state) {
    m_state = state; // Ensure base class state is also set
    m_animFrame = 0; // Reset animation frame index
    // m_animTimer = 0; // Reset by Duck base class or here if needed for specific timing

    switch (state) {
        case DuckState::FLYING:
            // Pygame logic: if self.dy == 0: self.anim = duckImg[1] (Flat) else: self.anim = duckImg[0] (UD)
            if (std::abs(m_velocity.y) < 0.1f) { // Effectively dy == 0 (horizontal flight)
                m_textureID = m_animTextureIDs_Flat[0];
                m_width = NORMAL_DUCK_FLAT_W;
                m_height = NORMAL_DUCK_FLAT_H;
                m_frameCount = static_cast<int>(m_animTextureIDs_Flat.size());
            } else { // Moving up or down
                m_textureID = m_animTextureIDs_UD[0];
                m_width = NORMAL_DUCK_UD_W;
                m_height = NORMAL_DUCK_UD_H;
                m_frameCount = static_cast<int>(m_animTextureIDs_UD.size());
            }
            break;
        case DuckState::SHOT:
            m_textureID = m_shotTextureID;
            m_width = ::NORMAL_DUCK_SHOT_W; // Use global constant
            m_height = ::NORMAL_DUCK_SHOT_H; // Use global constant
            m_frameCount = 1; // Single shot frame
            break;
        case DuckState::FALLING:
            m_textureID = m_fallingTextureIDs[0]; // Pygame uses one frame, then flips it
            m_width = NORMAL_DUCK_FALL_W;
            m_height = NORMAL_DUCK_FALL_H;
            m_frameCount = 1; // For now, will handle flip based on anim frame later if needed
                               // Pygame's falling animation was: frame 0 = fall_img, frame 1 = fall_img_flipped
            break;
    }
    // Update collider for new dimensions
    m_collider.w = m_width;
    m_collider.h = m_height;
}

void NormalDuck::update(float deltaTime) {
    Duck::update(deltaTime); // Call base class update for movement, state changes, boundary checks

    // Animation frame update based on m_animTimer and m_animDelayTicks from base Duck class
    if (m_animTimer >= m_animDelayTicks) {
        m_animTimer = 0; // Reset timer
        m_animFrame++;   // Advance frame

        if (m_state == DuckState::FLYING) {
            const auto& current_anim_seq = (std::abs(m_velocity.y) < 0.1f) ? m_animTextureIDs_Flat : m_animTextureIDs_UD;
            if (static_cast<size_t>(m_animFrame) >= current_anim_seq.size()) {
                m_animFrame = 0; // Loop animation
            }
            m_textureID = current_anim_seq[m_animFrame];
        } else if (m_state == DuckState::FALLING) {
            // Pygame falling animation for normal duck:
            // frame 0 = duckImg[3][0] (normal fall)
            // frame 1 = duckImg[3][1] (flipped fall)
            // It seems to alternate between these two if m_frameCount for falling was > 1.
            // For now, using one frame. If Pygame used two distinct frames for falling, we need to adjust.
            // The Pygame code: self.anim = duckImg[3] (which is [fall, fall_flipped])
            // if self.frame >= len(self.anim): self.frame = 0. So it does cycle.
            // Let's assume m_fallingTextureIDs will hold "normal_duck_fall" and "normal_duck_fall_flipped" if needed.
            // For now, it's just one. If we want the flip:
            if (m_fallingTextureIDs.size() > 1) { // If we had loaded a flipped version
                 if (static_cast<size_t>(m_animFrame) >= m_fallingTextureIDs.size()) m_animFrame = 0;
                 m_textureID = m_fallingTextureIDs[m_animFrame];
            } else { // Simple single frame fall, or handle flip with SDL_RendererFlip
                 m_flip = (m_animFrame % 2 == 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
                 // m_textureID remains m_fallingTextureIDs[0]
                 if (m_animFrame >= 2) m_animFrame = 0; // Reset after two "visual" frames (normal, flipped)
            }
        }
        // No animation update for SHOT state as it's a single frame before transitioning to FALLING
    }

    // Set flip based on horizontal velocity for flying state
    if (m_state == DuckState::FLYING) {
        if (m_velocity.x < 0) {
            m_flip = SDL_FLIP_HORIZONTAL;
        } else {
            m_flip = SDL_FLIP_NONE;
        }
    } else if (m_state != DuckState::FALLING) { // Don't change flip if falling, unless fall anim itself handles it
         m_flip = SDL_FLIP_NONE;
    }
}
