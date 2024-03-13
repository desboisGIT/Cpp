#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>

// Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Ball structure
struct Ball {
    float x, y;     // Position
    float dx, dy;   // Velocity
    int radius;     // Radius
};

// Initialize SDL and create a window
bool init(SDL_Window* &window, SDL_Renderer* &renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Bouncing Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Clear the renderer
void clearRenderer(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

// Render a ball
void renderBall(SDL_Renderer* renderer, const Ball& ball) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int w = -ball.radius; w <= ball.radius; ++w) {
        for (int h = -ball.radius; h <= ball.radius; ++h) {
            if (w * w + h * h <= ball.radius * ball.radius) {
                SDL_RenderDrawPoint(renderer, ball.x + w, ball.y + h);
            }
        }
    }
}

// Update ball position
void updateBall(Ball& ball) {
    ball.x += ball.dx;
    ball.y += ball.dy;

    // Check for collision with walls
    if (ball.x - ball.radius < 0 || ball.x + ball.radius > SCREEN_WIDTH) {
        ball.dx *= -1;
    }
    if (ball.y - ball.radius < 0 || ball.y + ball.radius > SCREEN_HEIGHT) {
        ball.dy *= -1;
    }
}

// Check for collision between two balls
bool checkCollision(const Ball& ball1, const Ball& ball2) {
    float dx = ball1.x - ball2.x;
    float dy = ball1.y - ball2.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance <= (ball1.radius + ball2.radius);
}

// Handle ball collisions
void handleCollisions(Ball& ball1, Ball& ball2) {
    if (checkCollision(ball1, ball2)) {
        // Swap velocities
        float temp_dx = ball1.dx;
        float temp_dy = ball1.dy;
        ball1.dx = ball2.dx;
        ball1.dy = ball2.dy;
        ball2.dx = temp_dx;
        ball2.dy = temp_dy;
    }
}

int main(int argc, char *argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!init(window, renderer)) {
        return 1;
    }

    Ball ball1 = {100, 100, 4, 3, 20}; // x, y, dx, dy, radius
    Ball ball2 = {400, 300, -3, -4, 20};

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        clearRenderer(renderer);
        
        updateBall(ball1);
        updateBall(ball2);

        handleCollisions(ball1, ball2);

        renderBall(renderer, ball1);
        renderBall(renderer, ball2);

        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Delay to control the frame rate
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
