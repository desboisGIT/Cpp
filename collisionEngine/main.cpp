#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>


const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
bool debug = false;


int fps = 144;
float deltaTime = 1.0f / fps;


const float SCREEN_HEIGHT_M = 0.3f; 
const int SCREEN_HEIGHT_PIXELS = 1080; 

const float M_TO_PIXELS = SCREEN_HEIGHT_PIXELS / SCREEN_HEIGHT_M;

struct Ball
{
    float x;
    float y;
    float vx;
    float vy;
    float ax;
    float ay;
    float elasticity;
    int mass;
    int radius;
    int r;
    int g;
    int b;
};

void init(SDL_Window* &window, SDL_Renderer* &renderer) 
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Bouncing Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void renderBall(SDL_Renderer* renderer, Ball ball) {
    SDL_SetRenderDrawColor(renderer, ball.r, ball.g, ball.b, 255);
    for (int w = -ball.radius; w <= ball.radius; ++w) {
        for (int h = -ball.radius; h <= ball.radius; ++h) {
            if (w * w + h * h <= ball.radius * ball.radius) {
                SDL_RenderDrawPoint(renderer, ball.x + w, ball.y + h);
            }
        }
    }
}



float nextPosX;
float nextPosY;
float friction = 0.9f;
int mousePowerX = 2;
int mousePowerY = 8;


bool resolveBallOverlap(Ball &ball1, Ball &ball2, float distance) {
    if (distance <= ball1.radius + ball2.radius) {

        float overlapDistance = (ball1.radius + ball2.radius) - distance;
        float angle = std::atan2(ball2.y - ball1.y, ball2.x - ball1.x);
        float offsetX = std::cos(angle) * overlapDistance * 0.5f;
        float offsetY = std::sin(angle) * overlapDistance * 0.5f;
        ball1.x -= offsetX;
        ball1.y -= offsetY;
        ball2.x += offsetX;
        ball2.y += offsetY;
        return true; 
    }
    return false; 
}

bool checkBallCollision(Ball &ball1, Ball &ball2) {
    float distance = std::sqrt(std::pow(ball2.x - ball1.x, 2) + std::pow(ball2.y - ball1.y, 2));
    if (distance <= ball1.radius + ball2.radius) {

        if (resolveBallOverlap(ball1, ball2, distance)) {

            float collisionAngle = std::atan2(ball2.y - ball1.y, ball2.x - ball1.x);


            float v1x = ball1.vx * std::cos(collisionAngle) + ball1.vy * std::sin(collisionAngle);
            float v1y = ball1.vy * std::cos(collisionAngle) - ball1.vx * std::sin(collisionAngle);
            float v2x = ball2.vx * std::cos(collisionAngle) + ball2.vy * std::sin(collisionAngle);
            float v2y = ball2.vy * std::cos(collisionAngle) - ball2.vx * std::sin(collisionAngle);


            float temp = v1x;
            v1x = v2x;
            v2x = temp;


            ball1.vx = (v1x * std::cos(collisionAngle) - v1y * std::sin(collisionAngle)) / ball1.mass;
            ball1.vy = (v1y * std::cos(collisionAngle) + v1x * std::sin(collisionAngle)) / ball1.mass;
            ball2.vx = (v2x * std::cos(collisionAngle) - v2y * std::sin(collisionAngle)) / ball2.mass;
            ball2.vy = (v2y * std::cos(collisionAngle) + v2x * std::sin(collisionAngle)) / ball2.mass;

            return true; 
        }
    }
    return false; 
}

void checkNextFrameCollision(Ball &obj, float nextPX, float nextPY){

    if(nextPY + obj.radius >= SCREEN_HEIGHT)
    {
        float penetrationDepthY = nextPY + obj.radius - SCREEN_HEIGHT;

        obj.y -= penetrationDepthY;

        obj.vy = (-obj.vy * obj.elasticity) / obj.mass;

        obj.ay += 0;
        if (std::abs(obj.vx) < 0.01){
            obj.vx = 0;
        }else{
        obj.vx *= friction;
        }

        obj.x = nextPosX;
    }
    else
    {
        if(nextPX + obj.radius >= SCREEN_WIDTH)
        {
            float penetrationDepthX = nextPX + obj.radius - SCREEN_WIDTH;

            obj.x -= penetrationDepthX;

            obj.vx = (-obj.vx * obj.elasticity) / obj.mass;

            obj.ax = 0;
            obj.y = nextPosY; 
        }
        else
        {
            if(nextPX - obj.radius <= 0)
            {
                float penetrationDepthX = 0 - nextPX + obj.radius;
                
                obj.x += penetrationDepthX;

                obj.vx = (-obj.vx * obj.elasticity) / obj.mass;

                obj.ax = 0;
                obj.y = nextPosY; 
            }
            else
            {
                obj.x = nextPosX;
                obj.y = nextPosY; 
            } 
        }   
    }
}

void updatePosition(Ball &obj){ 
    obj.vx += obj.ax * deltaTime;
    obj.vy += obj.ay * deltaTime;
    
    nextPosX =  obj.x + obj.vx * deltaTime;
    nextPosY =  obj.y + obj.vy * deltaTime;


    checkNextFrameCollision(obj, nextPosX, nextPosY);
}

void applyGravity(Ball &obj){ 
    obj.ay = 9.81 * M_TO_PIXELS;
}

void update(std::vector<Ball> &balls) {

    for (size_t i = 0; i < balls.size(); ++i) {
        Ball &ball1 = balls[i];


        applyGravity(ball1);
        updatePosition(ball1);


        for (size_t j = i + 1; j < balls.size(); ++j) {
            Ball &ball2 = balls[j];
            checkBallCollision(ball1, ball2);
        }
    }
}




int main(int argc, char *argv[])
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    init(window, renderer);

    std::vector<Ball> balls;
    balls.push_back({100, 100, 0, 0, 0, 0, 0.4f,1, 20, 255, 0, 0});

    for(int i = 0; i<=12;i++){
    balls.push_back({600, 100, 0, 0, 0, 0, 0.4f,1, 20, 255, 255, 255});
    }




    auto start_time = std::chrono::steady_clock::now();


    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int MouseX, MouseY;
                SDL_GetMouseState(&MouseX, &MouseY);
                balls[0].vx += (MouseX - balls[0].x) * mousePowerX;
                balls[0].vy += (MouseY - balls[0].y) * mousePowerY;
            }
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);

        update(balls);
        
        for (const auto &ball : balls) {
            renderBall(renderer, ball);
        }



        SDL_RenderPresent(renderer);
        SDL_Delay(deltaTime*1000); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
