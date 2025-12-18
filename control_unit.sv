module control_unit(
    input logic [7:0] instruction,

    output logic [1:0] opcode,
    output logic [1:0] rd,
    output logic [1:0] rs1,
    output logic [1:0] rs2,
    output logic [3:0] addr,
    output logic [3:0] imm
);
    
    // Extract all fields first (constant slices outside always_comb)
    logic [1:0] instr_opcode;
    logic [1:0] instr_rd;
    logic [1:0] instr_rs1;
    logic [1:0] instr_rs2;
    logic [3:0] instr_addr;
    logic [3:0] instr_imm;
    
    assign instr_opcode = instruction[7:6];
    assign instr_rd = instruction[5:4];
    assign instr_rs1 = instruction[3:2];
    assign instr_rs2 = instruction[1:0];
    assign instr_addr = instruction[5:2];
    assign instr_imm = instruction[3:0];

    // Decode based on opcode
    always_comb begin
        opcode = instr_opcode;
        case (instr_opcode)  // Use internal signal instead of output
            2'b00: begin
                // add-type instruction
                rd = instr_rd;
                rs1 = instr_rs1;
                rs2 = instr_rs2;

                // other fields not used
                addr = 4'b0000;
                imm = 4'b0000;
            end
            2'b10: begin
                // li-type instruction
                rd = instr_rd;
                imm = instr_imm;

                // other fields not used
                rs1 = 2'b00;
                rs2 = 2'b00;
                addr = 4'b0000;
            end
            2'b11: begin
                // bner0-type instruction
                addr = instr_addr;
                rs2 = instr_rs2;

                // other fields not used
                rd = 2'b00;
                rs1 = 2'b00;
                imm = 4'b0000;
            end            
            default: begin
                // TODO: Handle other opcodes
                rd = 2'b00;
                rs1 = 2'b00;
                rs2 = 2'b00;
                imm = 4'b0000;
                addr = 4'b0000;
            end
        endcase
    end

endmodule