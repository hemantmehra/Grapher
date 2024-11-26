#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <raylib.h>

int screen_width = 1600;
int screen_height = 900;

#define SAMPLE_SIZE 4000

typedef enum {
    PLUS,
    MUL,
    SIN,
    COS
} Operator;

typedef enum {
    Binary,
    Symbol,
    Number
} Kind;

struct _ast {
    Kind kind;

    union
    {
        struct
        {
            Operator op;
            struct _ast *left;
            struct _ast *right;
        } binary;
        double number;
        char symbol;
    } main;
};

typedef struct _ast Ast;

double translate_x(double x, double scale)
{
    return (x * scale + (double) screen_width / 2);
}

double translate_y(double y, double scale)
{
    return (double) screen_height / 2 - y * scale;
}

double func1(double x)
{
    return  x * cos(x) * sin(x);
}

void plot_points(Vector2 *points, double step, double scale)
{
    int i = 0;
    double x = - step * SAMPLE_SIZE/2;
    double y;
    while (i < SAMPLE_SIZE) {
        // printf("%f\n", x);
        y = func1(x);
        int yt = translate_y(y, scale);
        int xt = translate_x(x, scale);
        Vector2 p = {xt, yt};
        points[i] = p;
        x += step;
        i++;
    }
}

double eval_ast(Ast *ast, double x)
{
    switch (ast->kind)
    {
    case Symbol:
        return x;
    
    case Binary:
        {
            switch(ast->main.binary.op) {
                case PLUS:
                {
                    double l = eval_ast(ast->main.binary.left, x);
                    double r = eval_ast(ast->main.binary.right, x);
                    return l + r;
                }

                default:
                    assert(false);
                    break;
            }
            break;
        }
    
    case Number:
        return ast->main.number;

    default:
        assert(false);
        break;
    }
}

int main(void)
{
    InitWindow(screen_width, screen_height, "Grapher");
    double scale = 50;
    double step = 0.01;
    Vector2 points[SAMPLE_SIZE];

    Ast ast_x;
    ast_x.kind = Symbol;
    ast_x.main.symbol = 'x';

    Ast ast_4;
    ast_4.kind = Number;
    ast_4.main.number = 4;

    Ast ast;
    ast.kind = Binary;
    ast.main.binary.left = &ast_x;
    ast.main.binary.right = &ast_4;

    double r = eval_ast(&ast, 5);
    printf("[r=%lf]\n", r);

    plot_points(points, step, scale);
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
