#include <cstdint>
#include <vector>

class sCPU {
    public:
        sCPU();
        ~sCPU();

        // Get/Set PC
        uint8_t getPc();
        void setPc(uint8_t pc);


        // This section was made private, because these methods are only used in construction and execution internally.
        // They are not intended to be called directly from outside the class.

        // Get/Set register values
        uint8_t getRegister(uint8_t register_index);
        void setRegister(uint8_t register_index, uint8_t register_value);

        // Load program as raw instruction bytes (8-bit instructions)
        void loadInstructions(const std::vector<uint8_t>& bytes);

        // Helper: fetch 8-bit instruction at given address
        uint8_t fetchInstruction(uint8_t index);


        // Execute one instruction at PC
        // Returns true if a register was written
        // Also RETURNS which register and value were written via reference parameters
        bool executeInstruction(uint8_t& written_reg, uint8_t& written_value);

    private:
        // Architectural state
        uint8_t pc_;
        uint8_t regs_[4];

        // Instruction memory (raw bytes)
        std::vector<uint8_t> imem_;


 
};
