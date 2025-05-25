module immediate_generator(
    input [31:0] instruction,
    output reg [31:0] immediate
);

wire [6:0] opcode = instruction[6:0];
wire [2:0] funct3 = instruction[14:12];

always @(*) begin
    case(opcode)
        7'b0010011, 7'b0000011: begin // I-type
            immediate = {{20{instruction[31]}}, instruction[31:20]};
        end
        7'b0100011: begin // S-type
            immediate = {{20{instruction[31]}}, instruction[31:25], instruction[11:7]};
        end
        7'b1100011: begin // B-type
            immediate = {{19{instruction[31]}}, instruction[31], instruction[7], instruction[30:25], instruction[11:8], 1'b0};
        end
        default: immediate = 32'b0;
    endcase
end

endmodule
