#include "emul8.hpp"
#include "display.hpp"
#include "common.hpp"

struct CPU {
    uint8_t ram[4096];
    uint16_t pc;
    uint16_t i = 0;
    std::stack<uint16_t> stack;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t registers[16];
};

template<typename T>
const T get_bit(const T& number, const uint8_t& nth) {
    return (number & (1 << nth)) >> nth;
}

class Chip8 {
    CPU cpu;

    // We could also just use a reference to the 8-bit parts but meh,
    // CPU time is not an issue
    uint16_t fetch(const uint16_t& pc) const {
        return (cpu.ram[pc] << 8) | cpu.ram[pc + 1];
    }

    uint8_t* build_sprite(const uint8_t& height) {
        // Height * 8 gives us the amount of pixels, 4 bytes for all RGBA values of each pixel
        size_t size = 8 * 4 * height;
        uint8_t* pixels = new uint8_t[size];

        for(uint8_t offset = 0; offset < height; ++offset) {
            const uint8_t& bits = cpu.ram[cpu.i + offset];

            // RGBA8888 format
            for(uint8_t nth = 0; nth < 8; ++nth) {
                const uint8_t nth_bit = get_bit(bits, 7 - nth);

                const size_t mem_pos = offset * 32 + nth * 4;

                pixels[mem_pos] = 255;

                // TODO User-defined colors
                pixels[mem_pos + 1] = 255 * nth_bit;
                pixels[mem_pos + 2] = 255 * nth_bit;
                pixels[mem_pos + 3] = 255 * nth_bit;
            }
        }

        return pixels;
    }

    public:
    Chip8() = default;
    Display display;

    void load_code(const std::string filename) {
        std::ifstream input_file(filename, std::ios::binary | std::ios::ate);

        auto filesize = input_file.tellg();
        input_file.clear();
        input_file.seekg(0);

        size_t pos = 0x200u;

        input_file.read((char*) &cpu.ram[pos], filesize);
    }

    void run() {
        cpu.pc = 0x0200u;

        std::chrono::high_resolution_clock timer;

        display.create_window();

        while(true) {
            auto start = timer.now();
            uint16_t instruction = fetch(cpu.pc);
            cpu.pc += 2;

            switch ((instruction & 0xF000) >> 12) {
                case 0: {
                    if (instruction == 0x00E0) {
                        std::cout << "Cleared screen\n";
                        display.clear();
                    }
                } break;
                case 1: {
                    uint16_t address = instruction & 0x0FFF;
                    std::cout << "Jumping to " << std::hex << address << "\n";
                    cpu.pc = address;
                } break;
                case 6: {
                    const uint16_t reg = (instruction & 0x0F00) >> 8;
                    const uint8_t value = instruction & 0x00FF;
                    std::cout << "Setting register V" << reg << " to value " << (uint16_t) value << "\n";
                    cpu.registers[reg] = value;
                } break;
                case 7: {
                    uint16_t reg = (instruction & 0x0F00) >> 8;
                    uint8_t value = instruction & 0x00FF;
                    std::cout << "Adding " << (uint16_t) value << " to register V" << reg << "\n";
                    cpu.registers[reg] += value;
                } break;
                case 0xA: {
                    uint16_t value = instruction & 0x0FFF;
                    std::cout << "Setting index register to " << value << "\n";
                    cpu.i = value;
                } break;
                case 0xD: {
                    const uint8_t x_register = (instruction & 0x0F00) >> 8;
                    const uint8_t y_register = (instruction & 0x00F0) >> 4;
                    uint8_t bytes = instruction & 0x000F;

                    const uint8_t& x = cpu.registers[x_register];
                    const uint8_t& y = cpu.registers[y_register];
                    std::cout << "Drawing to (" << (int) x << ", " << (int) y << ")\n";

                    uint8_t* pixel_data = build_sprite(bytes);

                    display.draw_sprite(bytes, pixel_data, x, y);
                } break;
                default:
                    std::cout << "Instruction " << std::hex << instruction << " has not been implemented yet\n";
            }

            if (display.handle_events())
                break;

            std::cout << "\n";

            // TODO Change this into a variable calculated from a given instructions per second value
            // If processing the current instruction hasn't taken  ↓  that many milliseconds, sleep until that point
            std::this_thread::sleep_for(std::chrono::milliseconds(200) - (timer.now() - start));
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc == 2) {
        std::cout << "Loading file " << argv[1] << "...\n";
    } else {
        std::cout << "Usage: ./Emul-8(.exe) <filename>\n";
        return 0;
    }

    Chip8 chip8;
    chip8.load_code(argv[1]);

    chip8.run();
    return 0;
}
