module counter # (
    parameter WIDTH = 8  // set the default value for WIDTH, can be overridden during instantiation
) (
    // interface signals: in the perspective of the top-level module instantiating counter, this is what it inputs and outputs
    input logic clk,                // clock
    input logic rst,                // reset
    input logic en,                 // enable
    output logic [WIDTH-1:0] count  // count output of with WIDTH number of bits
);

/*
Basically, every time there is a rising edge of clk, if reset is true, we reset count to 0
Otherwise, we add enable to the count (hence adding 1 to the count if enable is true, else add 0 so count stays the same)
NOTE: always use nonblocking assignment <= for flipflops
*/
// always_ff @ (posedge clk)
//     if (rst) count <= {WIDTH{1'b0}};
//     else     count <= count + {{WIDTH - 1{1'b0}}, en};

/*
Challenge 2: implementing async reset
So basically whenever clk goes high OR whenever rst goes high, we will enter the loop
So if this is triggered by posedge rst, rst will be 1 so we will reset the count
otherwise if this block is triggered by posedge clock, then we see whether rst has remained high, and if so reset to 0
Else if it is triggered by posedge clock and rst is low, then increment count as per normal
*/
always_ff @ (posedge clk, posedge rst)
    if (rst) count <= {WIDTH{1'b0}};
    else     count <= count + {{WIDTH - 1{1'b0}}, en};

endmodule