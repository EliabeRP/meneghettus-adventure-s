#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <windows.h>
#include "constants.h"

SDL_Rect heroSource = {0, 0, 0, 0};
SDL_Rect heroDestination = {0, 0 , 0, 0};

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
    
    SDL_Texture *background_layer_1;
    SDL_Texture *background_layer_2;
    SDL_Texture *background_layer_3;
    SDL_Texture *hero_image;
    //SDL_Texture *hero_running;
    SDL_Texture *block_image;
} Game;


int hero_is_running = FALSE;
int hero_is_jumping = FALSE;
int position = RIGHT;

int test_is_jumping(Game *game) {
    for (int i = 0; i < 100; i++) {
        float bottom_hero = game->hero.y + 112;
        if (bottom_hero >= game->block[i].y && game->hero.y <= game->block[i].y + game->block[i].h && game->hero.x + 90 >= game->block[i].x && game->hero.x <= game->block[i].x + game->block[i].w) {
            return TRUE;
            break;
        }
    }
    return FALSE;         
}

void change_spritesheet(Game *game, char *source) {
    surface = IMG_Load(source);
        if (!surface) {
            printf("Erro ao carregar a imagem %s\n", SDL_GetError());
            game_is_running = FALSE;
        }
        game->hero_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void loadTextures(Game *game) {
    surface = IMG_Load("images/background_layer_1.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->background_layer_1 = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/background_layer_2.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->background_layer_2 = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/background_layer_3.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->background_layer_3 = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/sprites/spr_idle-export.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->hero_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);


    surface = IMG_Load("images/assets/oak-wood-17.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->block_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}


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

    heroDestination.x = heroSource.x = 0;
    heroDestination.y = heroSource.y = 0;
}

void process_input(Game *game) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            game_is_running = FALSE;
            break;
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                {
                    if (test_is_jumping(game)) {
                        game->hero.velocity = -3.0f;
                        game->hero.y -= 50.0f;
                    }
                    
                    hero_is_jumping = TRUE;

                    change_spritesheet(game, "images/sprites/spr_jumping-export.png");
                    
                }
            }
        }
    }
   

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_RIGHT]) {
        position = RIGHT;
        hero_is_running = TRUE;

        change_spritesheet(game, "images/sprites/spr_running-export.png");

        game->hero.x += 2.5f;
    } else if (state[SDL_SCANCODE_LEFT]) {
        position = LEFT;
        hero_is_running = TRUE;

        change_spritesheet(game, "images/sprites/spr_running-export.png");

        game->hero.x -= 2.5f;

    } else if (hero_is_jumping && test_is_jumping(game)) {

        change_spritesheet(game, "images/sprites/spr_idle-export.png");

        hero_is_jumping = FALSE;
    } else if (hero_is_running) {
        change_spritesheet(game, "images/sprites/spr_idle-export.png");
        
        hero_is_running = FALSE;
    }


    if (state[SDL_SCANCODE_DOWN]) {
        game->hero.y += 0.1f;
    }

}

void update(Game *game) {
    game->hero.y += game->hero.velocity;
    game->hero.velocity += GRAVITY;
    
    if (game->hero.y >= WINDOW_HEIGHT - 156) {
        game->hero.y = WINDOW_HEIGHT - 156; 
        game->hero.velocity = 0.0f;
    }

    heroDestination.x = game->hero.x;
    heroDestination.y = game->hero.y;
    heroSource.x = 112 * (int) ((SDL_GetTicks() / 100) % 6);
}

void render(Game *game) {
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderCopy(renderer, game->background_layer_1, NULL, NULL);
    SDL_RenderCopy(renderer, game->background_layer_2, NULL, NULL);
    SDL_RenderCopy(renderer, game->background_layer_3, NULL, NULL);
    

    //SDL_Rect rectHero = { game->hero.x-45, game->hero.y-50, 90, 100 };
    //SDL_RenderCopy(renderer, game->hero_image, NULL, &rectHero);
    heroDestination.w = heroSource.w;
    heroDestination.h = heroSource.h;
    heroSource.w = 112;
    heroSource.h = 112;

    if (position == RIGHT) {
        SDL_RenderCopy(renderer, game->hero_image, &heroSource, &heroDestination);
    } else {
        SDL_RenderCopyEx(renderer, game->hero_image, &heroSource, &heroDestination, 0, 0, SDL_FLIP_HORIZONTAL);
    }

    for (int i = 0; i < 100; i++) {
        game->block[i].w = 48;
        game->block[i].h = 48;
        game->block[i].x = i*game->block[i].w;
        game->block[i].y = WINDOW_HEIGHT - game->block[i].h;

        SDL_Rect rectBlock = { game->block[i].x, game->block[i].y, 50, 60 };
        SDL_RenderCopy(renderer, game->block_image, NULL, &rectBlock);
    }
    
    SDL_RenderPresent(renderer);
}

void end(Game *game) {
    SDL_DestroyTexture(game->block_image);
    SDL_DestroyTexture(game->hero_image);
    SDL_DestroyTexture(game->background_layer_1);
    SDL_DestroyTexture(game->background_layer_2);
    SDL_DestroyTexture(game->background_layer_3);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    setup();

    Game game;
    game.hero.x = WINDOW_WIDTH/2;
    game.hero.y = WINDOW_HEIGHT/2;
    game.hero.velocity = 0.03;

    loadTextures(&game);

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