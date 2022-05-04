#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>

#include "display.hpp"
#include "common.hpp"
#include "cpu.hpp"

class Chip8 {
    uint16_t fetch(const uint16_t& pc) const;

public:
    CPU cpu;
    Chip8() = default;
    Display display;
    void load_code(const std::string& filename);
    void run();
    void execute(const uint16_t& instruction);
};
