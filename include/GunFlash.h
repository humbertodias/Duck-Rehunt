#ifndef GUNFLASH_H
#define GUNFLASH_H

#include "Sprite.h"

class GunFlash : public Sprite {
public:
    GunFlash(const std::string& textureId, int x, int y, int w, int h, int lifeTimeFrames);

    void update(float deltaTime) override;
    bool isExpired() const;

private:
    int m_lifeTimeFrames; // How many frames the flash should last
    int m_framesLived;    // Counter for frames lived
};

#endif // GUNFLASH_H
