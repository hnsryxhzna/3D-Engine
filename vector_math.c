#include <stdio.h>
#include <vector_math.h>
#include <stdlib.h>
#include <math.h>

vec3 vector3(float x, float y, float z) {
    vec3 vector = {x, y, z};
    return vector;
}

vec3 vector3_plus(vec3 v0, vec3 v1) {
    vec3 vector = {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z};
    return vector;
}

vec4 vector4(float x, float y, float z, float k) {
    vec4 vector = {x, y, z, k};
    return vector;
}

vec4 vector4_plus(vec4 v0, vec4 v1) {
    vec4 vector = {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.k + v1.k};
    return vector;
}

float vec3_dot_product(vec3 v0, vec3 v1) {
    float product = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
    return product;
}

vec3 vec3_corss_product(vec3 v0, vec3 v1) {
    vec3 vector = {v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x};
    return vector;
}
