#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define LEN 20

#define SQUARE_AT(i, j) ((SDL_FRect) { \
    .x = (j) * LEN, .y = (i) * LEN, \
    .w = LEN, .h = LEN \
}) \

bool **board;
size_t width, height;

void wrapper_bool(bool flag) {
    if (!flag) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(0);
    }
}

const void *wrapper_pointer(const void *ptr) {
    if (!ptr) {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(0);
    }
    return ptr;
}

void update_board()
{
    bool **next_board = malloc(sizeof(bool *) * height);
    for (size_t i = 0; i < height; i++) {
        next_board[i] = malloc(sizeof(bool) * width);
    }

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            next_board[i][j] = false;
        }
    }

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            int cnt = 0;
            if (i > 0 && j > 0 && board[i - 1][j - 1])  {
                cnt++;
            }
            if (i > 0 && board[i - 1][j]) {
                cnt++;
            }
            if (i > 0 && j < width - 1 && board[i - 1][j + 1]) {
                cnt++;
            }
            if (j > 0 && board[i][j - 1]) {
                cnt++;
            }
            if (j < width - 1 && board[i][j + 1]) {
                cnt++;
            }
            if (i < height - 1 && j > 0 && board[i + 1][j - 1]) {
                cnt++;
            }
            if (i < height - 1 && board[i + 1][j]) {
                cnt++;
            }
            if (i < height - 1 && j < width - 1 && board[i + 1][j + 1]) {
                cnt++;
            }

            if (board[i][j]) {
                if (cnt == 2 || cnt == 3) {
                    next_board[i][j] = true;
                } else {
                    next_board[i][j] = false;
                }
            } else if (cnt == 3) {
                next_board[i][j] = true;
            } else {
                next_board[i][j] = false;
            }
        }
    }

    for (size_t i = 0; i < height; i++) {
        free(board[i]);
    }
    free(board);

    board = next_board;
}

void update_window(SDL_Renderer *r)
{
    wrapper_bool(SDL_RenderClear(r));
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            if (board[i][j]) {
                wrapper_bool(SDL_SetRenderDrawColor(r, 255, 255, 255, 255));
            } else { 
                wrapper_bool(SDL_SetRenderDrawColor(r, 0, 0, 0, 255));
            }
            wrapper_bool(SDL_RenderFillRect(r, &SQUARE_AT(i, j)));
        }
    }
    wrapper_bool(SDL_RenderPresent(r));
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        printf("Usage: conway [FILE]\n");
        printf("Start game with initial map FILE.\n");
        return 0;
    }

    if (argc > 2) {
        printf("Too many arguments.\n");
        return 0;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "ERROR: cannot access input file\n");
        exit(0);
    }
    
    if (fscanf(fp, "%zu%zu", &height, &width) != 2) {
        fprintf(stderr, "ERROR: cannot read the sizes of the map.\n");
    }

    board = malloc(sizeof(bool *) * height);
    for (size_t i = 0; i < height; i++) {
        board[i] = malloc(sizeof(bool) * width);
    }

    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            int input = 0;
            fscanf(fp, "%d", &input);
            board[i][j] = (input == 1) ? true : false;
        }
    }

    wrapper_bool(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *w = (SDL_Window *) wrapper_pointer(SDL_CreateWindow("conway",
            width * LEN, height * LEN, 0));

    SDL_Renderer *r = (SDL_Renderer *) wrapper_pointer(SDL_CreateRenderer(w, NULL));

    wrapper_bool(SDL_ShowWindow(w));

    wrapper_bool(SDL_RenderPresent(r));

    bool game_running = true;

    while (game_running) {
        SDL_PumpEvents();

        const bool *keyboard = (const bool *) wrapper_pointer(SDL_GetKeyboardState(NULL));
        if (keyboard[SDL_SCANCODE_Q]) {
            game_running = false;
            continue;
        }

        update_window(r);

        update_board();

        SDL_Delay(100);
    }

    for (size_t i = 0; i < height; i++) {
        free(board[i]);
    }
    free(board);

    SDL_DestroyRenderer(r);

    SDL_DestroyWindow(w);

    SDL_Quit();

    return 0;
}

