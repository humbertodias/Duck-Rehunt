#ifndef NEGADUCK_H
#define NEGADUCK_H

#include "Duck.h"
#include <vector>
#include <string>

class NegaDuck : public Duck {
public:
    NegaDuck(int x, int y);
    ~NegaDuck() override = default;

    void setAnimation(DuckState state) override;
    void update(float deltaTime) override;

    static void loadSharedAssets(SDL_Renderer* pRenderer);

private:
    // Pygame: negaDuckImg[0] = Sp (shot/falling?), negaDuckImg[1] = Flap (DarkwingFlap, DarkwingFlap2)
    // NegaDuck in Pygame used negaDuckImg[0] for its "dead" animation (which was just DarkwingSp.gif)
    // and negaDuckImg[1] for its flying animation.
    // The "shot" visual was the standard duck shot image (duckImg[2][0]) before falling.

    std::vector<std::string> m_flyingAnimTextureIDs;
    std::string m_shotTextureID_Nega; // This will actually be the standard duck shot texture ID
    std::string m_fallingTextureID_Nega; // This will be the DarkwingSp.gif

    static std::vector<std::string> s_textureIDs_Nega_Flying;
    static std::string s_textureID_Nega_Falling; // DarkwingSp.gif
    static std::string s_textureID_Standard_Shot; // For the brief "shot" moment
    static bool s_assetsLoaded_Nega;
};

#endif // NEGADUCK_H
