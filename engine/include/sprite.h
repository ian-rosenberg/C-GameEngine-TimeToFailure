#ifndef SPRITE_H
#define SPRITE_H

#include "types.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>

struct SpriteManager;
struct Sprite;

typedef struct {
    unsigned int                id;
    unsigned short              refCount;
    const char*                 name;
    SDL_Texture*                texture; // Ensure consistency here
    SDL_Rect                    srcRect;
    SDL_Rect                    destRect;   
    SDL_RendererFlip            flipFlags;
} Sprite;

typedef struct {
    SDL_Texture* texture;
    Sprite** sprites;
    int count;
} SpriteManager;
 
SpriteManager* createSpriteManager();

int loadSpriteSheetFromJson(SpriteManager* manager, SDL_Renderer* renderer, const char* jsonPath);

Sprite* getSpriteFromManager(SpriteManager* manager, int index);

Sprite* createSprite(SpriteManager* manager, SDL_Renderer* renderer, const char* path, int w, int h);

void destroySprite(SpriteManager* manager, Sprite* sprite);

void renderSprite(Sprite* sprite, SDL_Renderer* renderer);

Sprite* getSpriteFromManager(SpriteManager* manager, int index);

void drawAll(SpriteManager* manager, SDL_Renderer* renderer);

void getSpriteSize(Sprite* sprite, int* w, int* h);

#endif