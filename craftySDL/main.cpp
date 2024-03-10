#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TILESIZE 20

bool debug = true;

void renderALL(SDL_Renderer *renderer, int tiles[][SCREEN_HEIGHT / TILESIZE][5], int BlockColors[][4]) {
    for (int i = 0; i < SCREEN_WIDTH / TILESIZE; i++) {
        for (int j = 0; j < SCREEN_HEIGHT / TILESIZE; j++) {
            SDL_SetRenderDrawColor(renderer, BlockColors[tiles[i][j][4]][0], BlockColors[tiles[i][j][4]][1], BlockColors[tiles[i][j][4]][2], BlockColors[tiles[i][j][4]][3]);
            SDL_Rect rect = {tiles[i][j][0], tiles[i][j][1], tiles[i][j][2], tiles[i][j][3]};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

void renderTile(int x, int y, SDL_Renderer *renderer, int tiles[][SCREEN_HEIGHT / TILESIZE][5], int BlockColors[][4]) {
    SDL_SetRenderDrawColor(renderer, BlockColors[tiles[x][y][4]][0], BlockColors[tiles[x][y][4]][1], BlockColors[tiles[x][y][4]][2], BlockColors[tiles[x][y][4]][3]);
    SDL_Rect rect = {tiles[x][y][0], tiles[x][y][1], tiles[x][y][2], tiles[x][y][3]};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {

    int BlockColors[5][4] = {
        {102, 255, 255, 255}, //air
        {0, 200, 0, 255}, //grass
        {80, 40, 0, 255}, //dirt
        {255, 255, 255, 255},
        {255, 60, 0, 255}
    };

    int tiles[SCREEN_WIDTH / TILESIZE][SCREEN_HEIGHT / TILESIZE][5];

    if (debug) {
        printf("Number of tiles: %zu\n", sizeof(tiles) / sizeof(int) / 5);
        printf("tiles dataSize: %zu\n", sizeof(tiles[0][0]) / sizeof(int));
    }
    int TileNb = SCREEN_HEIGHT / TILESIZE;
    for (int i = 0; i < SCREEN_WIDTH / TILESIZE; i++) {
        for (int j = 0; j < SCREEN_HEIGHT / TILESIZE; j++) {
            tiles[i][j][0] = i * TILESIZE; 
            tiles[i][j][1] = j * TILESIZE; 
            tiles[i][j][2] = TILESIZE;     
            tiles[i][j][3] = TILESIZE;     
            if (j >= TileNb - 5) {
                if (j >= TileNb - 4){
                    tiles[i][j][4] = 2; 
                }else{
                    tiles[i][j][4] = 1;
                }
            } else {
                tiles[i][j][4] = 0; 
            }
        }
    }

    SDL_Window *window = SDL_CreateWindow("Moving Square", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);


    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    // Render the scene once
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderALL(renderer, tiles, BlockColors);

    tiles[8][8][4] = 3;
    renderTile(8,8, renderer, tiles, BlockColors);
 
    bool running = true;
    while (running) 
    {

        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int MouseX, MouseY;
                SDL_GetMouseState(&MouseX, &MouseY);
                int tileX = MouseX / TILESIZE;
                int tileY = MouseY / TILESIZE;

                if (event.button.button == SDL_BUTTON_LEFT) // Check the button here
                {
                    tiles[tileX][tileY][4] = 1;
                    renderTile(tileX, tileY, renderer, tiles, BlockColors);
                }

                if (event.button.button == SDL_BUTTON_RIGHT) // Check the button here
                {
                    tiles[tileX][tileY][4] = 0;
                    renderTile(tileX, tileY, renderer, tiles, BlockColors);
                }

                if (debug)
                {
                    printf("click at x:%d y:%d on tiles[%d][%d]\n", MouseX, MouseY, tileX, tileY);
                }
            }

            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
