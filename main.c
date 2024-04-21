#include <stdio.h>
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_image.h"
#include "constants.h"
#include <windows.h>

int game_is_running = TRUE;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;


typedef struct {
    float x, y;
    float velocity;
    int life;
} Hero;

typedef struct{
    float x, y;
    float w, h; 
}Block;

typedef struct {
    Hero hero;
    Block block[100];
    
    SDL_Texture *hero_image;
    SDL_Texture *block_image;
} Game;


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

void process_input(Game *game) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            game_is_running = FALSE;
            break;
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_RIGHT]) {
        game->hero.x += 2.5f;
    }
    if (state[SDL_SCANCODE_LEFT]) {
        game->hero.x -= 2.5f;
    }
    if (state[SDL_SCANCODE_UP]) {
        float bottom_teste = game->hero.y + 100;
        if (bottom_teste >= WINDOW_HEIGHT) {
            game->hero.velocity = 0.03;
            game->hero.y -= 50.0f;
        } 
    }
    if (state[SDL_SCANCODE_DOWN]) {
        game->hero.y += 0.1f;
    }
}



void update(Game *game) {
    float bottom_teste = game->hero.y + 100;

    if (bottom_teste >= WINDOW_HEIGHT) {
        game->hero.velocity = 0;
        game->hero.y = WINDOW_HEIGHT - 100;
    }

    game->hero.y += game->hero.velocity;
    game->hero.velocity += GRAVITY;
}

void render(Game *game) {
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_Rect rectHero = { game->hero.x, game->hero.y, 90, 100 };
    SDL_RenderCopy(renderer, game->hero_image, NULL, &rectHero);
    
    SDL_RenderPresent(renderer);
}

void end(Game *game) {
    SDL_DestroyTexture(game->hero_image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    setup();

    Game game;
    game.hero.x = WINDOW_WIDTH/2;
    game.hero.y = WINDOW_HEIGHT/2;
    game.hero.velocity = 0.03;


    surface = IMG_Load("sprites/teste.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game.hero_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    while(game_is_running) {
        int startLoop = SDL_GetTicks();

        process_input(&game);
        update(&game);
        render(&game);

        int delta = SDL_GetTicks() - startLoop;
        if (delta < DESIREDDELTA){
            SDL_Delay(DESIREDDELTA - delta);
        }
    }

    end(&game);

    return 0;
}