module immediate_extend(
    input logic [3:0] imm_in,
    // input logic sign_extend, // 1 - sign-extend, 0 - zero-extend

    output logic [7:0] imm_out
);
    always_comb begin
        imm_out = {4'b0000, imm_in}; // zero-extend by default
        
        // if (sign_extend) begin
        //     // Sign-extend
        //     imm_out = {{4{imm_in[3]}}, imm_in}; // replicate sign bit
        // end else begin
        //     // Zero-extend
        //     imm_out = {4'b0000, imm_in};
        // end
    end
endmodule
