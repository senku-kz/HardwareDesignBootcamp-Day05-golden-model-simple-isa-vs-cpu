/*
Simple Instruction Set Architecture (sISA) CPU
Integrates all components into a functional processor

Architecture:
- Von Neumann style (single memory for instructions)
- 4-bit PC, 8-bit data path
- 3 instruction types:
  * 00: ADD/SUB (R-type)
  * 10: LI (Load Immediate)
  * 11: BNER0 (Branch if Not Equal to R0)
*/

module main(
    input logic clk,
    input logic reset
);

    // ========== Signals ==========
    
    // Program Counter
    logic [3:0] pc_out;
    logic [1:0] pc_opcode;
    logic [3:0] pc_set_value;
    
    // Instruction Memory
    logic [7:0] instruction;
    
    // Control Unit
    logic [1:0] opcode;
    logic [1:0] rd, rs1, rs2;
    logic [3:0] imm;
    logic [3:0] branch_addr;
    
    // Register File
    logic reg_we;                  // Write enable
    logic [7:0] reg_wd;            // Write data (8 bits)
    logic [7:0] reg_rs1_data;      // Read data from rs1 (8 bits)
    logic [7:0] reg_rs2_data;      // Read data from rs2 (8 bits)
    logic [7:0] reg_rd_data;       // Read data from rd (8 bits)
    
    // Immediate Extend
    logic [7:0] imm_extended;
    
    // ALU
    logic [7:0] alu_operand_a;
    logic [7:0] alu_operand_b;
    logic [1:0] alu_op;
    logic [7:0] alu_result;
    logic alu_zero_flag;
    
    
    // ========== Component Instantiation ==========
    
    // 1. Program Counter
    program_counter pc_inst (
        .clk(clk),
        .reset(reset),
        .opcode(pc_opcode),
        .set_value(pc_set_value),
        .pc_out(pc_out)
    );
    
    // 2. Instruction Memory (ROM)
    instruction_memory imem_inst (
        .address(pc_out),
        .instruction(instruction)
    );
    
    // 3. Control Unit (Instruction Decoder)
    control_unit cu_inst (
        .instruction(instruction),
        .opcode(opcode),
        .rd(rd),
        .rs1(rs1),
        .rs2(rs2),
        .addr(branch_addr),
        .imm(imm)
    );
    
    // 4. Immediate Extend
    immediate_extend imm_ext_inst (
        .imm_in(imm),
        .imm_out(imm_extended)
    );
    
    // 5. Register File
    register_file regfile_inst (
        .clk(clk),
        .we(reg_we),
        .rd(rd),
        .rs1(rs1),
        .rs2(rs2),
        .wd(reg_wd),
        .rd_out(reg_rd_data),
        .rs1_out(reg_rs1_data),
        .rs2_out(reg_rs2_data)
    );
    
    // 6. ALU
    alu alu_inst (
        .operand_a(alu_operand_a),
        .operand_b(alu_operand_b),
        .alu_op(alu_op),
        .result(alu_result),
        .zero_flag(alu_zero_flag)
    );
    
    
    // ========== Control Logic ==========
    
    always_comb begin
        // Default values
        reg_we = 0;
        reg_wd = 0;
        alu_op = 2'b00;
        alu_operand_a = 0;
        alu_operand_b = 0;
        pc_opcode = 2'b00;      // Normal increment
        pc_set_value = 0;
        
        case (opcode)
            2'b00: begin
                // R-type: ADD/SUB instruction
                // rd = rs1 + rs2 (or sub, based on instruction detail)
                reg_we = 1;
                alu_op = 2'b00;              // ADD operation
                alu_operand_a = reg_rs1_data;
                alu_operand_b = reg_rs2_data;
                reg_wd = alu_result;         // Write ALU result back
            end
            
            2'b10: begin
                // I-type: Load Immediate (LI)
                // rd = imm (load immediate into register)
                reg_we = 1;
                reg_wd = imm_extended;       // Load extended immediate
            end
            
            2'b11: begin
                // Branch: BNER0
                // If r0 > rs2, then PC = branch_addr else PC increments
                reg_we = 0;  // No register write

                // Compare r0 (rd_out) against rs2 to decide branch
                if (reg_rd_data > reg_rs2_data) begin
                    pc_opcode = 2'b11;          // Enable branch
                    pc_set_value = branch_addr; // Set PC to branch target
                end else begin
                    pc_opcode = 2'b00;          // Normal increment
                end
            end
            
            default: begin
                // NOP or invalid instruction
                reg_we = 0;
            end
        endcase
    end

endmodule
