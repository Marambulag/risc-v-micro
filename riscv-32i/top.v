module top(
    input clk,
    input reset
);

// PC wires
wire [31:0] pc, next_pc, pc_plus4, branch_target;

// Instruction fetch
wire [31:0] instruction;

// Control signals
wire branch, mem_read, mem_to_reg, mem_write, alu_src, reg_write;
wire [1:0] alu_op;

// Register file wires
wire [4:0] rs1, rs2, rd;
wire [31:0] reg_read_data1, reg_read_data2, write_back_data;

// Immediate generator
wire [31:0] immediate;

// ALU Control
wire [3:0] alu_control_signal;

// ALU
wire [31:0] alu_input2, alu_result;
wire zero;

// Data Memory
wire [31:0] mem_read_data;

// Branch comparator
wire take_branch;

// Mux outputs
wire [31:0] alu_mux_out;
wire [31:0] mem_to_reg_mux_out;

// PC + 4
adder pc_adder(
    .in1(pc),
    .in2(32'd4),
    .out(pc_plus4)
);

// Branch target = PC + immediate
adder branch_adder(
    .in1(pc),
    .in2(immediate),
    .out(branch_target)
);

// PC update logic
assign next_pc = (take_branch) ? branch_target : pc_plus4;

program_counter pc_reg(
    .clk(clk),
    .reset(reset),
    .next_pc(next_pc),
    .pc(pc)
);

instruction_memory instr_mem(
    .addr(pc),
    .instruction(instruction)
);

assign rs1 = instruction[19:15];
assign rs2 = instruction[24:20];
assign rd  = instruction[11:7];

control_unit ctrl_unit(
    .opcode(instruction[6:0]),
    .branch(branch),
    .mem_read(mem_read),
    .mem_to_reg(mem_to_reg),
    .alu_op(alu_op),
    .mem_write(mem_write),
    .alu_src(alu_src),
    .reg_write(reg_write)
);

register_file reg_file(
    .clk(clk),
    .reg_write(reg_write),
    .rs1(rs1),
    .rs2(rs2),
    .rd(rd),
    .write_data(write_back_data),
    .read_data1(reg_read_data1),
    .read_data2(reg_read_data2)
);

immediate_generator imm_gen(
    .instruction(instruction),
    .immediate(immediate)
);

alu_control alu_ctrl(
    .alu_op(alu_op),
    .funct3(instruction[14:12]),
    .funct7(instruction[31:25]),
    .alu_control_signal(alu_control_signal)
);

mux2 #(32) alu_src_mux(
    .in0(reg_read_data2),
    .in1(immediate),
    .sel(alu_src),
    .out(alu_mux_out)
);

alu alu_unit(
    .input1(reg_read_data1),
    .input2(alu_mux_out),
    .alu_control(alu_control_signal),
    .alu_result(alu_result),
    .zero(zero)
);

data_memory data_mem(
    .clk(clk),
    .mem_write(mem_write),
    .mem_read(mem_read),
    .addr(alu_result),
    .write_data(reg_read_data2),
    .read_data(mem_read_data)
);

mux2 #(32) mem_to_reg_mux(
    .in0(alu_result),
    .in1(mem_read_data),
    .sel(mem_to_reg),
    .out(mem_to_reg_mux_out)
);

assign write_back_data = mem_to_reg_mux_out;

branch_comparator branch_comp(
    .rs1(reg_read_data1),
    .rs2(reg_read_data2),
    .branch(branch),
    .take_branch(take_branch)
);

endmodule
