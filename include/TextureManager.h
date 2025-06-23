#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>

class TextureManager {
public:
    static TextureManager* Instance();

    bool load(std::string fileName, std::string id, SDL_Renderer* pRenderer);
    // Load with color key
    bool load(std::string fileName, std::string id, SDL_Renderer* pRenderer, Uint8 r, Uint8 g, Uint8 b);

    void draw(std::string id, int x, int y, int width, int height, SDL_Renderer* pRenderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void drawFrame(std::string id, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer, double angle = 0, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void drawTile(std::string id, int margin, int spacing, int x, int y, int width, int height, int currentRow, int currentFrame, SDL_Renderer* pRenderer);

    SDL_Texture* getTexture(std::string id);
    void clearTexture(std::string id); // Remove a specific texture
    void clean(); // Remove all textures and clean up

private:
    TextureManager() {}
    ~TextureManager(); // Private destructor for singleton
    TextureManager(const TextureManager&);
    TextureManager& operator=(const TextureManager&);

    std::map<std::string, SDL_Texture*> m_textureMap;
    static TextureManager* s_pInstance;
};

#endif // TEXTUREMANAGER_H
