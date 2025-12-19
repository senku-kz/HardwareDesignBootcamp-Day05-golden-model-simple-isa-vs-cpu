#include <iostream>
#include <iomanip>
#include <vector>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vmain.h"
#include "sCPU.h"

int clock_cycles = 40;

void clock_cycle(Vmain* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    cpu->clk = 0;
    cpu->eval();
    tfp->dump(time++);
    
    cpu->clk = 1;
    cpu->eval();
    tfp->dump(time++);
}

bool compare_cpus(Vmain* designed_cpu, sCPU* golden_cpu, int cycle) {
    bool match = true;
    
    // Compare PC (4-bit value stored in uint8_t)
    uint8_t designed_pc = designed_cpu->pc_debug;
    uint8_t golden_pc = golden_cpu->getPc();
    
    if (designed_pc != golden_pc) {
        std::cout << "  err Cycle " << std::setw(3) << cycle << ": PC mismatch - Designed CPU: " 
                  << std::setw(3) << (int)designed_pc << ", Golden CPU: " << std::setw(3) << (int)golden_pc << "\n";
        match = false;
    }
    
    // Compare all registers
    for (int i = 0; i < 4; i++) {
        uint8_t designed_reg = 0;
        uint8_t golden_reg = golden_cpu->getRegister(i);
        
        // Get register value from hardware CPU
        switch(i) {
            case 0: designed_reg = designed_cpu->reg0_debug; break;
            case 1: designed_reg = designed_cpu->reg1_debug; break;
            case 2: designed_reg = designed_cpu->reg2_debug; break;
            case 3: designed_reg = designed_cpu->reg3_debug; break;
        }
                
        if (designed_reg != golden_reg) {
            std::cout << "  err Cycle " << std::setw(3) << cycle << ": R" << i << " mismatch - Designed CPU: " 
                      << std::setw(3) << (int)designed_reg << ", Golden CPU: " << std::setw(3) << (int)golden_reg << "\n";
            match = false;
        }
    }
    
    return match;
}

void print_state(Vmain* designed_cpu, sCPU* golden_cpu, int cycle) {
    std::cout << "Cycle " << std::setw(3) << cycle << ":\n";
    std::cout << "  PC:\t\tDesigned CPU: " << std::setw(3) << (int)designed_cpu->pc_debug 
              << "\tGolden CPU: " << std::setw(3) << (int)golden_cpu->getPc() << "\n";
    std::cout << "  Registers:\n";
    for (int i = 0; i < 4; i++) {
        uint8_t designed_reg = 0;
        uint8_t golden_reg = golden_cpu->getRegister(i);
        
        // Get register value from hardware CPU
        switch(i) {
            case 0: designed_reg = designed_cpu->reg0_debug; break;
            case 1: designed_reg = designed_cpu->reg1_debug; break;
            case 2: designed_reg = designed_cpu->reg2_debug; break;
            case 3: designed_reg = designed_cpu->reg3_debug; break;
        }

        std::cout << "    R" << i << ":\t\tDesigned CPU: " << std::setw(3) << (int)designed_reg 
                  << "\tGolden CPU: " << std::setw(3) << (int)golden_reg;
        if (designed_reg == golden_reg) {
            std::cout << "\tok";
        } else {
            std::cout << "\terr";
        }
        std::cout << "\n";
    }
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create hardware CPU and VCD trace
    Vmain* designed_cpu = new Vmain;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    designed_cpu->trace(tfp, 99);
    tfp->open("waveform_cpu.vcd");
    
    // Create golden CPU
    sCPU* golden_cpu = new sCPU;
    
    // Load instructions into reference CPU (same as in instruction_memory.sv)
    std::vector<uint8_t> instructions = {
        0b10001010,  // 0: li r0, 10
        0b10010000,  // 1: li r1, 0
        0b10100000,  // 2: li r2, 0
        0b10110001,  // 3: li r3, 1
        0b00010111,  // 4: add r1, r1, r3
        0b00101001,  // 5: add r2, r2, r1
        0b11010001,  // 6: bner0 r1, 4
        0b11011111,  // 7: bner0 r3, 7
        0b00000000,  // 8-15: NOP
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000
    };
    golden_cpu->loadInstructions(instructions);
    
    uint64_t time = 0;
    bool all_match = true;
    
    std::cout << "Testing Simple ISA CPU (Designed CPU vs Golden CPU)\n";
    std::cout << "===================================================\n\n";
    
    // Reset both CPUs
    std::cout << "Resetting CPUs...\n";
    designed_cpu->reset = 1;
    clock_cycle(designed_cpu, tfp, time);
    clock_cycle(designed_cpu, tfp, time);
    designed_cpu->reset = 0;

    golden_cpu->setPc(0);
    std::cout << "ok Reset complete\n\n";
    
    // Run for clock_cycles clock cycles to execute instructions
    std::cout << "Running CPUs for " << clock_cycles << " cycles with comparison...\n\n";
    for (int cycle = 0; cycle < clock_cycles; cycle++) {
        // First, verify both CPUs are at the same PC before executing
        uint8_t designed_pc_before = designed_cpu->pc_debug;
        uint8_t golden_pc_before = golden_cpu->getPc();
        
        if (designed_pc_before != golden_pc_before) {
            std::cout << "  ⚠ Cycle " << std::setw(3) << cycle << ": PC desynchronized before execution - Designed CPU: " 
                      << std::setw(3) << (int)designed_pc_before << ", Golden CPU: " << std::setw(3) << (int)golden_pc_before << "\n";
            all_match = false;
        }
        
        // Clock the hardware CPU (this executes instruction at current PC and updates PC)
        clock_cycle(designed_cpu, tfp, time);
        
        // Execute the same instruction in reference CPU (after HW clock to sync PC updates)
        uint8_t written_reg, written_value;
        bool reg_written = golden_cpu->executeInstruction(written_reg, written_value);
        
        // Compare states after each cycle
        bool match = compare_cpus(designed_cpu, golden_cpu, cycle);
        if (!match) {
            all_match = false;
        }
        
        // Print state every 10 cycles, on first cycles, or on mismatch
        if (cycle < 10 || cycle % 10 == 0 || !match) {
            print_state(designed_cpu, golden_cpu, cycle);
            if (!match) {
                std::cout << "  err MISMATCH DETECTED!\n";
            }
            std::cout << "\n";
        }
    }
    
    // Final comparison
    std::cout << "\nFinal State Comparison:\n";
    print_state(designed_cpu, golden_cpu, clock_cycles);
    
    if (all_match) {
        std::cout << "\nok All comparisons passed! CPUs match perfectly.\n";
    } else {
        std::cout << "\nerr Some mismatches detected. See details above.\n";
    }
    
    std::cout << "\nVCD file: waveform_cpu.vcd\n";
    std::cout << "To view waveforms:\n";
    std::cout << "  gtkwave waveform_cpu.vcd\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete designed_cpu;
    delete golden_cpu;
    
    return all_match ? 0 : 1;
}
