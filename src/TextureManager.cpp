#include "TextureManager.h"
#include <iostream>

TextureManager* TextureManager::s_pInstance = nullptr;

TextureManager* TextureManager::Instance() {
    if (s_pInstance == nullptr) {
        s_pInstance = new TextureManager();
    }
    return s_pInstance;
}

TextureManager::~TextureManager() {
    clean();
}

bool TextureManager::load(std::string fileName, std::string id, SDL_Renderer* pRenderer) {
    SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());

    if (pTempSurface == nullptr) {
        std::cerr << "Failed to load image: " << fileName << " - " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
    SDL_FreeSurface(pTempSurface);

    if (pTexture == nullptr) {
        std::cerr << "Failed to create texture from surface: " << fileName << " - " << SDL_GetError() << std::endl;
        return false;
    }

    m_textureMap[id] = pTexture;
    return true;
}

bool TextureManager::load(std::string fileName, std::string id, SDL_Renderer* pRenderer, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Surface* pTempSurface = IMG_Load(fileName.c_str());

    if (pTempSurface == nullptr) {
        std::cerr << "Failed to load image: " << fileName << " - " << IMG_GetError() << std::endl;
        return false;
    }

    // Set color key
    SDL_SetColorKey(pTempSurface, SDL_TRUE, SDL_MapRGB(pTempSurface->format, r, g, b));

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pTempSurface);
    SDL_FreeSurface(pTempSurface);

    if (pTexture == nullptr) {
        std::cerr << "Failed to create texture from surface: " << fileName << " - " << SDL_GetError() << std::endl;
        return false;
    }

    m_textureMap[id] = pTexture;
    return true;
}


void TextureManager::draw(std::string id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip) {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = destRect.w = width;
    srcRect.h = destRect.h = height;
    destRect.x = x;
    destRect.y = y;

    SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
}

// Assumes texture is a spritesheet, draws a specific frame
void TextureManager::drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, double angle, SDL_RendererFlip flip) {
    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.x = width * currentFrame;
    srcRect.y = height * currentRow;
    srcRect.w = width;
    srcRect.h = height;

    destRect.x = x;
    destRect.y = y;
    destRect.w = width;
    destRect.h = height;

    SDL_RenderCopyEx(pRenderer, m_textureMap[id], &srcRect, &destRect, angle, nullptr, flip);
}

// More generic tile drawing, not used in this project but good for a generic manager
void TextureManager::drawTile(std::string id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer)
{
    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.x = margin + (spacing + width) * currentFrame;
    srcRect.y = margin + (spacing + height) * currentRow;
    srcRect.w = destRect.w = width;
    srcRect.h = destRect.h = height;
    destRect.x = x;
    destRect.y = y;

    SDL_RenderCopy(pRenderer, m_textureMap[id], &srcRect, &destRect);
}


SDL_Texture* TextureManager::getTexture(std::string id) {
    return m_textureMap[id]; // Returns nullptr if id doesn't exist
}

void TextureManager::clearTexture(std::string id) {
    SDL_Texture* texture = m_textureMap[id];
    if (texture) {
        SDL_DestroyTexture(texture);
        m_textureMap.erase(id);
    }
}

void TextureManager::clean() {
    for (auto const& [id, texture] : m_textureMap) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    m_textureMap.clear();
    // Note: s_pInstance is deleted by whoever calls delete on TextureManager::Instance() or when program ends if it's a global.
    // However, the destructor is private. This singleton is meant to live for the duration of the app.
    // A proper cleanup would involve a static deallocator or a method called at shutdown.
    // For this project, SDL_Quit() will handle texture memory associated with renderers.
}
