#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include "constants.h"

SDL_Rect heroSource = {0, 0, 0, 0};
SDL_Rect heroDestination = {0, 0 , 0, 0};
SDL_Rect orcSource = {0, 0, 0, 0};
SDL_Rect orcDestination = {0, 0, 0, 0};
SDL_Rect redOrcSource = {0, 0, 0, 0};
SDL_Rect redOrcDestination = {0, 0, 0, 0};


int game_is_running = TRUE;
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;

typedef struct {
    float x, y;
    float velocity;
    float life;
} Hero;

typedef struct{
    float x, y;
    float w, h; 
}Block;

typedef struct {
    float x, y;
    float velocity;
    int life;
    int orc_direction;
    int orc_is_running;
} Orc;

typedef struct {
    float x, y;
    float velocity;
    int life;
    int orc_direction;
    int orc_is_running;
} Red_Orc;

typedef struct {
    Hero hero;
    Orc orc;
    Red_Orc red_orc;
    Block block[390];
    
    SDL_Texture *background_layer_1;
    SDL_Texture *background_layer_2;
    SDL_Texture *background_layer_3;
    SDL_Texture *hero_image;
    SDL_Texture *block_image;
    SDL_Texture *orc_image;
    SDL_Texture *red_orc_image;
} Game;

TTF_Font* kill_font = NULL;
TTF_Font* life_font = NULL;


int orc_count = 0;
int kill_count = 0;

int hero_is_running = FALSE;
int hero_is_jumping = FALSE;
int hero_is_attacking = FALSE;
int hero_is_dead = FALSE;
int hero_direction = RIGHT;

