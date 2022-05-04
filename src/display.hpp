#pragma once

#include <unordered_map>
#include <SDL.h>

#include "common.hpp"

class Display {
    static constexpr float SCALE = 10;
    SDL_Window* window;
    SDL_Renderer* renderer;
    const SDL_Color WHITE {0xFF, 0xFF, 0xFF, 0xFF};
    const SDL_Color BLACK {0x00, 0x00, 0x00, 0xFF};
    uint32_t vram[32][64];
    const uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);

    std::unordered_map<uint8_t, SDL_Scancode> key_map {
        {0x1, SDL_SCANCODE_1}, {0x2, SDL_SCANCODE_2}, {0x3, SDL_SCANCODE_3}, {0xC, SDL_SCANCODE_4},
        {0x4, SDL_SCANCODE_Q}, {0x5, SDL_SCANCODE_W}, {0x6, SDL_SCANCODE_E}, {0xD, SDL_SCANCODE_R},
        {0x7, SDL_SCANCODE_A}, {0x8, SDL_SCANCODE_S}, {0x9, SDL_SCANCODE_D}, {0xE, SDL_SCANCODE_F},
        {0xA, SDL_SCANCODE_Z}, {0x0, SDL_SCANCODE_X}, {0xB, SDL_SCANCODE_C}, {0xF, SDL_SCANCODE_V},
    };

    void render();
    std::pair<uint32_t*, bool> write_to_vram(const uint8_t height, const uint8_t* pixel_data, const uint8_t& x_pos, const uint8_t& y_pos);

public:
    bool create_window();
    void clear();
    int handle_events();
    bool get_key(const uint8_t& key);
    bool draw_sprite(const uint8_t& height, const uint8_t* pixel_data, const uint8_t& x, const uint8_t& y);
};
