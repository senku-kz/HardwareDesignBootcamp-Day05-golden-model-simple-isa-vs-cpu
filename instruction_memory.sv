module instruction_memory(
    input logic [3:0] address,
    output logic [7:0] instruction
);

    // Simple instruction memory with 16 instructions (4-bit address)
    logic [7:0] memory [0:15];

    // Initialize the instruction memory with example program:
    // Program: Load immediates, add them, and loop
    initial begin
        // Format: [opcode(2) | fields...]
        // Opcode: 00=ADD, 10=LI, 11=BNER0
        /*
        10001010    # 0: li r0, 10
        10010000    # 1: li r1, 0
        10100000    # 2: li r2, 0
        10110001    # 3: li r3, 1
        00010111    # 4: add r1, r1, r3
        00101001    # 5: add r2, r2, r1
        11010001    # 6: bner0 r1, 4
        11011111    # 7: bner0 r3, 7
        */
        
        memory[0]  = 8'b10001010;  // li r0, 10     (Load 10 into r0)
        memory[1]  = 8'b10010000;  // li r1, 0     (Load 0 into r1)
        memory[2]  = 8'b10100000;  // li r2, 0     (Load 0 into r2)
        memory[3]  = 8'b10110001;  // li r3, 1     (Load 1 into r3)
        memory[4]  = 8'b00010111;  // add r1, r1, r3  (r1 = r1 + r3)
        memory[5]  = 8'b00101001;  // add r2, r2, r1  (r2 = r2 + r1)
        memory[6]  = 8'b11010001;  // bner0 r1, 4  (Branch to 4 if r1≠0)
        memory[7]  = 8'b11011111;  // bner0 r3, 7  (Branch to 7 if r3≠0)
        memory[8]  = 8'b00000000;
        memory[9]  = 8'b00000000;
        memory[10] = 8'b00000000;
        memory[11] = 8'b00000000;
        memory[12] = 8'b00000000;
        memory[13] = 8'b00000000;
        memory[14] = 8'b00000000;
        memory[15] = 8'b00000000;
    end

    // Read instruction from memory based on address
    always_comb begin
        instruction = memory[address];
    end

endmodule
