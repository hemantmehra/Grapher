/*
MIT License

Copyright (c) 2024 Hemant Mehra

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <raylib.h>

int screen_width = 1600;
int screen_height = 900;

#define SAMPLE_SIZE 4000
#define TOKENS_COUNT 1000

typedef enum {
    T_Open,
    T_Close,
    T_Operator,
    T_Symbol,
    T_Number
} TokenKind;

typedef enum {
    PLUS,
    MUL,
    SIN,
    COS,
    INVALID
} Operator;

struct _token {
    TokenKind kind;

    union {
        Operator operator;
        char symbol;
        double number;
    } main;
};

typedef struct _token Token;

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

double eval_ast(Ast *ast, double x);

void plot_points(Ast* ast, Vector2 *points, double step, double scale)
{
    int i = 0;
    double x = - step * SAMPLE_SIZE/2;
    double y;
    while (i < SAMPLE_SIZE) {
        // printf("%f\n", x);
        y = eval_ast(ast, x);
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
        // printf("eval_ast: Symbol x %lf \n", x);
        return x;
    
    case Binary:
        {
            // printf("eval_ast: Binary\n");
            switch(ast->main.binary.op) {
                case PLUS:
                {
                    double l = eval_ast(ast->main.binary.left, x);
                    double r = eval_ast(ast->main.binary.right, x);
                    return l + r;
                }

                case MUL:
                {
                    double l = eval_ast(ast->main.binary.left, x);
                    double r = eval_ast(ast->main.binary.right, x);
                    return l * r;
                }

                case COS:
                {
                    double l = eval_ast(ast->main.binary.left, x);
                    return cos(l);
                }

                case SIN:
                {
                    double l = eval_ast(ast->main.binary.left, x);
                    return sin(l);
                }

                default:
                    printf("[ASSERT] %d\n", ast->main.binary.op);
                    assert(false);
                    break;
            }
            break;
        }
    
    case Number:
        // printf("eval_ast: Number %lf \n", ast->main.number);
        return ast->main.number;

    default:
        assert(false);
        break;
    }
}

struct _string {
    char *str;
    int length;
};

typedef struct _string String;

void str_add_char(String *str, char ch)
{
    int l = str->length;
    str->str[l] = ch;
    str->length++;
}

bool str_eq(String str1, char *str2)
{
    int l1 = str1.length;
    int l2 = strlen(str2);

    if (l1 != l2) return false;

    int i = 0;
    int j = 0;
    while(i < l1) {
        if (str1.str[i++] != str2[j++]) return false;
    }
    return true;
}

Operator get_operator(String str)
{
    if (str_eq(str, "*")) return MUL;
    if (str_eq(str, "+")) return PLUS;
    if (str_eq(str, "sin")) return SIN;
    if (str_eq(str, "cos")) return COS;
    return INVALID;
}

bool is_number(String str)
{
    int i = 0;
    while (i < str.length) {
        bool digit_check = str.str[i] >= '0' && str.str[i] <= '9';
        if (!digit_check) return false;
        i++;
    }
    return true;
}

double parse_number(String str)
{
    double num = 0;
    for (int i=0; i < str.length; i++) {
        num += pow(10, i) * (str.str[i] - '0');
    }
    return num;
}

Token get_token(String word)
{
    Token t;
    if(is_number(word)) {
        t.kind = T_Number;
        t.main.number = parse_number(word);
    }
    else {
        Operator op = get_operator(word);
        if (op == INVALID) {
            t.kind = T_Symbol;
            t.main.symbol = word.str[0];
        }
        else {
            t.kind = T_Operator;
            t.main.operator = op;
        }
    }
    return t;
}

int tokenize(Token *tokens, String function)
{
    bool in_word = false;
    char buffer[100];
    String word = {buffer, 0};
    int i = 0;
    int j = 0;
    while(i < function.length) {
        printf("[-] %c\n", function.str[i]);

        if (function.str[i] == '(') {
            tokens[j++] = CLITERAL(Token) {.kind = T_Open, .main = 0};
        }
        else if (function.str[i] == ')') {
            if (word.length != 0) {
                Token t = get_token(word);
                word.length = 0;
                tokens[j++] = t;
            }
            tokens[j++] = CLITERAL(Token) {.kind = T_Close, .main = 0};
            in_word = false;
        }
        else {
            if (in_word && !isspace(function.str[i])) {
                str_add_char(&word, function.str[i]);
            }
            else if (in_word && isspace(function.str[i])) {
                if (word.length != 0) {
                    Token t = get_token(word);
                    word.length = 0;
                    tokens[j++] = t;
                    in_word = false;
                }
            }
            else if (!in_word && isspace(function.str[i])) {
                if (word.length != 0) {
                    Token t = get_token(word);
                    word.length = 0;
                    tokens[j++] = t;
                    in_word = false;
                }
            }
            else if (!isspace(function.str[i])) {
                str_add_char(&word, function.str[i]);
                in_word = true;
            }
            else assert(false);
        }
        i++;
    }

    return j;
}

struct _arena {
    char *ptr;
    int pos;
};

typedef struct _arena Arena;

char* arena_alloc(Arena *a, int size)
{
    int p = a->pos;
    a->pos += size;
    return a->ptr + p;
}

struct _ast_idx_pair {
    Ast *ast;
    int idx;
};

typedef struct _ast_idx_pair Ast_idx_pair;

Ast_idx_pair parse_ast(Arena *arena, Token *tokens, int idx)
{
    if(tokens[idx].kind == T_Open) {
        printf("parse open\n");
        Ast * ast = (Ast*) arena_alloc(arena, sizeof(Ast));
        ast->kind = Binary;
        ast->main.binary.op = tokens[idx+1].main.operator;
        Ast_idx_pair p1 = parse_ast(arena, tokens, idx+2);
        ast->main.binary.left = p1.ast;

        if (tokens[p1.idx].kind == T_Close) {
            return CLITERAL(Ast_idx_pair) {ast, p1.idx+1};
        }

        Ast_idx_pair p2 = parse_ast(arena, tokens, p1.idx);
        ast->main.binary.right = p2.ast;

        return CLITERAL(Ast_idx_pair) {ast, p2.idx+1};
    }

    if (tokens[idx].kind == T_Number) {
        printf("parse number\n");
        Ast * ast = (Ast*) arena_alloc(arena, sizeof(Ast));
        ast->kind = Number;
        ast->main.number = tokens[idx].main.number;
        return CLITERAL(Ast_idx_pair) {ast, idx+1};
    }

    if (tokens[idx].kind == T_Symbol) {
        printf("parse symbol\n");
        Ast * ast = (Ast*) arena_alloc(arena, sizeof(Ast));
        ast->kind = Symbol;
        ast->main.symbol = tokens[idx].main.symbol;
        return CLITERAL(Ast_idx_pair) {ast, idx+1};
    }

    assert(false);
}

void print_tokens(Token *tokens, int token_count)
{
    for (int i=0; i < token_count; i++) {
        switch(tokens[i].kind) {
            case T_Open:
                printf("OPEN\n");
                break;
            case T_Close:
                printf("CLOSE\n");
                break;
            case T_Symbol:
                printf("[SYM] %c\n", tokens[i].main.symbol);
                break;
            case T_Number:
                printf("[NUM] %lf\n", tokens[i].main.number);
                break;
            case T_Operator:
                printf("[OP]\n");
                break;
            default: assert(false);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: .\\main <input_file>\n");
        exit(1);
    }

    FILE *fn = fopen(argv[1], "r");
    
    if (fn == NULL) {
        printf("No such file: %s\n", argv[1]);
        exit(1);
    }
    InitWindow(screen_width, screen_height, "Grapher");

    char *input_buffer = (char *) malloc(2048);
    fread(input_buffer, 2048, 1, fn);
    printf("Formula: %s\n", input_buffer);

    double scale = 50;
    double step = 0.01;
    Vector2 points[SAMPLE_SIZE];
    Token tokens[TOKENS_COUNT];

    char *f = input_buffer;
    String func_code = {f, strlen(f)};
    int token_count = tokenize(tokens, func_code);
    printf("[+] Number of Tokens: %d\n", token_count);
    print_tokens(tokens, token_count);

    Arena arena = {0};
    arena.ptr = (char *) malloc(2048);
    Ast_idx_pair ast_idx_pair = parse_ast(&arena, tokens, 0);
    plot_points(ast_idx_pair.ast, points, step, scale);
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

    free(arena.ptr);
    CloseWindow();
}
