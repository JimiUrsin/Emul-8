#include "emul8.hpp"
#include "display.hpp"
#include "common.hpp"

struct CPU {
    // 0 through F
    uint8_t ram[4096] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    uint16_t pc;
    uint16_t i = 0;
    std::stack<uint16_t> stack;
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;
    uint8_t registers[16];
    const uint16_t IPS = 60;
};

class Chip8 {
    CPU cpu;

    // We could also just use a reference to the 8-bit parts but meh,
    // CPU time is not an issue
    uint16_t fetch(const uint16_t& pc) const {
        return (cpu.ram[pc] << 8) | cpu.ram[pc + 1];
    }

    public:
    Chip8() = default;
    Display display;

    void load_code(const std::string& filename) {
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

        if (!display.create_window()) {
            std::cout << "Failed to initialize the SDL window, exiting...\n";
            exit(0);
        }
        
        auto next_instruction = timer.now();
        auto instruction_delay = std::chrono::milliseconds(1000 / cpu.IPS);

        auto timer_delay = std::chrono::milliseconds(1000 / 60);
        auto next_timer = timer.now() + timer_delay;

        while(true) {
            next_instruction += instruction_delay;
            const uint16_t instruction = fetch(cpu.pc);
            cpu.pc += 2;

            std::cout << std::hex << std::setfill('0') << std::setw(4) << instruction << ": ";

            execute(instruction);

            if (next_timer < timer.now()) {
                cpu.delay_timer -= (cpu.delay_timer > 0);
                cpu.sound_timer -= (cpu.sound_timer > 0);

                next_timer += timer_delay;
            }

            if (display.handle_events())
                break;

            std::this_thread::sleep_until(next_instruction);
        }
    }

    void execute(const uint16_t& instruction) {
        switch ((instruction & 0xF000) >> 12) {
            case 0: {
                if (instruction == 0x00E0) {
                    std::cout << "Cleared screen\n";
                    display.clear();
                } else if (instruction == 0x00EE) {
                    std::cout << "Returning from subroutine\n";
                    cpu.pc = cpu.stack.top();
                    cpu.stack.pop();
                }
            } break;

            case 1: {
                const uint16_t address = instruction & 0x0FFF;
                std::cout << "Jumping to " << std::hex << address << "\n";
                cpu.pc = address;
            } break;

            case 2: {
                const uint16_t address = instruction & 0x0FFF;
                std::cout << "Subroutine call, jumping to " << std::hex << address << "\n";
                cpu.stack.push(cpu.pc);
                cpu.pc = address;
            } break;

            case 3: {
                const size_t reg = (instruction & 0x0F00) >> 8;
                if (cpu.registers[reg] == (instruction & 0x00FF)) {
                    std::cout << "Value of V" << reg << " was " << (instruction & 0x00FF) << ", skipping next instruction\n";
                    cpu.pc += 2;
                } else {
                    std::cout << "Value of V" << reg << " was not " << (instruction & 0x00FF) << ", not skipping\n";
                }
            } break;

            case 4: {
                const size_t reg = (instruction & 0x0F00) >> 8;

                if (cpu.registers[reg] != (instruction & 0x00FF)) {
                    std::cout << "Value of V" << reg << " was not " << (instruction & 0x00FF) << ", skipping next instruction\n";
                    cpu.pc += 2;
                } else {
                    std::cout << "Value of V" << reg << " was " << (instruction & 0x00FF) << ", not skipping\n";
                }
            } break;

            case 6: {
                const uint16_t reg = (instruction & 0x0F00) >> 8;
                const uint8_t value = instruction & 0x00FF;
                std::cout << "Setting register V" << reg << " to value " << (uint16_t) value << "\n";
                cpu.registers[reg] = value;
            } break;

            case 7: {
                const uint16_t reg = (instruction & 0x0F00) >> 8;
                const uint8_t value = instruction & 0x00FF;
                std::cout << "Adding " << (uint16_t) value << " to register V" << reg << "\n";
                cpu.registers[reg] += value;
            } break;

            case 0xA: {
                const uint16_t value = instruction & 0x0FFF;
                std::cout << "Setting index register to " << value << "\n";
                cpu.i = value;
            } break;

            case 0xC: {
                const size_t reg = (instruction & 0x0F00) >> 8;
                const uint8_t kk = instruction & 0x00FF;

                cpu.registers[reg] = (rand() % 256) & kk;
            } break;

            case 0xD: {
                const uint8_t x_register = (instruction & 0x0F00) >> 8;
                const uint8_t y_register = (instruction & 0x00F0) >> 4;
                const uint8_t height = instruction & 0x000F;

                const uint8_t& x = cpu.registers[x_register];
                const uint8_t& y = cpu.registers[y_register];
                std::cout << "Drawing to (" << (int) x << ", " << (int) y << ")\n";

                const bool collision = display.draw_sprite(height, &cpu.ram[cpu.i], x, y);
                if (collision)
                    cpu.registers[0xF] = 1;
            } break;

            case 0xF: {
                // Used as the target register in all commands except 0xFX29,
                // where it's an index for a character
                const size_t reg = (instruction & 0x0F00) >> 8;

                switch(instruction & 0x00FF) {
                    case 0x07: {
                        std::cout << "Setting V" << reg << " to the delay timer value\n";
                        cpu.registers[reg] = cpu.delay_timer;
                    } break;

                    case 0x15: {
                        std::cout << "Setting the delay timer to the value of V" << reg << "\n";
                        cpu.delay_timer = cpu.registers[reg];
                    } break;

                    case 0x1E: {
                        std::cout << "Adding the value of V" << reg << " to I\n";
                        cpu.i += cpu.registers[reg];
                    } break;

                    case 0x29: {
                        std::cout << "Setting I to the address of digit " << reg << "\n";
                        cpu.i = reg * 5;
                    } break;

                    case 0x33: {
                        cpu.ram[cpu.i] = cpu.registers[reg] / 100;
                        cpu.ram[cpu.i + 1] = cpu.registers[reg] % 100 / 10;
                        cpu.ram[cpu.i + 2] = cpu.registers[reg] % 10;

                        std::cout << "Writing the decimal representation of V" << reg << " to " << cpu.i << "\n";
                    } break;

                    case 0x65: {
                        std::cout << "Filling registers from memory up to V" << reg << "\n";
                        for (size_t n = 0; n <= reg; ++n) {
                            cpu.registers[n] = cpu.ram[cpu.i + n];
                        }
                    } break;

                    default:
                        std::cout << "Instruction " << std::hex << instruction << " has not been implemented yet\n";
                }
            } break;

            default:
                std::cout << "Instruction " << std::hex << instruction << " has not been implemented yet\n";
        }
    }
};

int main(int argc, char* argv[]) {
    std::string filename;
    if (argc == 2) {
        std::cout << "Loading file " << argv[1] << "...\n";
        filename.assign(argv[1]);
    } else {
        std::cout << "Enter filename: ";
        std::getline(std::cin, filename);
    }

    std::cout << "\n\n";

    Chip8 chip8;
    chip8.load_code(filename);

    chip8.run();
    return 0;
}
