#include <math.h>
#include <stdio.h>
#include <raylib.h>

int screen_width = 1600;
int screen_height = 900;

#define SAMPLE_SIZE 4000

double translate_x(double x, double scale)
{
    return (x * scale + (double) screen_width / 2);
}

double translate_y(double y, double scale)
{
    return (double) screen_height / 2 - y * scale;
}

int main(void)
{
    InitWindow(screen_width, screen_height, "Grapher");
    double scale = 50;
    double step = 0.01;
    Vector2 points[SAMPLE_SIZE];

    int i = 0;
    double x = - step * SAMPLE_SIZE/2;
    double y;
    while (i < SAMPLE_SIZE) {
        // printf("%f\n", x);
        y = x * cos(x) * sin(x);
        int yt = translate_y(y, 50);
        int xt = translate_x(x, 50);
        Vector2 p = {xt, yt};
        points[i] = p;
        x += step;
        i++;
    }

    // for (int i = 0; i < SAMPLE_SIZE; i++) {
    //     printf("%f %f\n", points[i].x, points[i].y);
    // }

    while(!WindowShouldClose()) {
        if (IsKeyDown(KEY_Q)) break;
        BeginDrawing();
        ClearBackground(BLACK);

        DrawLine(screen_width/2, 0, screen_width/2, screen_height, WHITE);
        DrawLine(0, screen_height/2, screen_width, screen_height/2, WHITE);

        int i = 0;
        while(i < screen_width) {
            int k = i * 50 + screen_width/2;
            // DrawLine(k, 0, k, screen_height, GRAY);
            i++;
        }

        i = 0;
        while(i < screen_height) {
            int k = i * 50 + screen_height/2;
            // DrawLine(0, k, screen_width, k, GRAY);
            i++;
        }

        for (int i = 0; i < SAMPLE_SIZE-1; i++) {
            // DrawCircleV(points[i], 2, WHITE);
            DrawLineV(points[i], points[i+1], BLUE);
        }

        EndDrawing();
    }

    CloseWindow();
}
