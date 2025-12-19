#include <cstdint>
#include <vector>
#include "sCPU.h"

// Instruction format (8-bit):
// Bits 7-6: opcode (2 bits)
// LOAD (10): 10 DD MMMM (dest=2bits, immediate=4bits)
// ADD  (00): 00 DD S1 S2 (dest=2bits, src1=2bits, src2=2bit)
// JUMP (11): 11 AAAA S2 (address=4bits, src2=2bit)
// BNER0 (11): 11 AAAA S2 (branch to address if register S2 != r0)


// Constructors
sCPU::sCPU(){
    // Initialize PC and registers
    this->pc_ = 0;
    for (int i = 0; i < 4; ++i) {
        regs_[i] = 0b0;
    }

    // Initialize instruction memory as empty
    this->imem_.clear();
}

// Destructor
sCPU::~sCPU() {

}

// Get/Set PC
uint8_t sCPU::getPc() {
    return this->pc_;
}

void sCPU::setPc(uint8_t pc) {
    this->pc_ = pc;
}

// Get/Set register values
uint8_t sCPU::getRegister(uint8_t register_index) {
    if (register_index < 8) {
        return regs_[register_index];
    }
    return 0;
}

void sCPU::setRegister(uint8_t register_index, uint8_t register_value) {
    if (register_index < 8) {
        regs_[register_index] = register_value;
    }
}

// Load program as raw instruction bytes (8-bit instructions)
void sCPU::loadInstructions(const std::vector<uint8_t>& instructions) {
    this->imem_ = instructions;
}

uint8_t sCPU::fetchInstruction(uint8_t index) {
    // Fetch 8-bit instruction from byte address
    if (index >= this->imem_.size()) {
        return 0;
    }
    return this->imem_[index];
}

// Execute one instruction at PC
// Returns true if a register was written
// Also RETURNS which register and value were written via reference parameters
bool sCPU::executeInstruction(uint8_t& written_reg, uint8_t& written_value) {
    uint8_t instruction = fetchInstruction(this->pc_);
    uint8_t opcode = (instruction >> 6) & 0x3;

    bool reg_written = false;

    if (opcode == 0b10) {
        // LOAD: 10 DD MMMM
        uint8_t destination_register = (instruction >> 4) & 0x3;
        uint8_t immediate = instruction & 0xF;

        this->regs_[destination_register] = immediate;
        written_reg = destination_register;
        written_value = immediate;
        reg_written = true;

        this->pc_++;

    } else if (opcode == 0b00) {
        // ADD: 00 DD S1 S2
        uint8_t dest_reg = (instruction >> 4) & 0x3;
        uint8_t src1_reg = (instruction >> 2) & 0x3;
        uint8_t src2_reg = instruction & 0x3;
        uint8_t result = this->regs_[src1_reg] + this->regs_[src2_reg];
        this->regs_[dest_reg] = result;
        written_reg = dest_reg;
        written_value = result;
        reg_written = true;

        this->pc_++;

    } else if (opcode == 0b11) {
        // BNER0 / JUMP: 11 AAAA S2
        // Branch to address if register S2 != r0
        uint8_t target_addr = (instruction >> 2) & 0xF;
        uint8_t src2_reg = instruction & 0x3;
        
        if (this->regs_[src2_reg] != this->regs_[0]) {
            this->pc_ = target_addr;
        } else {
            this->pc_++;
        }

    } else {
        this->pc_++;
    }

    return reg_written;
}
