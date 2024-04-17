#include <stdio.h>
#include "src/include/SDL2/SDL.h"
#include "constants.h"
#include <windows.h>

int game_is_running = TRUE;
SDL_Window *window;
SDL_Renderer *renderer;

void setup() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Erro ao iniciar o SDL %s\n", SDL_GetError());
        game_is_running = FALSE;
    }

    window = SDL_CreateWindow(
        "Meneghettus Adventures",
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!window) {
        printf("Falha ao abrir a janela %s\n", SDL_GetError());
        game_is_running = FALSE;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        printf("Falha ao criar o render %s\n", SDL_GetError());
        game_is_running = FALSE;
    }

}

void process_input() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                game_is_running = FALSE;
                break;
        }
    }
}

void update() {
    //TODO
}

void render() {
    //TODO
}

void end() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    setup();

    while(game_is_running) {
        process_input();
        update();
        render();
    }

    end();

    return 0;
}