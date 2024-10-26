module counter # (
    parameter WIDTH = 8
) (
    // interface signals
    input logic             clk,
    input logic             rst,
    input logic             ld,
    input logic[WIDTH-1:0]  v,
    output logic[WIDTH-1:0] count
);

/*
basically, whenever on a rising clock edge, if reset is high, we set counter to 0
otherwise if ld is high, we set the count to the constant v,
otherwise we increment count by 1
*/
// always_ff @ (posedge clk)
//     if (rst) count <= {WIDTH{1'b0}};
//     else     count <= ld? v : count + {{WIDTH-1{1'b0}}, 1'b1};

/*
Part 2: we want to single step the counting such that we count up by 1 only when we press the switch, else do nothing
The idea is that we want to use the switch to provide one clock pulse, rather than using the continuous clock
To achieve this, we set the ff to only enter the loop whenever we see a positive edge of ld, instead of positive edge of clk
And then in the test bench, we interface the counter with vbuddy by passing in the vbdFlag as the input ld signal
So that in one-shot mode, whenever we press the switch, vbdFlag will be 1, and then reset to 0
So that we only send one rising edge to the ff, and thus the ff only increments by 1
Afterward, the vbdflag will go back to 0 so we do not enter the loop any more until we press the switch again.
*/
always_ff @ (posedge ld)
    if (rst) count <= {WIDTH{1'b0}};
    else     count <= count + {{WIDTH-1{1'b0}}, 1'b1};

endmodule
