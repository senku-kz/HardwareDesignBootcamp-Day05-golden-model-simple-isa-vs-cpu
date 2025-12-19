
module register_file(
    input logic clk,
    input logic we, // write enable
    input logic [1:0] rd, // destination register
    input logic [1:0] rs1, // source register 1
    input logic [1:0] rs2, // source register 2
    input logic [7:0] wd, // write data
    output logic [7:0] rd_out, // read data from rd
    output logic [7:0] rs1_out, // read data from rs1
    output logic [7:0] rs2_out, // read data from rs2
    // Debug outputs for all registers
    output logic [7:0] reg0_out,
    output logic [7:0] reg1_out,
    output logic [7:0] reg2_out,
    output logic [7:0] reg3_out
);

    // Define 4 registers of 8 bits each (2 bits to address 4 registers)
    logic [7:0] registers [0:3];

    // Read ports (combinational)
    assign rs1_out = registers[rs1];
    assign rs2_out = registers[rs2];
    assign rd_out  = registers[rd];
    
    // Debug outputs for all registers
    assign reg0_out = registers[0];
    assign reg1_out = registers[1];
    assign reg2_out = registers[2];
    assign reg3_out = registers[3];

    // Write port (sequential)
    always_ff @(posedge clk) begin
        if (we) begin
            registers[rd] <= wd; // Write data to destination register
        end
    end

endmodule
