#ifndef NORMALDUCK_H
#define NORMALDUCK_H

#include "Duck.h"
#include <map>

class NormalDuck : public Duck {
public:
    NormalDuck(int x, int y);
    ~NormalDuck() override = default;

    void setAnimation(DuckState state) override;
    void update(float deltaTime) override; // To handle specific animation frame updates

    static void loadSharedAssets(SDL_Renderer* pRenderer); // Static method to load textures once

private:
    // Texture IDs for different animation states/directions
    // Pygame: duckImg[0] = Up/Down (UD1, UD2, UD3), duckImg[1] = Flat (F1, F2, F3)
    // duckImg[2] = Shot, duckImg[3] = Fall (Fall, Fall_flipped)

    // We'll use a simpler approach: store texture IDs for sequences
    std::vector<std::string> m_animTextureIDs_UD;    // Up/Down flying
    std::vector<std::string> m_animTextureIDs_Flat;  // Horizontal flying
    std::string m_shotTextureID;
    std::vector<std::string> m_fallingTextureIDs; // Normal and flipped

    // Static members for texture IDs, so they are shared among all NormalDuck instances
    // And loaded only once by loadAssets()
    static std::vector<std::string> s_textureIDs_UD;
    static std::vector<std::string> s_textureIDs_Flat;
    static std::string s_shotTextureID_Normal;
    static std::vector<std::string> s_fallingTextureIDs_Normal;
    static bool s_assetsLoaded;
};

#endif // NORMALDUCK_H
