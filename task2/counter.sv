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
Challenge portion: we want the behaviour to be that if en = 1, we count up, else if en = 0, we count down
en is controlled using the vbd flag parameter
*/
always_ff @ (posedge clk)
    if (rst)     count <= {WIDTH{1'b0}};                      // if reset is high, set it to 0
    else if (en) count <= count + {{WIDTH - 1{1'b0}}, 1'b1};   // else if en = 1, increment count by 1
    else         count <= count - {{WIDTH - 1{1'b0}}, 1'b1};  // else, en = 0 and decrement count by 1

endmodule
