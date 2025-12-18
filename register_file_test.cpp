#include <iostream>
#include <bitset>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vregister_file.h"

void print_registers(Vregister_file* rf, const std::string& msg) {
    std::cout << "  " << msg << "\n";
    std::cout << "    rs1[" << (int)rf->rs1 << "] = 0b" << std::bitset<8>((int)rf->rs1_out) << "\n";
    std::cout << "    rs2[" << (int)rf->rs2 << "] = 0b" << std::bitset<8>((int)rf->rs2_out) << "\n";
    std::cout << "     rd[" << (int)rf->rd << "] = 0b" << std::bitset<8>((int)rf->rd_out) << "\n";
}

void clock_cycle(Vregister_file* rf, VerilatedVcdC* tfp, uint64_t& time) {
    rf->clk = 0;
    rf->eval();
    tfp->dump(time++);
    
    rf->clk = 1;
    rf->eval();
    tfp->dump(time++);
}

int main(int argc, char** argv) {
    // Инициализация Verilator
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    
    // Создание модуля и VCD trace
    Vregister_file* rf = new Vregister_file;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    rf->trace(tfp, 99);
    tfp->open("waveform_rf.vcd");
    
    uint64_t time = 0;
    
    std::cout << "Testing Register File\n";
    std::cout << "=====================\n\n";
    
    // Initialize
    rf->we = 0;
    rf->rd = 0;
    rf->rs1 = 0;
    rf->rs2 = 0;
    rf->wd = 0;
    
    // Test 1: Register 0 
    std::cout << "Test 1: Set value to Register R0\n";
    uint8_t expected_result = 0b11111111;
    uint8_t actual_result = 0;


    rf->we = 1;
    rf->rd = 0;
    rf->wd = expected_result;
    clock_cycle(rf, tfp, time);
    
    rf->we = 0;
    rf->rs1 = 0;
    rf->eval();
    tfp->dump(time++);
    actual_result = rf->rs1_out;
    
    if (actual_result != expected_result) {
        std::cerr << "  ✗ FAIL: r0 should be 0, got " << (int)actual_result << "\n";
        return 1;
    }
    std::cout << "  ✓ Register R0 was set to 0b11111111, read back as 0b" 
              << std::bitset<8>(actual_result) << " (expected 0b11111111)\n\n";
    
    // Test 2: Write and read back from r1
    std::cout << "Test 2: Write and read from r1\n";
    rf->we = 1;
    rf->rd = 1;
    rf->wd = 0x42;
    clock_cycle(rf, tfp, time);
    
    rf->we = 0;
    rf->rs1 = 1;
    rf->eval();
    tfp->dump(time++);
    
    if (rf->rs1_out != 0x42) {
        std::cerr << "  ✗ FAIL: Expected 0x42, got 0x" << std::hex << (int)rf->rs1_out << std::dec << "\n";
        return 1;
    }
    std::cout << "  ✓ r1 = 0x42\n\n";
    
    // Test 3: Write to multiple registers
    std::cout << "Test 3: Write to multiple registers\n";
    
    // Write to r0, r1, r2, r3
    uint8_t test_values[4] = {
        0b10001010, 
        0b10010000, 
        0b10100000, 
        0b10110001
    };

    for (int i = 0; i <= 3; i++) {
        rf->we = 1;
        rf->rd = i;
        rf->wd = test_values[i];
        clock_cycle(rf, tfp, time);
        std::cout << "  Written r" << i << " = 0x" << std::hex << test_values[i] << std::dec << "\n";
    }
    
    // Read back all registers
    std::cout << "\n  Reading back:\n";
    rf->we = 0;
    for (int i = 0; i <= 3; i++) {
        rf->rs1 = i;
        rf->eval();
        tfp->dump(time++);
        
        uint8_t expected_result = test_values[i];
        uint8_t actual_result = rf->rs1_out;

        if (actual_result != expected_result) {
            std::cerr << "  ✗ FAIL at r" << i << ": Expected 0x" << std::hex << expected_result 
                      << ", got 0x" << (int)actual_result << std::dec << "\n";
            return 1;
        }
        std::cout << "  ✓ r" << i << " = 0x" << std::hex << (int)actual_result << std::dec << "\n";
    }
    std::cout << "\n";
    
    // Test 4: Read two registers simultaneously
    std::cout << "Test 4: Simultaneous dual-port read (rs1 and rs2)\n";
    rf->rs1 = 1;
    rf->rs2 = 2;
    rf->eval();
    tfp->dump(time++);
    
    print_registers(rf, "Dual read:");

    uint8_t expected_result_rs1 = test_values[rf->rs1];
    uint8_t expected_result_rs2 = test_values[rf->rs2];
    uint8_t actual_result_rs1 = rf->rs1_out;
    uint8_t actual_result_rs2 = rf->rs2_out;
    
    if (actual_result_rs1 != expected_result_rs1 || actual_result_rs2 != expected_result_rs2) {
        std::cerr << "  ✗ FAIL: Dual read failed\n";
        return 1;
    }
    std::cout << "  ✓ Dual read successful\n\n";
    
    // Test 5: Write enable off - no write
    std::cout << "Test 5: Write enable disabled (we=0)\n";
    rf->we = 0;
    rf->rd = 1;
    rf->wd = 0xFF;  // Try to write with we=0
    clock_cycle(rf, tfp, time);
    
    rf->rs1 = 1;
    rf->eval();
    tfp->dump(time++);

    expected_result = test_values[rf->rd];
    actual_result = rf->rs1_out;
    
    if (expected_result != actual_result) {  // Should still be old value
        std::cerr << "  ✗ FAIL: Register changed when we=0\n";
        return 1;
    }
    std::cout << "  ✓ r1 unchanged (still 0x" << std::hex << (int)actual_result << std::dec << ")\n\n";
    
    // Test 6: Overwrite register
    std::cout << "Test 6: Overwrite existing register\n";
    rf->we = 1;
    rf->rd = 2;
    rf->wd = 0x11;  // Overwrite r2 (was 0xBB)
    clock_cycle(rf, tfp, time);
    
    rf->we = 0;
    rf->rs1 = 2;
    rf->eval();
    tfp->dump(time++);
    
    if (rf->rs1_out != 0x11) {
        std::cerr << "  ✗ FAIL: Overwrite failed\n";
        return 1;
    }
    std::cout << "  ✓ r2 overwritten: 0xBB → 0x11\n\n";
    
    // Test 7: Read from rd port
    std::cout << "Test 7: Read from rd port\n";
    rf->rd = 3;
    rf->eval();
    tfp->dump(time++);

    expected_result = test_values[rf->rd];
    actual_result = rf->rd_out;
    
    if (actual_result != expected_result) {
        std::cerr << "  ✗ FAIL: rd_out incorrect\n";
        return 1;
    }
    std::cout << "  ✓ rd[3] = 0x" << std::hex << (int)actual_result << std::dec << "\n\n";
    
    // Test 8: All registers at once
    std::cout << "Test 8: Read all three ports simultaneously\n";

    for (int i = 0; i <= 3; i++) {
        rf->we = 1;
        rf->rd = i;
        rf->wd = test_values[i];
        clock_cycle(rf, tfp, time);
        std::cout << "  Written r" << i << " = 0b" << std::bitset<8>(test_values[i]) << "\n";
    }

    rf->rd = 3;
    rf->rs1 = 1;
    rf->rs2 = 2;
    rf->eval();
    tfp->dump(time++);
    
    print_registers(rf, "Triple read:");

    expected_result_rs1 = test_values[rf->rs1];
    expected_result_rs2 = test_values[rf->rs2];
    uint8_t expected_result_rd = test_values[rf->rd];
    actual_result_rs1 = rf->rs1_out;
    actual_result_rs2 = rf->rs2_out;
    uint8_t actual_result_rd = rf->rd_out;
    
    if (actual_result_rd != expected_result_rd 
        || actual_result_rs1 != expected_result_rs1 
        || actual_result_rs2 != expected_result_rs2
    ) {
        std::cerr << "  ✗ FAIL: Triple read failed\n";
        return 1;
    }
    std::cout << "  ✓ All three ports read correctly\n\n";
    
    // Cleanup
    tfp->close();
    delete tfp;
    delete rf;
    
    std::cout << "✅ All tests passed!\n";
    std::cout << "VCD file: waveform_rf.vcd\n";
    return 0;
}
