module alu_control(
    input [1:0] alu_op,
    input [2:0] funct3,
    input [6:0] funct7,
    output reg [3:0] alu_control_signal
);

always @(*) begin
    case(alu_op)
        2'b00: alu_control_signal = 4'b0010; // ADD (load/store)
        2'b01: alu_control_signal = 4'b0110; // SUB (branch)
        2'b10: begin
            case({funct7[5], funct3})
                4'b0000: alu_control_signal = 4'b0010; // ADD
                4'b1000: alu_control_signal = 4'b0110; // SUB
                4'b1110: alu_control_signal = 4'b0000; // AND
                4'b1100: alu_control_signal = 4'b0001; // OR
                4'b0100: alu_control_signal = 4'b0111; // SLT
                default: alu_control_signal = 4'b1111; // NOP/invalid
            endcase
        end
        default: alu_control_signal = 4'b1111; // NOP/invalid
    endcase
end

endmodule
