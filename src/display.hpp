#pragma once

#include "common.hpp"
#include <SDL.h>

class Display {
    static constexpr float scale = 10;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_Texture*> textures;
    const SDL_Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
    const SDL_Color BLACK {0x00, 0x00, 0x00, 0xFF};

    void render() {
        SDL_RenderPresent(renderer);
    }

    public:
    bool create_window() {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0)
            return false;

        window = SDL_CreateWindow("Emul-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_ALWAYS_ON_TOP);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderSetScale(renderer, scale, scale);
        render();

        return true;
    }

    void clear() {
        SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    }

    int handle_events() {
        SDL_Event e;

        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                SDL_Quit();
                return 1;
            }
        }
        return 0;
    }

    void draw_sprite(const uint8_t height, const uint8_t* pixel_data, const uint8_t x, const uint8_t y) {
        SDL_Rect* sprite_rect = new SDL_Rect{x, y, 8, height};
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 8, height);
        SDL_UpdateTexture(texture, NULL, pixel_data, 4 * 8);
        SDL_RenderCopy(renderer, texture, NULL, sprite_rect);

        // Clean up
        delete[] pixel_data;
        delete sprite_rect;
        SDL_DestroyTexture(texture);

        render();
    }
};
