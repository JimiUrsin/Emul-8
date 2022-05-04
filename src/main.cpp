#include <iostream>

#include "emul8.hpp"

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
