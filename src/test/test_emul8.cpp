#include "../emul8.hpp"

#include <gtest/gtest.h>

Chip8 chip8{};

class InstructionTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            chip8.cpu.pc = 0x200;

            for(int i = 0; i < 16; ++i) {
                chip8.cpu.registers[i] = 0;
            }
        }
};

TEST(SmokeTest, TestInitialPC) {
    ASSERT_EQ(chip8.cpu.pc, 0x200);
}

TEST_F(InstructionTest, TestRET) {
    chip8.cpu.stack.push(0x123);
    chip8.execute(0x00EE);
    ASSERT_EQ(chip8.cpu.pc, 0x123);
}

TEST_F(InstructionTest, TestJP) {
    chip8.execute(0x1123);
    ASSERT_EQ(chip8.cpu.pc, 0x123);    
}

TEST_F(InstructionTest, TestCALL) {
    chip8.execute(0x2123U);
    ASSERT_EQ(chip8.cpu.stack.top(), 0x200);
    ASSERT_EQ(chip8.cpu.pc, 0x123);
}

TEST_F(InstructionTest, TestSESkipsIfByteIsEqual) {
    chip8.cpu.registers[0] = 0x12;
    chip8.execute(0x3012);
    ASSERT_EQ(chip8.cpu.pc, 0x202);
}

TEST_F(InstructionTest, TestSEDoesntSkipIfByteIsNotEqual) {
    chip8.cpu.registers[0] = 0x12;
    chip8.execute(0x3034);
    ASSERT_EQ(chip8.cpu.pc, 0x200);
}

TEST_F(InstructionTest, TestSNEDoesntSkipIfByteIsEqual) {
    chip8.cpu.registers[0] = 0x12;
    chip8.execute(0x4012);
    ASSERT_EQ(chip8.cpu.pc, 0x200);
}

TEST_F(InstructionTest, TestSNESkipsIfByteIsNotEqual) {
    chip8.cpu.registers[0] = 0x12;
    chip8.execute(0x4034);
    ASSERT_EQ(chip8.cpu.pc, 0x202);
}

TEST_F(InstructionTest, TestSESkipsIfRegistersAreEqual) {
    chip8.cpu.registers[0] = 0x12;
    chip8.cpu.registers[1] = 0x12;
    chip8.execute(0x5010);
    ASSERT_EQ(chip8.cpu.pc, 0x202);
}

TEST_F(InstructionTest, TestLDWithByte) {
    chip8.execute(0x6A56);
    ASSERT_EQ(chip8.cpu.registers[0xA], 0x56);
}

TEST_F(InstructionTest, TestADDWithByte) {
    chip8.execute(0x7678);
    ASSERT_EQ(chip8.cpu.registers[6], 0x78);
}

TEST_F(InstructionTest, TestRegisterLD) {
    chip8.cpu.registers[0xB] = 0x73;
    chip8.execute(0x84B0);
    ASSERT_EQ(chip8.cpu.registers[4], 0x73);
}

TEST_F(InstructionTest, TestRegisterOR) {
    chip8.cpu.registers[1] = 1;
    chip8.execute(0x8011);
    ASSERT_EQ(chip8.cpu.registers[0], 1);
}

TEST_F(InstructionTest, TestRegisterAND) {
    chip8.cpu.registers[1] = 1;
    chip8.execute(0x8012);
    ASSERT_EQ(chip8.cpu.registers[0], 0);

}

TEST_F(InstructionTest, TestRegisterXOR) {
    chip8.cpu.registers[1] = 1;
    chip8.execute(0x8013);
    ASSERT_EQ(chip8.cpu.registers[0], 1);
}

TEST_F(InstructionTest, TestRegisterADD) {
    chip8.cpu.registers[1] = 3;
    chip8.execute(0x8014);
    ASSERT_EQ(chip8.cpu.registers[0], 3);
}

TEST_F(InstructionTest, TestRegisterADDCarry) {
    chip8.cpu.registers[0] = 192;
    chip8.cpu.registers[1] = 64;
    chip8.execute(0x8014);
    ASSERT_EQ(chip8.cpu.registers[0], 0);
    ASSERT_EQ(chip8.cpu.registers[0xF], 1);
}

TEST_F(InstructionTest, TestRegisterSUB) {
    chip8.cpu.registers[0] = 7;
    chip8.cpu.registers[1] = 3;
    chip8.execute(0x8015);
    ASSERT_EQ(chip8.cpu.registers[0], 4);
    ASSERT_EQ(chip8.cpu.registers[0xF], 1)  << "SUB instruction did not set borrow flag correctly";
}

TEST_F(InstructionTest, TestRegisterSUBBorrow) {
    chip8.cpu.registers[4] = 25;
    chip8.cpu.registers[9] = 30;
    chip8.execute(0x8495);
    ASSERT_EQ(chip8.cpu.registers[4], 251) << "Subtraction result was incorrect";
    ASSERT_EQ(chip8.cpu.registers[0xF], 0) << "SUB instruction did not set borrow flag correctly";
}

TEST_F(InstructionTest, TestRegisterSHR) {
    chip8.cpu.registers[7] = 64;
    chip8.execute(0x8706);
    ASSERT_EQ(chip8.cpu.registers[7], 32) << "Bit-shift right resulted in an incorrect value";
}

TEST_F(InstructionTest, TestRegisterSUBN) {
    chip8.cpu.registers[0] = 3;
    chip8.cpu.registers[1] = 7;
    chip8.execute(0x8017);
    ASSERT_EQ(chip8.cpu.registers[0], 4);
    ASSERT_EQ(chip8.cpu.registers[0xF], 1)  << "SUBN instruction did not set the borrow flag correctly";
}

TEST_F(InstructionTest, TestRegisterSUBNBorrow) {
    chip8.cpu.registers[4] = 30;
    chip8.cpu.registers[9] = 25;
    chip8.execute(0x8497);
    ASSERT_EQ(chip8.cpu.registers[4], 251) << "SUBN Subtraction result was incorrect";
    ASSERT_EQ(chip8.cpu.registers[0xF], 0) << "SUBN instruction did not set the borrow flag correctly";
}

TEST_F(InstructionTest, TestRegisterSHL) {
    chip8.cpu.registers[5] = 14;
    chip8.execute(0x850E);
    ASSERT_EQ(chip8.cpu.registers[5], 28) << "SHL gave out an incorrect result";
}

TEST_F(InstructionTest, TestRegisterSHLSetsCarry) {
    chip8.cpu.registers[0xC] = 0b1000'0000;
    chip8.execute(0x8C0E);
    ASSERT_EQ(chip8.cpu.registers[0xC], 0);
    ASSERT_EQ(chip8.cpu.registers[0xF], 1) << "SHL did not set carry flag on overflow";
}

TEST_F(InstructionTest, TestRegisterSNESkipsIfNotEqual) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestRegisterLDI) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestRegisterJP) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestRND) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestSKP) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestSKNP) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestRegisterLDDT) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestLDK) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestLDDT) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestLDST) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestADDI) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TESTLDI) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestLDBCD) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestStoreRegisters) {
    GTEST_SKIP_("Not implemented yet");
}

TEST_F(InstructionTest, TestReadRegisters) {
    GTEST_SKIP_("Not implemented yet");
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
