module program_counter(
    input logic clk,
    input logic [1:0] opcode,
    input logic [3:0] set_value,
    input logic reset,
    output logic [3:0] pc_out
);

always_ff @(posedge clk) begin

    if (reset) begin
        pc_out <= 4'b0000; // Reset PC to 0
    end else if (opcode == 2'b11) begin
        pc_out <= set_value; // Branch instruction sets PC to address
    end else begin
        pc_out <= pc_out + 4'b0001; // Increment PC by 1
    end

end

endmodule
