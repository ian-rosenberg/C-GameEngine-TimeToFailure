#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include "sprite.h"

#define SCREEN_TITLE "Time To Failure"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct GraphicsEngine;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SpriteManager* spriteManager;
}GraphicsEngine;

GraphicsEngine* initGraphicsEngine();

void engineLoop(GraphicsEngine* engine);

void destroyEngine(GraphicsEngine* engine);

#endif