SDL_Texture* renderText(const char* text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer){
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        printf("Erro ao renderizar texto: %s\n", TTF_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int map[MAP_HEIGHT][MAP_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };


int test_is_jumping(Game *game) {
    for (int i = 0; i < 100; i++) {
        float bottom_hero = game->hero.y + 112;
        if (bottom_hero >= WINDOW_HEIGHT - 24) {
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

void change_orc_spritesheet(Game *game, char *source) {
    surface = IMG_Load(source);
        if (!surface) {
            printf("Erro ao carregar a imagem %s\n", SDL_GetError());
            game_is_running = FALSE;
        }
        game->orc_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void change_red_orc_spritesheet(Game *game, char *source) {
    surface = IMG_Load(source);
        if (!surface) {
            printf("Erro ao carregar a imagem %s\n", SDL_GetError());
            game_is_running = FALSE;
        }
        game->red_orc_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void animate_orc(Game *game) {

    if (game->orc.x < game->hero.x - 32) {
        change_orc_spritesheet(game, "images/assets/Orc_Warrior/Run.png");
        game->orc.orc_is_running = TRUE;
        game->orc.orc_direction = RIGHT;
        game->orc.x += 1.5f;
    } else if (game->orc.x > game->hero.x + 32) {
        change_orc_spritesheet(game, "images/assets/Orc_Warrior/Run.png");
        game->orc.orc_is_running = TRUE;
        game->orc.orc_direction = LEFT;
        game->orc.x -= 1.5f;
    } else {
        change_orc_spritesheet(game, "images/assets/Orc_Warrior/Attack_1.png");
        game->orc.orc_is_running = FALSE;
        game->hero.life -= 0.3;
    }
    
}

void animate_red_orc(Game *game) {

    if (game->red_orc.x < game->hero.x - 32) {
        change_red_orc_spritesheet(game, "images/assets/Orc_Berserk/Run.png");
        game->red_orc.orc_is_running = TRUE;
        game->red_orc.orc_direction = RIGHT;
        game->red_orc.x += 1.5f;
    } else if (game->red_orc.x > game->hero.x + 32) {
        change_red_orc_spritesheet(game, "images/assets/Orc_Berserk/Run.png");
        game->red_orc.orc_is_running = TRUE;
        game->red_orc.orc_direction = LEFT;
        game->red_orc.x -= 1.5f;
    } else {
        change_red_orc_spritesheet(game, "images/assets/Orc_Berserk/Attack_1.png");
        game->red_orc.orc_is_running = FALSE;
        game->hero.life -= 0.3;
    }
    
}

void attack_orc(Game *game) {
    if (hero_is_attacking) {
        float distance = sqrt(pow(game->hero.x - game->orc.x, 2) + pow(game->hero.y - game->orc.y, 2));
        if (distance < 50) { 
            game->orc.life -= 1;
            if (game->orc.life <= 0) {
                printf("Orc morto!\n");
                kill_count++;
                change_orc_spritesheet(game, "images/assets/Orc_Warrior/Dead.png");
            }
        }
    }
}

void attack_red_orc(Game *game) {
    if (hero_is_attacking) {
        float distance = sqrt(pow(game->hero.x - game->red_orc.x, 2) + pow(game->hero.y - game->red_orc.y, 2));
        if (distance < 50) { 
            game->red_orc.life -= 1;
            if (game->red_orc.life <= 0) {
                printf("Orc morto!\n");
                kill_count++;
                change_red_orc_spritesheet(game, "images/assets/Orc_Berserk/Dead.png");
            }
        }
    }
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

    surface = IMG_Load("images/assets/Orc_Warrior/Idle.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->orc_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/assets/Orc_Berserk/Idle.png");
    if (!surface) {
        printf("Erro ao carregar a imagem %s\n", SDL_GetError());
        game_is_running = FALSE;
    }
    game->red_orc_image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

}


void setup() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Erro ao iniciar o SDL %s\n", SDL_GetError());
        game_is_running = FALSE;
    }

    if (TTF_Init() != 0){
        printf("Erro ao iniciar o SDL_ttf %s\n", SDL_GetError());
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

    kill_font = TTF_OpenFont("Minecraft.ttf", 50); // Use a fonte desejada e o tamanho
    if (!kill_font) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        game_is_running = FALSE;
    }

    life_font = TTF_OpenFont("Minecraft.ttf", 50); // Use a fonte desejada e o tamanho
    if (!life_font) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        game_is_running = FALSE;
    }

    heroDestination.x = heroSource.x = 0;
    heroDestination.y = heroSource.y = 0;

    orcDestination.x = orcSource.x = 0;
    orcDestination.y = orcSource.y = 0;

    redOrcDestination.x = redOrcSource.x = 0;
    redOrcDestination.y = redOrcSource.y = 0;

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
                    break;
                }
                case SDLK_a:
                {
                    hero_is_attacking = TRUE;
                    change_spritesheet(game, "images/sprites/spr_attack-export.png");
                    break;
                }
            }
            break;
        }
        case SDL_KEYUP:
        {
            switch (event.key.keysym.sym) {
                case SDLK_a:
                {
                    hero_is_attacking = FALSE;
                    change_spritesheet(game, "images/sprites/spr_idle-export.png");
                    break;
                }
            }
            break;
        }
    }
   

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_RIGHT] && game->hero.x <= WINDOW_WIDTH - 112) {
        hero_direction = RIGHT;
        hero_is_running = TRUE;
        int can_walk = TRUE;

        change_spritesheet(game, "images/sprites/spr_running-export.png");
        game->hero.x += 4.5f;
        
    } else if (state[SDL_SCANCODE_LEFT] && game->hero.x >= 0) {
        hero_direction = LEFT;
        hero_is_running = TRUE;

        change_spritesheet(game, "images/sprites/spr_running-export.png");

        game->hero.x -= 4.5f;

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
    
    if (game->hero.y >= WINDOW_HEIGHT - 132) {
        game->hero.y = WINDOW_HEIGHT - 132; 
        game->hero.velocity = 0.0f;
    }

    heroDestination.x = game->hero.x;
    heroDestination.y = game->hero.y;
    
    if (!hero_is_dead && heroSource.x <= 1800) {
        heroSource.x = 112 * (int) ((SDL_GetTicks() / 100) % 6);
    }
    
    orcDestination.x = game->orc.x;
    orcDestination.y = game->orc.y;
    orcSource.x = 96 * (int) ((SDL_GetTicks() / 100) % 4);

    redOrcDestination.x = game->red_orc.x;
    redOrcDestination.y = game->red_orc.y;
    redOrcSource.x = 96 * (int) ((SDL_GetTicks() / 100) % 4);
    
    if (game->orc.life > 0) {
        animate_orc(game);
    } else {
        change_orc_spritesheet(game, "images/assets/Orc_Warrior/Dead.png");
        if (orc_count < 5) {
            game->orc.orc_is_running = TRUE;
            game->orc.life = 100;
        } else {
           game->orc.orc_is_running = FALSE;
        }
        game->orc.x = -100;
    }
    attack_orc(game);

    if (game->red_orc.life > 0) {
        animate_red_orc(game);
    } else {
        change_red_orc_spritesheet(game, "images/assets/Orc_Berserk/Dead.png");
        if (orc_count < 5) {
            game->red_orc.orc_is_running = TRUE;
            game->red_orc.life = 100;
        } else {
            game->red_orc.orc_is_running = FALSE;
        }
        game->red_orc.x = 1300;
    }
    attack_red_orc(game);


    if (game->hero.life <= 0) {
        change_spritesheet(game, "images/sprites/spr_dying-export.png");
        hero_is_dead = TRUE;
    } 

}


