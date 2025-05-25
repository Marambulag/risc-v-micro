module instruction_memory(
    input [31:0] addr,
    output [31:0] instruction
);

reg [31:0] memory [0:255];

// Initialize from file
initial begin
    $readmemh("/home/manuel/small/test_program.mem", memory);
end

assign instruction = memory[addr[9:2]]; // Word aligned, 4-byte instructions

endmodule
