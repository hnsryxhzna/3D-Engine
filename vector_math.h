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

vec3 vector3(float x, float y, float z);
vec3 vector3_plus(vec3 v0, vec3 v1);
vec3 v3_sub(vec3 v0, vec3 v1);
vec3 vector3_scale(vec3 vector, float scale);
vec4 vector4(float x, float y, float z, float k);
vec4 vector4_plus(vec4 v0, vec4 v1);
float vec3_dot_product(vec3 v0, vec3 v1);
vec3 vec3_corss_product(vec3 v0, vec3 v1);
float vec3_len(vec3 vector);
vec3 vec3_norm(vec3 vector);
mat4 mat4_identity();
mat4 mat4_mul(mat4 matrix0, mat4 matrix1);
vec4 mat4_mul_vec4(mat4 matrix, vec4 vector);
vec3 mat4_mul_point(mat4 matrix, vec3 vector);
mat4 mat4_translate(float tx, float ty, float tz);
mat4 mat4_rotate_x(float a);
mat4 mat4_rotate_y(float a);
mat4 mat4_look_at(vec3 eye, vec3 target, vec3 up);
mat4 mat4_perspective(float fovy_rad, float aspect, float near, float far);

#endif
