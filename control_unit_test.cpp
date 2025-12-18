#include <iostream>
#include <iomanip>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vcontrol_unit.h"

void print_instruction(uint8_t instr) {
    std::cout << "    Instruction: 0b" << std::bitset<8>(instr) 
              << " (0x" << std::hex << (int)instr << std::dec << ")\n";
}

int main(int argc, char** argv) {
    // Инициализация Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Создание модуля и VCD trace
    Vcontrol_unit* cu = new Vcontrol_unit;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    cu->trace(tfp, 99);
    tfp->open("waveform_cu.vcd");
    
    uint64_t time = 0;
    int test_count = 0;
    
    std::cout << "Testing Control Unit Decoder\n";
    std::cout << "============================\n\n";
    
    // Test 1: ADD-type instruction (opcode = 2'b00)
    std::cout << "Test 1: ADD-type instruction (opcode = 2'b00)\n";
    std::cout << "  Format: [opcode(2) | rd(2) | rs1(2) | rs2(2)]\n";
    
    // Instruction: 00 11 10 01 = 0b00111001 = 0x39
    // opcode=2'b00, rd=2'b11, rs1=2'b10, rs2=2'b01
    uint8_t add_instr = 0b00111001;
    cu->instruction = add_instr;
    cu->eval();
    tfp->dump(time++);
    
    print_instruction(add_instr);
    std::cout << "  Expected: opcode=0b00, rd=0b11, rs1=0b10, rs2=0b01, addr=X, imm=X\n";
    std::cout << "  Actual:   opcode=0b" << std::bitset<2>(cu->opcode)
              << ", rd=0b" << std::bitset<2>(cu->rd)
              << ", rs1=0b" << std::bitset<2>(cu->rs1)
              << ", rs2=0b" << std::bitset<2>(cu->rs2) << "\n";
    
    if (cu->opcode == 0b00 && cu->rd == 0b11 && cu->rs1 == 0b10 && cu->rs2 == 0b01) {
        std::cout << "  ✓ PASS\n\n";
        test_count++;
    } else {
        std::cerr << "  ✗ FAIL\n\n";
        return 1;
    }
    
    // Test 2: LI-type instruction (opcode = 2'b10)
    std::cout << "Test 2: LI-type instruction (opcode = 2'b10)\n";
    std::cout << "  Format: [opcode(2) | rd(2) | imm(4)]\n";
    
    // Instruction: 10 01 1111 = 0b10011111 = 0x9F
    // opcode=2'b10, rd=2'b01, imm=4'b1111
    uint8_t li_instr = 0b10011111;
    cu->instruction = li_instr;
    cu->eval();
    tfp->dump(time++);
    
    print_instruction(li_instr);
    std::cout << "  Expected: opcode=0b10, rd=0b01, imm=0b1111, rs1=X, rs2=X, addr=X\n";
    std::cout << "  Actual:   opcode=0b" << std::bitset<2>(cu->opcode)
              << ", rd=0b" << std::bitset<2>(cu->rd)
              << ", imm=0b" << std::bitset<4>(cu->imm) << "\n";
    
    if (cu->opcode == 0b10 && cu->rd == 0b01 && cu->imm == 0b1111) {
        std::cout << "  ✓ PASS\n\n";
        test_count++;
    } else {
        std::cerr << "  ✗ FAIL\n\n";
        return 1;
    }
    
    // Test 3: BNER0-type instruction (opcode = 2'b11)
    std::cout << "Test 3: BNER0-type instruction (opcode = 2'b11)\n";
    std::cout << "  Format: [opcode(2) | addr(4) | rs2(2)]\n";
    
    // Instruction: 11 0101 10 = 0b11010110 = 0xD6
    // opcode=2'b11, addr=4'b0101, rs2=2'b10
    uint8_t branch_instr = 0b11010110;
    cu->instruction = branch_instr;
    cu->eval();
    tfp->dump(time++);
    
    print_instruction(branch_instr);
    std::cout << "  Expected: opcode=0b11, addr=0b0101, rs2=0b10, rd=X, rs1=X, imm=X\n";
    std::cout << "  Actual:   opcode=0b" << std::bitset<2>(cu->opcode)
              << ", addr=0b" << std::bitset<4>(cu->addr)
              << ", rs2=0b" << std::bitset<2>(cu->rs2) << "\n";
    
    if (cu->opcode == 0b11 && cu->addr == 0b0101 && cu->rs2 == 0b10) {
        std::cout << "  ✓ PASS\n\n";
        test_count++;
    } else {
        std::cerr << "  ✗ FAIL\n\n";
        return 1;
    }
    
    // Test 4: Multiple test cases for ADD-type
    std::cout << "Test 4: Multiple ADD-type test cases\n";
    struct TestCase {
        uint8_t instr;
        uint8_t exp_opcode;
        uint8_t exp_rd;
        uint8_t exp_rs1;
        uint8_t exp_rs2;
    };
    
    TestCase add_tests[] = {
        {0b00000000, 0b00, 0b00, 0b00, 0b00},  // All zeros
        {0b00110101, 0b00, 0b11, 0b01, 0b01},  // Mixed values
        {0b00111111, 0b00, 0b11, 0b11, 0b11},  // All ones
    };
    
    for (int i = 0; i < 3; i++) {
        cu->instruction = add_tests[i].instr;
        cu->eval();
        tfp->dump(time++);
        
        if (cu->opcode == add_tests[i].exp_opcode &&
            cu->rd == add_tests[i].exp_rd &&
            cu->rs1 == add_tests[i].exp_rs1 &&
            cu->rs2 == add_tests[i].exp_rs2) {
            std::cout << "  ✓ Test case " << (i+1) << " PASS\n";
            test_count++;
        } else {
            std::cerr << "  ✗ Test case " << (i+1) << " FAIL\n";
            return 1;
        }
    }
    
    std::cout << "\n";
    
    // Test 5: Multiple test cases for LI-type
    std::cout << "Test 5: Multiple LI-type test cases\n";
    struct LITestCase {
        uint8_t instr;
        uint8_t exp_opcode;
        uint8_t exp_rd;
        uint8_t exp_imm;
    };
    
    LITestCase li_tests[] = {
        {0b10000000, 0b10, 0b00, 0b0000},  // All zeros
        {0b10101010, 0b10, 0b10, 0b1010},  // Alternating bits
        {0b10111111, 0b10, 0b11, 0b1111},  // All ones (rd and imm)
    };
    
    for (int i = 0; i < 3; i++) {
        cu->instruction = li_tests[i].instr;
        cu->eval();
        tfp->dump(time++);
        
        if (cu->opcode == li_tests[i].exp_opcode &&
            cu->rd == li_tests[i].exp_rd &&
            cu->imm == li_tests[i].exp_imm) {
            std::cout << "  ✓ Test case " << (i+1) << " PASS\n";
            test_count++;
        } else {
            std::cerr << "  ✗ Test case " << (i+1) << " FAIL\n";
            return 1;
        }
    }
    
    std::cout << "\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cu;
    
    std::cout << "✅ All " << test_count << " tests passed!\n";
    std::cout << "VCD file: waveform_cu.vcd\n";
    return 0;
}
