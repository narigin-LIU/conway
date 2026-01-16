#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LEN 20

// Game of Life rules
#define SURVIVE_MIN 2
#define SURVIVE_MAX 3
#define BIRTH_COUNT 3

#define SQUARE_AT(i, j) ((SDL_FRect) { \
    .x = (j) * LEN, .y = (i) * LEN, \
    .w = LEN, .h = LEN \
}) \

bool **board;
bool **next_board;
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

// Allocate a 2D boolean array as contiguous memory for better cache locality
bool **alloc_board(size_t rows, size_t cols) {
    // Check for potential overflow in multiplication
    if (cols > 0 && rows > SIZE_MAX / cols / sizeof(bool)) {
        return NULL;  // Would overflow
    }
    
    bool **arr = malloc(sizeof(bool *) * rows);
    if (!arr) return NULL;
    
    bool *data = malloc(sizeof(bool) * rows * cols);
    if (!data) {
        free(arr);
        return NULL;
    }
    
    for (size_t i = 0; i < rows; i++) {
        arr[i] = data + i * cols;
    }
    
    return arr;
}

// Free a contiguous 2D boolean array
void free_board(bool **arr) {
    if (arr) {
        free(arr[0]);  // Free the contiguous data block
        free(arr);     // Free the row pointers
    }
}

void update_board()
{
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
                next_board[i][j] = (cnt == SURVIVE_MIN || cnt == SURVIVE_MAX);
            } else {
                next_board[i][j] = (cnt == BIRTH_COUNT);
            }
        }
    }

    // Swap boards instead of allocating/freeing
    bool **temp = board;
    board = next_board;
    next_board = temp;
}

void update_window(SDL_Renderer *r)
{
    wrapper_bool(SDL_RenderClear(r));
    // Track what color we currently have set in SDL
    // false = black (dead cells), true = white (alive cells)
    bool current_color = false;  // Start with black
    wrapper_bool(SDL_SetRenderDrawColor(r, 0, 0, 0, 255));
    
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            // Only change color if cell state differs from current color
            if (board[i][j] != current_color) {
                if (board[i][j]) {
                    // Cell is alive, need white
                    wrapper_bool(SDL_SetRenderDrawColor(r, 255, 255, 255, 255));
                    current_color = true;
                } else { 
                    // Cell is dead, need black
                    wrapper_bool(SDL_SetRenderDrawColor(r, 0, 0, 0, 255));
                    current_color = false;
                }
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

    board = alloc_board(height, width);
    next_board = alloc_board(height, width);
    if (!board || !next_board) {
        fprintf(stderr, "ERROR: cannot allocate memory for boards\n");
        exit(1);
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

    free_board(board);
    free_board(next_board);

    SDL_DestroyRenderer(r);

    SDL_DestroyWindow(w);

    SDL_Quit();

    return 0;
}

