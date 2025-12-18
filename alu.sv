module alu(
    input logic [7:0] operand_a,
    input logic [7:0] operand_b,
    input logic [1:0] alu_op, // 00 = add, 01 = sub, 10 = and, 11 = or

    output logic [7:0] result,
    output logic zero_flag
);

    always_comb begin
        case (alu_op)
            2'b00: result = operand_a + operand_b; // add
            2'b01: result = operand_a - operand_b; // sub
            2'b10: result = operand_a & operand_b; // and
            2'b11: result = operand_a | operand_b; // or
            default: result = 8'b00000000; // default case
        endcase

        // Set zero flag
        if (result == 8'b00000000) begin
            zero_flag = 1'b1;
        end else begin
            zero_flag = 1'b0;
        end
    end
endmodule
