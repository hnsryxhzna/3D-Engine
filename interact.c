#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <signal.h>
#include <termios.h>

#define TWO_PI 6.28318530718f

static struct termios saved_tio;
 
static void restore_terminal(void) {
    printf("\x1b[?1002l\x1b[?1006l\x1b[?25h");
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_tio);
}
 
static void on_sigint(int sig) {
    (void) sig;
    exit(0);
}
 
static void setup_terminal(void) {
    tcgetattr(STDIN_FILENO, &saved_tio);
    struct termios raw = saved_tio;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    atexit(restore_terminal);
    signal(SIGINT, on_sigint);
    printf("\x1b[?1002h\x1b[?1006h\x1b[?25l");
}


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

    setup_terminal();
    char buffer[256];
    int len = 0;
    int dragging = 0;
    int last_col = 0;
    int last_row = 0;

    printf("\x1b[2J");
    for (;;) {
        printf("\x1b[H");
        render_frame(A, B);
        fflush(stdout);

        struct pollfd pfd = {
            STDIN_FILENO, POLLIN, 0
        };

        if (poll(&pfd, 1, 30) <= 0) {
            continue;
        }

        int n = read(STDIN_FILENO, buffer + len, sizeof(buffer) - 1 - len);
        if (n <= 0) {
            continue;
        }

        len += n;
        buffer[len] = '\0';
 
        int i = 0;
        while (i < len) {
            int btn, col, row, used = 0;
            char kind;
            if (buffer[i] == 'q') exit(0);
            // TODO: FIX
            if (sscanf(buffer + i, "\x1b[<%d;%d;%d%c%n", &btn, &col, &row, &kind, &used) == 4) {
                if (kind == 'm') {
                    dragging = 0;
                } else if (btn == 0) {
                    dragging = 1;
                } else if (btn == 32 && dragging) {
                    A -= (row - last_row) * 0.10f;
                    B -= (col - last_col) * 0.05f;
                }

                last_col = col;
                last_row = row;
                i += used;
            } else if (buffer[i] == '\x1b' && len - i < 16) {
                break;
            } else {
                i++;
            }
        }

        memmove(buffer, buffer + i, len - i);
        len -= i;
    }

    return 0;
}
