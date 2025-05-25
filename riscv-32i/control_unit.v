module control_unit(
    input [6:0] opcode,
    output reg branch,
    output reg mem_read,
    output reg mem_to_reg,
    output reg [1:0] alu_op,
    output reg mem_write,
    output reg alu_src,
    output reg reg_write
);

always @(*) begin
    case(opcode)
        7'b0110011: begin // R-type
            branch      = 0;
            mem_read    = 0;
            mem_to_reg  = 0;
            alu_op      = 2'b10;
            mem_write   = 0;
            alu_src     = 0;
            reg_write   = 1;
        end
        7'b0000011: begin // Load
            branch      = 0;
            mem_read    = 1;
            mem_to_reg  = 1;
            alu_op      = 2'b00;
            mem_write   = 0;
            alu_src     = 1;
            reg_write   = 1;
        end
        7'b0100011: begin // Store
            branch      = 0;
            mem_read    = 0;
            mem_to_reg  = 0; // X
            alu_op      = 2'b00;
            mem_write   = 1;
            alu_src     = 1;
            reg_write   = 0;
        end
        7'b1100011: begin // Branch
            branch      = 1;
            mem_read    = 0;
            mem_to_reg  = 0; // X
            alu_op      = 2'b01;
            mem_write   = 0;
            alu_src     = 0;
            reg_write   = 0;
        end
        7'b0010011: begin // I-type (e.g. addi)
            branch      = 0;
            mem_read    = 0;
            mem_to_reg  = 0;
            alu_op      = 2'b10;
            mem_write   = 0;
            alu_src     = 1;
            reg_write   = 1;
        end
        default: begin
            branch      = 0;
            mem_read    = 0;
            mem_to_reg  = 0;
            alu_op      = 2'b00;
            mem_write   = 0;
            alu_src     = 0;
            reg_write   = 0;
        end
    endcase
end

endmodule
