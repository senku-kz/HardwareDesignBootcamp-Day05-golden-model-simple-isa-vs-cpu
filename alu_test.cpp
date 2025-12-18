#include <iostream>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Valu.h"

static void dump_state(Valu* alu) {
    std::cout << "    A=0x" << std::hex << (int)alu->operand_a
              << " B=0x" << (int)alu->operand_b
              << " op=" << std::bitset<2>(alu->alu_op)
              << " => R=0x" << (int)alu->result
              << " Z=" << std::dec << (int)alu->zero_flag << "\n";
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // Create DUT and VCD trace
    Valu* alu = new Valu;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    alu->trace(tfp, 99);
    tfp->open("waveform_alu.vcd");

    uint64_t time = 0;
    auto eval_dump = [&](void) {
        alu->eval();
        tfp->dump(time++);
        dump_state(alu);
    };

    std::cout << "Testing ALU\n";
    std::cout << "==========\n\n";

    int passed = 0;

    // Test 1: ADD (simple)
    std::cout << "Test 1: ADD (5 + 7 = 12)\n";
    alu->operand_a = 5;
    alu->operand_b = 7;
    alu->alu_op = 0b00; // add
    eval_dump();
    if (alu->result == 12 && alu->zero_flag == 0) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 2: SUB (7 - 7 = 0)
    std::cout << "Test 2: SUB (7 - 7 = 0)\n";
    alu->operand_a = 7;
    alu->operand_b = 7;
    alu->alu_op = 0b01; // sub
    eval_dump();
    if (alu->result == 0 && alu->zero_flag == 1) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 3: AND (0xAA & 0x0F = 0x0A)
    std::cout << "Test 3: AND (0xAA & 0x0F = 0x0A)\n";
    alu->operand_a = 0xAA;
    alu->operand_b = 0x0F;
    alu->alu_op = 0b10; // and
    eval_dump();
    if (alu->result == 0x0A && alu->zero_flag == 0) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 4: OR (0x00 | 0x00 = 0x00)
    std::cout << "Test 4: OR (0x00 | 0x00 = 0x00)\n";
    alu->operand_a = 0x00;
    alu->operand_b = 0x00;
    alu->alu_op = 0b11; // or
    eval_dump();
    if (alu->result == 0x00 && alu->zero_flag == 1) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 5: ADD overflow (0xFF + 0x01 -> 0x00)
    std::cout << "Test 5: ADD overflow (0xFF + 0x01 -> 0x00)\n";
    alu->operand_a = 0xFF;
    alu->operand_b = 0x01;
    alu->alu_op = 0b00; // add
    eval_dump();
    if (alu->result == 0x00 && alu->zero_flag == 1) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 6: SUB underflow (0x00 - 0x01 -> 0xFF)
    std::cout << "Test 6: SUB underflow (0x00 - 0x01 -> 0xFF)\n";
    alu->operand_a = 0x00;
    alu->operand_b = 0x01;
    alu->alu_op = 0b01; // sub
    eval_dump();
    if (alu->result == 0xFF && alu->zero_flag == 0) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Test 7: Default stability (keep op AND then OR)
    std::cout << "Test 7: Operation switching stability\n";
    alu->operand_a = 0x55;
    alu->operand_b = 0x0F;
    alu->alu_op = 0b10; // and
    eval_dump();
    if (alu->result != 0x00) { /* just sanity */ } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }
    alu->alu_op = 0b11; // or
    eval_dump();
    if (alu->result == (0x55 | 0x0F)) { std::cout << "  \xE2\x9C\x93 PASS\n\n"; passed++; } else { std::cerr << "  \xE2\x9C\x97 FAIL\n"; return 1; }

    // Cleanup
    tfp->close();
    delete tfp;
    delete alu;

    std::cout << "✅ All " << passed << " tests passed!\n";
    std::cout << "VCD file: waveform_alu.vcd\n";
    return 0;
}
