#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int NUM_BALLS = 100;

struct Ball {
    float x, y;
    float vx, vy;
    int radius;
    SDL_Color color;
};

bool init(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Bouncing Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

SDL_Texture* createTexture(SDL_Renderer* renderer, int radius, const SDL_Color& color) {
    const int diameter = radius * 2;
    const int textureSize = diameter * diameter * 4; 
    Uint32* pixels = new Uint32[textureSize];

    for (int y = 0; y < diameter; ++y) {
        for (int x = 0; x < diameter; ++x) {
            int dx = x - radius;
            int dy = y - radius;
            float distanceSquared = dx * dx + dy * dy;
            if (distanceSquared <= radius * radius) {
                int index = y * diameter + x;
                pixels[index] = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), color.r, color.g, color.b, color.a);
            }
        }
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixels, diameter, diameter, 32, diameter * 4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    delete[] pixels;

    return texture;
}

SDL_Color randomColor() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);
    return { static_cast<Uint8>(dis(gen)), static_cast<Uint8>(dis(gen)), static_cast<Uint8>(dis(gen)), 255 };
}

void solve(std::vector<Ball>& balls) {
    for (int i = 0; i < NUM_BALLS; ++i) {
        balls[i].x += balls[i].vx;
        balls[i].y += balls[i].vy;

        if (balls[i].x - balls[i].radius < 0 || balls[i].x + balls[i].radius > SCREEN_WIDTH) {
            balls[i].vx *= -1;
        }
        if (balls[i].y - balls[i].radius < 0 || balls[i].y + balls[i].radius > SCREEN_HEIGHT) {
            balls[i].vy *= -1;
        }
    }
}

void drawBall(SDL_Renderer* renderer, SDL_Texture* texture, const Ball& ball) {
    SDL_Rect destRect = { static_cast<int>(ball.x - ball.radius), static_cast<int>(ball.y - ball.radius), ball.radius * 2, ball.radius * 2 };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}

int main() {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!init(window, renderer)) {
        return 1;
    }

    std::vector<Ball> balls(NUM_BALLS);

    for (int i = 0; i < NUM_BALLS; ++i) {
        balls[i].x = 20 + rand() % (SCREEN_WIDTH - 2 * 20);
        balls[i].y = 20 + rand() % (SCREEN_HEIGHT - 2 * 20);
        balls[i].vx = (rand() % 5) - 2;
        balls[i].vy = (rand() % 5) - 2;
        balls[i].radius = 10;
        balls[i].color = randomColor();
    }

    // C texture
    SDL_Texture* ballTexture = createTexture(renderer, 15, {255, 255, 255, 255});

    bool quit = false;
    while (!quit) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        solve(balls);

        for (int i = 0; i < NUM_BALLS; ++i) {
            drawBall(renderer, ballTexture, balls[i]);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }


    SDL_DestroyTexture(ballTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
