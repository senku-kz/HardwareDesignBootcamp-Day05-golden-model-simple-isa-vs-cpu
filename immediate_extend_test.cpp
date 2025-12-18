#include <iostream>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vimmediate_extend.h"

int main(int argc, char** argv) {
    // Initialize Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    // Create DUT and VCD trace
    Vimmediate_extend* dut = new Vimmediate_extend;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("waveform_imm.vcd");

    uint64_t time = 0;

    auto eval_dump = [&]() {
        dut->eval();
        tfp->dump(time++);
    };

    std::cout << "Testing immediate_extend (zero-extend 4->8)\n";
    std::cout << "==========================================\n\n";

    int passed = 0;

    // Sweep all 4-bit values and check zero-extend into 8 bits
    for (int v = 0; v < 16; ++v) {
        dut->imm_in = v & 0xF;
        eval_dump();

        uint8_t expected = static_cast<uint8_t>(v & 0x0F); // 0x0[v]
        bool ok = (dut->imm_out == expected);

        std::cout << "imm_in=0b" << std::bitset<4>(v)
                  << " -> imm_out=0x" << std::hex << (int)dut->imm_out
                  << std::dec << " (expected 0x" << std::hex << (int)expected << std::dec << ")"
                  << (ok ? "  \xE2\x9C\x93" : "  \xE2\x9C\x97") << "\n";

        if (!ok) {
            std::cerr << "\n✗ FAIL: zero-extend mismatch at value " << v << "\n";
            tfp->close();
            delete tfp;
            delete dut;
            return 1;
        }
        passed++;
    }

    // Spot checks on edges
    std::cout << "\nEdge checks:\n";
    // 0x0 -> 0x00
    dut->imm_in = 0x0; eval_dump(); if (dut->imm_out != 0x00) { std::cerr << "Edge 0x0 failed\n"; return 1; }
    // 0xF -> 0x0F (still zero-extend; sign not used here)
    dut->imm_in = 0xF; eval_dump(); if (dut->imm_out != 0x0F) { std::cerr << "Edge 0xF failed\n"; return 1; }

    // Cleanup
    tfp->close();
    delete tfp;
    delete dut;

    std::cout << "\n✅ All " << passed << " zero-extend cases passed!\n";
    std::cout << "VCD file: waveform_imm.vcd\n";
    return 0;
}
