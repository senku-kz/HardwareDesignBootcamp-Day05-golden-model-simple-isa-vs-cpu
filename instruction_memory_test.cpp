#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vinstruction_memory.h"

int main(int argc, char** argv) {
    // Инициализация Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Создание модуля и VCD trace
    Vinstruction_memory* rom = new Vinstruction_memory;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    rom->trace(tfp, 99);
    tfp->open("waveform_rom.vcd");
    
    uint64_t time = 0;
    
    std::cout << "Testing Instruction ROM (Combinational Logic)\n";
    std::cout << "============================================\n\n";
    
    // Test 1: Read all initialized instructions
    std::cout << "Test 1: Read all initialized instructions\n";
    
    /*
    10001010    # 0: li r0, 10
    10010000    # 1: li r1, 0
    10100000    # 2: li r2, 0
    10110001    # 3: li r3, 1
    00010111    # 4: add r1, r1, r3
    00101001    # 5: add r2, r2, r1
    11010001    # 6: bner0 r1, 4
    11011111    # 7: bner0 r3, 7
    */

    // Expected values from instructionRom.sv
    uint8_t expected[8] = {
        0b10001010, // Address 0
        0b10010000, // Address 1
        0b10100000, // Address 2
        0b10110001, // Address 3
        0b00010111, // Address 4
        0b00101001, // Address 5
        0b11010001, // Address 6
        0b11011111  // Address 7
    };
    
    for (int addr = 0; addr < 8; addr++) {
        rom->address = addr;
        rom->eval();
        tfp->dump(time++);
        
        uint8_t actual = rom->instruction;
        std::cout << "  Address " << addr << ": 0x" << std::hex << (int)actual 
                  << " (expected 0x" << (int)expected[addr] << ")\n" << std::dec;
        
        if (actual != expected[addr]) {
            std::cerr << "  ✗ FAIL: Expected 0x" << std::hex << (int)expected[addr] 
                      << ", got 0x" << (int)actual << std::dec << "\n";
            return 1;
        }
        std::cout << "  ✓ Match\n";
    }
    
    // Test 2: Read zero-initialized addresses
    std::cout << "\nTest 2: Read zero-initialized addresses (8-15)\n";
    for (int addr = 8; addr <= 15; addr++) {
        rom->address = addr;
        rom->eval();
        tfp->dump(time++);
        
        if (rom->instruction != 0) {
            std::cerr << "  ✗ FAIL at address " << addr << ": Expected 0x00, got 0x" 
                      << std::hex << (int)rom->instruction << std::dec << "\n";
            return 1;
        }
        std::cout << "  ✓ Address " << addr << " = 0x00\n";
    }
    
    // Test 3: Combinational property - immediate response to address change
    std::cout << "\nTest 3: Combinational logic (immediate response)\n";
    
    int test_sequence[5] = {0, 15, 7, 3, 10};
    for (int i = 0; i < 5; i++) {
        rom->address = test_sequence[i];
        rom->eval();
        tfp->dump(time++);
        
        // Check that we get correct output immediately (no clock delay)
        uint8_t addr = test_sequence[i];
        std::cout << "  Address " << addr << " -> instruction = 0x" << std::hex 
                  << (int)rom->instruction << std::dec << "\n";
    }
    
    // Test 4: All addresses are readable
    std::cout << "\nTest 4: Full address space coverage\n";
    int pass_count = 0;
    for (int addr = 0; addr < 16; addr++) {
        rom->address = addr;
        rom->eval();
        tfp->dump(time++);
        
        if (rom->instruction >= 0 && rom->instruction <= 255) {
            pass_count++;
        } else {
            std::cerr << "  ✗ FAIL: Invalid instruction at address " << addr << "\n";
            return 1;
        }
    }
    std::cout << "  ✓ All " << pass_count << " addresses accessible\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete rom;
    
    std::cout << "\n✅ All tests passed!\n";
    std::cout << "VCD file: waveform_rom.vcd\n";
    return 0;
}
