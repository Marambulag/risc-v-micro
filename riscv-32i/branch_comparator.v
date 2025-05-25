module branch_comparator(
    input [31:0] rs1,
    input [31:0] rs2,
    input branch,
    output reg take_branch
);

always @(*) begin
    if(branch && (rs1 == rs2))
        take_branch = 1;
    else
        take_branch = 0;
end

endmodule
