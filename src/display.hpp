#pragma once

#include "common.hpp"
#include <SDL.h>

class Display {
    static constexpr float SCALE = 10;
    SDL_Window* window;
    SDL_Renderer* renderer;
    const SDL_Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
    const SDL_Color BLACK {0x00, 0x00, 0x00, 0xFF};

    uint32_t vram[32][64];

    void render() {
        SDL_RenderPresent(renderer);
    }

    std::pair<uint32_t*, bool> write_to_vram(const uint8_t height, const uint8_t* pixel_data, const uint8_t& x_pos, const uint8_t& y_pos) {
        uint32_t* color_data = new uint32_t[height*8];
        uint32_t changes = 0;

        for(size_t y = 0; y < height; ++y) {
            for(size_t x = 0; x < 8; ++x) {
                uint32_t new_value = 0xFFFFFF00 * get_bit(pixel_data[y], 7 - x);
                new_value |= vram[y_pos + y][x_pos + x];
                color_data[y*8 + x] = new_value;

                // Collision happened if this bit's color data was set to 0
                changes += new_value == 0xFF;
            }
        }

        return std::make_pair(color_data, changes > 0);
    }

    public:
    bool create_window() {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0)
            return false;

        window = SDL_CreateWindow("Emul-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_ALWAYS_ON_TOP);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderSetScale(renderer, SCALE, SCALE);
        render();

        // Initialize VRAM
        for(size_t y = 0; y < 32; ++y) {
            for(size_t x = 0; x < 64; ++x) {
                vram[y][x] = 0xFF;
            }
        }

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

    bool draw_sprite(const uint8_t& height, const uint8_t* pixel_data, const uint8_t& x, const uint8_t& y) {
        SDL_Rect* sprite_rect = new SDL_Rect{x, y, 8, height};
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, 8, height);
        auto colordata_collision = write_to_vram(height, pixel_data, x, y);

        SDL_UpdateTexture(texture, NULL, colordata_collision.first, 4 * 8);
        SDL_RenderCopy(renderer, texture, NULL, sprite_rect);

        // Clean up
        delete[] pixel_data;
        delete sprite_rect;
        SDL_DestroyTexture(texture);

        render();
        return colordata_collision.second;
    }
};
