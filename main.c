/*  MIT License
 *  Copyright (c) 2025 Skyrix
 *  
 *  To compile this program on windows with msys2, please use the following command:  
 *  gcc main.c -o waveplt -lmingw32 -lSDL2main -lSDL2 -lm
 *  
 */

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define A 100.0
#define N_TERMS_SCALE 1
#define OMEGA0 2.0
#define POINTS 500*SCALE_X

#define SCALE_STEP 2

double SCALE_X = 2.0;
double N_TERMS = 2.0;

int screen_width = 800;
int screen_height =  600;

void draw_axes(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray color

    SDL_RenderDrawLine(renderer, 0, screen_height / 2, screen_width, screen_height / 2);
    SDL_RenderDrawLine(renderer, screen_width / 2, 0, screen_width / 2, screen_height);
}

void draw_period_lines(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 31, 31, 31, 255);  // Set color for thin gray lines
    
    double T = (2 * M_PI) / OMEGA0;
    double T_pixels = (T * screen_width) / (2 * M_PI * SCALE_X);

    int center_x = screen_width / 2;

    // Draw vertical lines
    for (int i = -SCALE_X; i <= SCALE_X; i++) {  // Draw multiple periods
        int x_pos = center_x + (int)(i * T_pixels);
        if (x_pos >= 0 && x_pos < screen_width) {
            SDL_RenderDrawLine(renderer, x_pos, 0, x_pos, screen_height);
        }
    }
}


void draw_wave(SDL_Renderer *renderer) {
    for (int i = 0; i < POINTS; i++) {
        double x = (double)i / (POINTS - 1) * (2 * M_PI / OMEGA0) * SCALE_X;
        double sum = 0.0;

        for (int n = 1; n <= N_TERMS; n++) {
            //sum += sin(OMEGA0 * x);
            //sum += cos(n * OMEGA0 * x) / (4.0 * n * n - 1.0);                     // Wave-like wave (sea) (careful n=1)
            //sum += sin((2.0*n + 1) * OMEGA0 * x) / (2.0 * n + 1.0);               // Square wave (careful n=0)
            sum += (sin(n * (0.25) * M_PI)/(n * 0.25 * M_PI))*cos(n * OMEGA0 * x);  // Scalable square wave (pwm) (careful n=1)
        }
        
        //double y = A * sum;                                     // Simple sine wave
        //double y = (2 * A / M_PI) - (4 * A / M_PI) * sum;     // Wave-like wave (sea)
        //double y = (4 * A / M_PI) * sum;                      // Square wave
        double y = (0.25*2.0*A) + (2.0*0.25*2.0*A) * sum;     // Scalable square wave (pwm)
        y = screen_height / 2 - y;
        
        SDL_RenderDrawPoint(renderer, (int)(x * screen_width / (2 * M_PI / OMEGA0))/SCALE_X, (int)y);
    }
}

void handle_input(int *running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = 0;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_RIGHT:  // Increase SCALE_X
                    SCALE_X += SCALE_STEP;
                    break;
                case SDLK_LEFT:   // Decrease SCALE_X
                    if (SCALE_X > SCALE_STEP)  // Prevent negative or zero scale
                        SCALE_X -= SCALE_STEP;
                    break;
                case SDLK_DOWN:
                    if (N_TERMS >= 0)  // Prevent negative or zero scale
                        N_TERMS -= N_TERMS_SCALE;
                    break;
                case SDLK_UP:
                    N_TERMS += N_TERMS_SCALE;
                    break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Fourier Plt", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_MaximizeWindow(window);
    SDL_GetWindowSize(window, &screen_width, &screen_height);
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);


    int running = 1;
    SDL_Event event;
    while (running) {
        handle_input(&running);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        draw_axes(renderer);
        draw_period_lines(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        draw_wave(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
