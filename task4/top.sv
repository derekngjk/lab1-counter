module top # (
    parameter WIDTH = 8
)(
    // interface signals
    input logic            clk,  // clk signal
    input logic            rst,  // reset signal
    input logic            en,   // enable
    output logic[11:0]     bcd   // output as a bcd
);

    // interconnect signal. This is the output of the counter module, which is fed as input to the bin2bcd module
    logic[WIDTH-1:0] count;  

// pass in the input clk, rst, en as the clk, rst, and en of the counter module
// output of the counter module stored in count
counter myCounter (
    .clk (clk),
    .rst (rst),
    .en (en),
    .count (count)
);

// pass in the count signal as the input for bin2bcd
// and take the output of bin2bcd as the output for the top module
bin2bcd myDecoder (
    .x (count),
    .BCD (bcd)
);

endmodule
