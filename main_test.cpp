#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vmain.h"

void clock_cycle(Vmain* cpu, VerilatedVcdC* tfp, uint64_t& time) {
    cpu->clk = 0;
    cpu->eval();
    tfp->dump(time++);
    
    cpu->clk = 1;
    cpu->eval();
    tfp->dump(time++);
}

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Create CPU and VCD trace
    Vmain* cpu = new Vmain;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    cpu->trace(tfp, 99);
    tfp->open("waveform_cpu.vcd");
    
    uint64_t time = 0;
    
    std::cout << "Testing Simple ISA CPU\n";
    std::cout << "======================\n\n";
    
    // Reset
    std::cout << "Resetting CPU...\n";
    cpu->reset = 1;
    clock_cycle(cpu, tfp, time);
    clock_cycle(cpu, tfp, time);
    cpu->reset = 0;
    std::cout << "✓ Reset complete\n\n";
    
    // Run for 100 clock cycles to execute instructions
    std::cout << "Running CPU for 100 cycles...\n";
    for (int cycle = 0; cycle < 100; cycle++) {
        clock_cycle(cpu, tfp, time);
        std::cout << "Cycle " << cycle << " complete\n";
    }
    
    std::cout << "\n✅ CPU simulation complete!\n";
    std::cout << "VCD file: waveform_cpu.vcd\n";
    std::cout << "\nTo view waveforms:\n";
    std::cout << "  gtkwave waveform_cpu.vcd\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete cpu;
    
    return 0;
}