void render(Game *game) {
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderCopy(renderer, game->background_layer_1, NULL, NULL);
    SDL_RenderCopy(renderer, game->background_layer_2, NULL, NULL);
    SDL_RenderCopy(renderer, game->background_layer_3, NULL, NULL);
    
    heroDestination.w = heroSource.w;
    heroDestination.h = heroSource.h;
    heroSource.w = 112;
    heroSource.h = 112;

    orcDestination.w = orcSource.w;
    orcDestination.h = orcSource.h;
    orcSource.w = 96;
    orcSource.h = 96;

    redOrcDestination.w = redOrcSource.w;
    redOrcDestination.h = redOrcSource.h;
    redOrcSource.w = 96;
    redOrcSource.h = 96;

    if (game->red_orc.orc_direction == RIGHT) {
        SDL_RenderCopy(renderer, game->red_orc_image, &redOrcSource, &redOrcDestination);
    } else {
        SDL_RenderCopyEx(renderer, game->red_orc_image, &redOrcSource, &redOrcDestination, 0, 0, SDL_FLIP_HORIZONTAL);
    }

    if (game->orc.orc_direction == RIGHT) {
        SDL_RenderCopy(renderer, game->orc_image, &orcSource, &orcDestination);
    } else {
        SDL_RenderCopyEx(renderer, game->orc_image, &orcSource, &orcDestination, 0, 0, SDL_FLIP_HORIZONTAL);
    }

    if (hero_direction == RIGHT) {
        SDL_RenderCopy(renderer, game->hero_image, &heroSource, &heroDestination);
    } else {
        SDL_RenderCopyEx(renderer, game->hero_image, &heroSource, &heroDestination, 0, 0, SDL_FLIP_HORIZONTAL);
    }

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 1) {
                SDL_Rect dstRect = {x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT};
                SDL_RenderCopy(renderer, game->block_image, NULL, &dstRect);
            }
        }
    }

    // Kill count
    SDL_Color textColor = {255, 255, 255, 255};
    char kill_text[20];
    sprintf(kill_text, "Kills: %d", kill_count);
    SDL_Texture* killTexture = renderText(kill_text, kill_font, textColor, renderer);
    if (killTexture){
        int text_width, text_height;
        SDL_QueryTexture(killTexture, NULL, NULL, &text_width, &text_height);
        SDL_Rect textRect = {10, 10, text_width, text_height};
        SDL_RenderCopy(renderer, killTexture, NULL, &textRect);
        SDL_DestroyTexture(killTexture);
    }

    char life_text[20];
    sprintf(life_text, "Life: %.0f", game->hero.life);
    SDL_Texture* lifeTexture = renderText(life_text, life_font, textColor, renderer);
    if (lifeTexture){
        int text_width, text_height;
        SDL_QueryTexture(lifeTexture, NULL, NULL, &text_width, &text_height);
        SDL_Rect textRect = {1000, 10, text_width, text_height};
        SDL_RenderCopy(renderer, lifeTexture, NULL, &textRect);
        SDL_DestroyTexture(lifeTexture);
    }


    
    SDL_RenderPresent(renderer);
}


void end(Game *game) {
    SDL_DestroyTexture(game->block_image);
    SDL_DestroyTexture(game->hero_image);
    SDL_DestroyTexture(game->orc_image);
    SDL_DestroyTexture(game->red_orc_image);
    SDL_DestroyTexture(game->background_layer_1);
    SDL_DestroyTexture(game->background_layer_2);
    SDL_DestroyTexture(game->background_layer_3);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
    setup();

    Game game;
    game.hero.x = WINDOW_WIDTH/2;
    game.hero.y = WINDOW_HEIGHT/2;
    game.hero.velocity = 0.03;
    game.hero.life = 500;

    game.orc.x = WINDOW_WIDTH - 96;
    game.orc.y = WINDOW_HEIGHT - 120;
    game.orc.life = 100;

    game.red_orc.x = 96;
    game.red_orc.y = WINDOW_HEIGHT - 120;
    game.red_orc.life = 100;


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