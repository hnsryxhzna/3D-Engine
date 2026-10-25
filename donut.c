#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#define TWO_PI 6.28318530718f

int render_frame (float A, float B) {
    const float theta_spacing = 0.07;
    const float phi_spacing = 0.02;

    float cA = cosf(A);
    float sA = sinf(A);
    float cB = cosf(B);
    float sB = sinf(B);

    const int W = 100; // 210
    const int H = 50; // 68
    const float R1 = 1.0f;
    const float R2 = 2.0f;
    const float K2 = 5.0f;
    const float K1 = W * K2 * 3.0f / (8.0f * (R1 + R2)); 

    const char *shades = ".,-~:;=!*#$@";

    char output[H][W];
    float z_buffer[H][W];
    memset(output, ' ', sizeof(output));
    memset(z_buffer, 0, sizeof(z_buffer));

    for (float theta = 0; theta < TWO_PI; theta += theta_spacing) {
        float cos_theta = cosf(theta);
        float sin_theta = sinf(theta);

        for (float phi = 0; phi < TWO_PI; phi += phi_spacing) {
            float cos_phi = cosf(phi);
            float sin_phi = sinf(phi);

            float circle_x = R2 + R1 * cos_theta;
            float circle_y = R1 * sin_theta;

            float x = circle_x * (cB * cos_phi + sA * sB * sin_phi) - (R1 * cA * sB * sin_theta);
            float y = circle_x * (cos_phi * sB - cB * sA * sin_phi) + (R1 * cA * cB * sin_theta);
            float z = cA * circle_x * sin_phi + R1 * sA * sin_theta;
            float ooz = 1.0f / (z + K2);

            // perspective projection
            int px = (int) (W / 2 + K1 * ooz * x);
            int py = (int) (H / 2 - K1 * ooz * y * 0.5f);

            // surface normal
            float L = cos_phi * cos_theta * sB - cA * cos_theta * sin_phi - sA * sin_theta + cB * (cA * sin_theta - cos_theta * sA * sin_phi);

            if (L > 0 && px >= 0 && px < W && py >= 0 && py < H) {
                if (ooz > z_buffer[py][px]) {
                    z_buffer[py][px] = ooz;
                    int index = (int) (L * 8.0f);
                    if (index < 0) index = 0;
                    if (index > 11) index = 11;
                    output[py][px] = shades[index];
                }
            }
        }
    }

    for (int j = 0; j < H; j += 1) {
        for (int i = 0; i < W; i += 1) {
            putchar(output[j][i]);
        }
        putchar(10);
    }

    return 0;
}

int main () {
    float A = 0.0f;
    float B = 0.0f;

    printf("\x1b[2J");
    for (;;) {
        printf("\x1b[H");
        render_frame(A, B);
        A += 0.04f;
        B += 0.03f;
        usleep(30000);
    }

    return 0;
}
