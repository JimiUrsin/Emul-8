#include "../emul8.hpp"

#include <gtest/gtest.h>

Chip8 chip8{};

TEST(SmokeTests, TestInitialPC) {
    ASSERT_EQ(chip8.cpu.pc, 0x200);
}

TEST(InstructionTests, TestRET) {
    chip8.cpu.stack.push(0x123);
    chip8.execute(0x00EE);
    ASSERT_EQ(chip8.cpu.pc, 0x123);
}

TEST(InstructionTests, TestCALL) {
    chip8.execute(0x2123U);
    ASSERT_EQ(chip8.cpu.stack.top(), 0x200);
    ASSERT_EQ(chip8.cpu.pc, 0x123);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}