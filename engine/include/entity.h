#ifndef ENTITY_H
#define ENTITY_H
#include "types.h"
#include "sprite.h"

typedef struct
{
    Vector3F        worldPosition;
    Vector2F        scale;
    float           rotation;

    struct Entity*  parent;

    struct Sprite*  currentSprite;

    void            (*think)(struct Entity* entity);
    void            (*update)(struct Entity* entity);
    void            (*draw)(struct Entity* entity, SDL_Renderer* renderer);
    int             (*destroy)(struct Entity* entity);
}Entity;

/*Entity* createEntity(Vector3F position,
    Vector2F scale, 
    float rotation, 
    Entity* parent, Sprite* sprite, void (*think)(Entity* entity), void (*update)(Entity* entity), void (*draw)(Entity* entity, SDL_Renderer* renderer), int (*destroy)(Entity* entity));
    */
#endif
