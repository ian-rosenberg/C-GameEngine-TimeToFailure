#include "sprite.h"
#include <string.h>
#include <ctype.h>

// Helper to extract integer from JSON key
static int extract_json_int(const char* json, const char* key) {
    char* pos = strstr(json, key);
    if (!pos) return -1;
    pos = strchr(pos, ':');
    if (!pos) return -1;
    return atoi(pos + 1);
}

// Helper to extract string from JSON key
static void extract_json_string(const char* json, const char* key, char* out, int maxLen) {
    char* pos = strstr(json, key);
    if (!pos) return;
    pos = strchr(pos, ':');
    if (!pos) return;
    pos = strchr(pos, '"');
    if (!pos) return;
    pos++;
    int i = 0;
    while (*pos && *pos != '"' && i < maxLen - 1) {
        out[i++] = *pos++;
    }
    out[i] = '\0';
}

int loadSpriteSheetFromJson(SpriteManager* manager, SDL_Renderer* renderer, const char* jsonPath) {
    FILE* file = fopen(jsonPath, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open JSON: %s\n", jsonPath);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);

    char* json = (char*)malloc(len + 1);
    fread(json, 1, len, file);
    json[len] = '\0';
    fclose(file);

    int columns = extract_json_int(json, "\"columns\"");
    int tilewidth = extract_json_int(json, "\"tilewidth\"");
    int tileheight = extract_json_int(json, "\"tileheight\"");
    int margin = extract_json_int(json, "\"margin\"");
    int spacing = extract_json_int(json, "\"spacing\"");
    int tilecount = extract_json_int(json, "\"tilecount\"");
    int startIndex = extract_json_int(json, "\"startIndex\"");

    char imagePath[256];
    extract_json_string(json, "\"spritesheet\"", imagePath, sizeof(imagePath));

    free(json); // Done with raw JSON

    SDL_Surface* surface = IMG_Load(imagePath);
    if (!surface) {
        fprintf(stderr, "Failed to load image: %s\n", IMG_GetError());
        return 0;
    }

    manager->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    manager->sprites = (Sprite**)malloc(tilecount * sizeof(Sprite*));
    manager->count = tilecount;

    for (int i = 0; i < tilecount; i++) {
        Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));
        sprite->id = startIndex + i;
        sprite->refCount = 1;
        sprite->name = NULL;
        sprite->texture = manager->texture;

        int col = i % columns;
        int row = i / columns;

        sprite->srcRect.x = margin + col * (tilewidth + spacing);
        sprite->srcRect.y = margin + row * (tileheight + spacing);
        sprite->srcRect.w = tilewidth;
        sprite->srcRect.h = tileheight;

        sprite->destRect = sprite->srcRect;
        sprite->flipFlags = SDL_FLIP_NONE;

        manager->sprites[i] = sprite;
    }

    return 1;
}

SpriteManager* createSpriteManager() {
    SpriteManager* manager = (SpriteManager*)malloc(sizeof(SpriteManager));
    if (!manager)
        return NULL;

    manager->sprites = (Sprite**)malloc(sizeof(Sprite*));
    manager->count = 0;
    return manager;
}

Sprite* createSprite(SpriteManager* manager, SDL_Renderer* renderer, const char* path, int w, int h) {
    Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));
    SDL_Surface* surface = IMG_Load(path);
    if (sprite == NULL) {
        if(surface != NULL)
            SDL_FreeSurface(surface);
        return NULL;
    }

    sprite->texture = (SDL_Texture**)malloc(sizeof(SDL_Texture*));
    if (sprite->texture == NULL) {
        free(sprite);
        if(surface != NULL)
            SDL_FreeSurface(surface);
        return NULL;
    }

    *(sprite->texture) = SDL_CreateTextureFromSurface(renderer, surface);
    
    sprite->srcRect = (SDL_Rect){ 0, 0, w, h };
    sprite->destRect = (SDL_Rect){ 0, 0, w, h };
    sprite->flipFlags = SDL_FLIP_NONE;
    sprite->refCount = 1;
    sprite->id = manager->count;
    sprite->name = extract_filename_string(path);

    Sprite** temp = (Sprite**)realloc(manager->sprites, (manager->count + 1) * sizeof(Sprite*));
    if (temp == NULL) {
        SDL_FreeSurface(surface);
        free(sprite->texture);
        free(sprite);
        return NULL;
    }
    manager->sprites = temp;
    manager->sprites[manager->count] = sprite;
    manager->count++;

    SDL_FreeSurface(surface);

    return sprite;
}


Sprite* copySprite(SpriteManager* manager, SDL_Renderer* renderer, Sprite* old) {
    Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));
    if (sprite == NULL) {
        return NULL;
    }
    sprite->texture = old->texture;
    sprite->srcRect = old->srcRect;
    sprite->destRect = old->destRect;
    sprite->flipFlags = old->flipFlags;
    sprite->name = copy_const_char(old->name);
    return sprite;
}

void destroySprite(SpriteManager* manager, Sprite* sprite) {
    if (sprite->texture != NULL && sprite->refCount == 0) {
        SDL_DestroyTexture(*sprite->texture);
    }

    free(sprite);
}


void renderSprite(Sprite* sprite, SDL_Renderer* renderer) {
    SDL_RenderCopyEx(renderer, 
    *sprite->texture, 
    &sprite->srcRect,
    &sprite->destRect, 
    0,
    NULL,
    sprite->flipFlags);
}

Sprite* getSpriteFromManager(SpriteManager* manager, int index) {
    if (index < 0 || index >= manager->count) {
        return NULL;
    }
    return manager->sprites[index];
}

void drawAll(SpriteManager* manager, SDL_Renderer* renderer) {
    for (int i = 0; i < manager->count; i++) {
        renderSprite(manager->sprites[i], renderer);
    }
}

void getSpriteSize(Sprite* sprite, int* w, int* h) {
    if (sprite != NULL && sprite->texture != NULL && sprite->srcRect.w > 0 && sprite->srcRect.h > 0)
        SDL_QueryTexture(*sprite->texture, NULL, NULL, w, h);
    else if (sprite == NULL)
    {
        printf("Sprite is NULL, no dimensions to return...\n");
        return;
    }
    else {
        *w = sprite->srcRect.w;
        *h = sprite->srcRect.h;
    }
}