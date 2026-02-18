#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <string.h>

struct QuadtreeNode;
struct Quadtree;

typedef struct  {
    float x;
    float y;
} Vector2F;

typedef struct {
    int x;
    int y;
} Vector2;  

typedef struct  {
    float x;
    float y;
    float z;
} Vector3F;

typedef struct {
    int x;
    int y;
    int z;
} Vector3;  

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Vector4F;

typedef struct {
    int x;
    int y;
    int z;
    int w;
} Vector4;  


typedef struct {
    Vector2F position;
    Vector2F size;

    struct QuadtreeNode* children[4];
} QuadtreeNode;

typedef struct {
    QuadtreeNode* root;
} Quadtree;



const char* extract_filename_string(const char* path);

const char* copy_const_char(const char* source);


#endif