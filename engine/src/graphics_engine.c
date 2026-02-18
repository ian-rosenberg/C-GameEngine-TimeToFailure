#include "graphics_engine.h"


GraphicsEngine* initGraphicsEngine() {
    GraphicsEngine* engine = malloc(sizeof(GraphicsEngine));
    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;

    engine = calloc(1, sizeof(GraphicsEngine));

    if (engine == NULL) {
        printf("Memory allocation for GraphicsEngine failed!\n");
        exit(1);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        free(engine);
        exit(1);
    }

    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        free(engine);
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        exit(1);
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); 

    engine->window = window;
    engine->renderer = renderer;


    engine->spriteManager = createSpriteManager();

    return engine;
}

void engineLoop(GraphicsEngine* engine) {
    SDL_Event e;
    unsigned int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }
        }
        SDL_RenderClear(engine->renderer);

        drawAll(engine->spriteManager, engine->renderer);

        SDL_RenderPresent(engine->renderer);
    }
    
    destroyEngine(engine);
}

void destroyEngine(GraphicsEngine* engine) {
    SDL_DestroyRenderer(engine->renderer);
    SDL_DestroyWindow(engine->window);
    engine->renderer = NULL;
    engine->window = NULL;  
    SDL_Quit();
    free(engine);
}

