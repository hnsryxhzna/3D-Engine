#ifndef VECTORMATH_H
#define VECTORMATH_H

typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    float x;
    float y;
    float z;
    float k;
} vec4;

typedef struct {
    float matrix[4][4];
} mat4;

#endif
