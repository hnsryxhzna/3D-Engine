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

vec3 v3_sub(vec3 v0, vec3 v1) {
    vec3 vector = {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z};
    return vector;
}

vec3 vector3_scale(vec3 vector, float scale) {
    vec3 vector = {vector.x * scale, vector.y * scale, vector.z * scale};
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

float vec3_len(vec3 vector) {
    return sqrt(vec3_dot_product(vector, vector));
}

vec3 vec3_norm(vec3 vector) {
    float length = vec3_len(vector);
    if (length < 1e-8f) {
        return vector;
    }
    return vector3_scale(vector, 1.0f / length);
}

mat4 mat4_identity() {
    mat4 M = {{{0}}};

    for (int i = 0; i < 4; i++) {
        M.matrix[i][i] = 1.0f;
    }

    return M;
}

mat4 mat4_mul(mat4 matrix0, mat4 matrix1) {
    mat4 matrix2 = {{{0}}};

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++){
            float s = 0.0f;

            for (int k = 0; k < 4; k++) {
                s += matrix0.matrix[i][k] * matrix1.matrix[k][j];
            }
            matrix2.matrix[i][j] = s;
        }

    return matrix2;
}

vec4 mat4_mul_vec4(mat4 matrix, vec4 vector) {
    vec4 res;
    res.x = matrix.matrix[0][0] * vector.x + matrix.matrix[0][1] * vector.y + matrix.matrix[0][2] * vector.z + matrix.matrix[0][3] * vector.k;
    res.y = matrix.matrix[1][0] * vector.x + matrix.matrix[1][1] * vector.y + matrix.matrix[1][2] * vector.z + matrix.matrix[1][3] * vector.k;
    res.z = matrix.matrix[2][0] * vector.x + matrix.matrix[2][1] * vector.y + matrix.matrix[2][2] * vector.z + matrix.matrix[2][3] * vector.k;
    res.k = matrix.matrix[3][0] * vector.x + matrix.matrix[3][1] * vector.y + matrix.matrix[3][2] * vector.z + matrix.matrix[3][3] * vector.k;
    return res;
}

vec3 mat4_mul_point(mat4 matrix, vec3 vector) {
    vec4 res = mat4_mul_vec4(matrix, (vec4){vector.x, vector.y, vector.z, 1.0f});
    return vector3(res.x, res.y, res.z);
}

mat4 mat4_translate(float tx, float ty, float tz) {
    mat4 res = mat4_identity();
    res.matrix[0][3] = tx; res.matrix[1][3] = ty; res.matrix[2][3] = tz;
    return res;
}

mat4 mat4_rotate_x(float a) {
    mat4 res = mat4_identity();
    float cos = cosf(a);
    float sin = sinf(a);

    res.matrix[1][1] = cos;
    res.matrix[1][2] = -sin;
    res.matrix[2][1] = sin;
    res.matrix[2][2] = cos;
    return res;
}

mat4 mat4_rotate_y(float a) {
    mat4 res = mat4_identity();
    float cos = cosf(a);
    float sin = sinf(a);

    res.matrix[0][0] = cos;
    res.matrix[0][2] = sin;
    res.matrix[2][0] = -sin;
    res.matrix[2][2] = cos;
    return res;
}

mat4 mat4_look_at(vec3 eye, vec3 target, vec3 up) {
    vec3 z = vec3_norm(v3_sub(eye, target));
    vec3 x = vec3_norm(vec3_corss_product(up, z));
    vec3 y = vec3_corss_product(z, x);
    mat4 res = mat4_identity();

    res.matrix[0][0] = x.x; res.matrix[0][1] = x.y; res.matrix[0][2] = x.z; res.matrix[0][3] = -vec3_dot_product(x, eye);
    res.matrix[1][0] = y.x; res.matrix[1][1] = y.y; res.matrix[1][2] = y.z; res.matrix[1][3] = -vec3_dot_product(y, eye);
    res.matrix[2][0] = z.x; res.matrix[2][1] = z.y; res.matrix[2][2] = z.z; res.matrix[2][3] = -vec3_dot_product(z, eye);
    return res;
}

mat4 mat4_perspective(float fovy_rad, float aspect, float near, float far) {
    float f = 1.0f / tanf(fovy_rad * 0.5f);
    mat4 res = {{{0}}};

    res.matrix[0][0] = f / aspect;
    res.matrix[1][1] = f;
    res.matrix[2][2] = (far + near) / (near - far);
    res.matrix[2][3] = (2.0f * far * near) / (near - far);
    res.matrix[3][2] = -1.0f;
    return res;
}
