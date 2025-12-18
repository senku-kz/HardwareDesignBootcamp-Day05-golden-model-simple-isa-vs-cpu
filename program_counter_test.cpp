#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vprogram_counter.h"

int main(int argc, char** argv) {
    // Инициализация Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Создание модуля и VCD trace
    Vprogram_counter* pc = new Vprogram_counter;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    pc->trace(tfp, 99);
    tfp->open("waveform_pc.vcd");
    
    uint64_t time = 0;
    
    // Initialize signals
    pc->opcode = 0b00;    // Normal increment mode
    pc->set_value = 0;
    
    // Test 1: Reset
    std::cout << "Test 1: Reset PC to 0\n";
    pc->reset = 1;
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
    
    if (pc->pc_out != 0) {
        std::cerr << "FAIL: Reset did not set pc_out to 0 (got " << (int)pc->pc_out << ")\n";
        return 1;
    }
    std::cout << "  ✓ pc_out = 0\n";
    
    // Test 2: Increment from 0 to 1
    std::cout << "\nTest 2: Increment PC\n";
    pc->reset = 0;
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
    
    if (pc->pc_out != 1) {
        std::cerr << "FAIL: First increment failed (expected 1, got " << (int)pc->pc_out << ")\n";
        return 1;
    }
    std::cout << "  ✓ pc_out = 1\n";
    
    // Test 3: Multiple increments (1->2->3->4->5)
    std::cout << "\nTest 3: Multiple increments\n";
    for (int i = 2; i <= 5; i++) {
        pc->clk = 0;
        pc->eval();
        tfp->dump(time++);
        
        pc->clk = 1;
        pc->eval();
        tfp->dump(time++);
        
        if (pc->pc_out != i) {
            std::cerr << "FAIL: Expected " << i << ", got " << (int)pc->pc_out << "\n";
            return 1;
        }
        std::cout << "  ✓ pc_out = " << i << "\n";
    }
    
    // Test 4: Counter overflow (15->0)
    std::cout << "\nTest 4: Counter overflow (15->0)\n";
    pc->reset = 1;
    for (int i = 0; i < 2; i++) {
        pc->clk = !pc->clk;
        pc->eval();
        tfp->dump(time++);
    }
    pc->reset = 0;
    
    // Increment to 15
    for (int i = 0; i < 15; i++) {
        pc->clk = 0;
        pc->eval();
        tfp->dump(time++);
        pc->clk = 1;
        pc->eval();
        tfp->dump(time++);
    }
    
    // Check overflow: 15 + 1 = 0 (4-bit overflow)
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);

    if (pc->pc_out != 0) {
        std::cerr << "FAIL: Overflow failed (expected 0, got " << (int)pc->pc_out << ")\n";
        return 1;
    }
    std::cout << "  ✓ pc_out = 0 (overflow correct)\n";
    
    // Test 5: Branch instruction (set PC to specific value)
    std::cout << "\nTest 5: Branch to address 9\n";
    pc->opcode = 0b11;      // Branch opcode
    pc->set_value = 0b1001;      // Target address
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
    
    if (pc->pc_out != 9) {
        std::cerr << "FAIL: Branch failed (expected 9, got " << (int)pc->pc_out << ")\n";
        return 1;
    }
    std::cout << "  ✓ pc_out = 9 (branch successful)\n";
    
    // Test 6: Return to normal increment after branch
    std::cout << "\nTest 6: Resume increment after branch\n";
    pc->opcode = 0b00;      // Normal mode
    pc->clk = 0;
    pc->eval();
    tfp->dump(time++);
    
    pc->clk = 1;
    pc->eval();
    tfp->dump(time++);
    
    if (pc->pc_out != 10) {
        std::cerr << "FAIL: Increment after branch failed (expected 10, got " << (int)pc->pc_out << ")\n";
        return 1;
    }
    std::cout << "  ✓ pc_out = 10 (increment resumed)\n";

    
    // Cleanup
    tfp->close();
    delete tfp;
    delete pc;
    
    std::cout << "\n✅ All tests passed!\n";
    std::cout << "VCD file: waveform.vcd\n";
    return 0;
}