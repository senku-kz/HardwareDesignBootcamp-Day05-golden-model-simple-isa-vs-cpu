module immediate_extend(
    input logic [3:0] imm_in,
    // input logic sign_extend, // 1 - sign-extend, 0 - zero-extend

    output logic [7:0] imm_out
);
    always_comb begin
        imm_out = {4'b0000, imm_in}; // zero-extend by default
    end
endmodule
