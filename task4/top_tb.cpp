// include header files
#include "Vtop.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "vbuddy.cpp"


// purpose of the test bench is to simulate the behaviour of the module under specified input conditions
int main(int argc, char **argv, char **env) {
    // i is number of cycles to simulate. clk varies between 0 and 1 to emulate the clock signal on each cycle
    int i;
    int clk;

    Verilated::commandArgs(argc, argv);
    // create top verilog instance which is the device under test (DUT)
    Vtop* top = new Vtop;
    // turn on signal tracing, and tell verilator to dump the waveform data to counter.cvd
    Verilated::traceEverOn(true);
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top -> trace(tfp, 99);
    tfp -> open("counter.vcd");

    // initialise vbuddy connection. port path is in vbuddy.cfg
    if (vbdOpen() != 1) return (-1);
    // note: will have error if the header name is too long
    vbdHeader("Lab 1: Counter");

    // set the initial conditions of the top-level module. Just use rst 0 and en 1 to count normally
    top -> clk = 1;
    top -> rst = 0;
    top -> en = 1;

    // i is the index showing which clock cycle we are currently on. So simulate for 300 clock cycles
    for (i = 0; i < 1000; i++) {
        
        // on each cycle, we toggle clock two times so that clock switches between 0 and 1 on each cycle
        for (clk = 0; clk < 2; clk++) {
            // dump the variables into the vcd file
            tfp -> dump(2 * i + clk);  // units in ps
            top -> clk = !top -> clk;
            // eval basically sends the corresponding input signals into the module to update the output
            top -> eval();
        }

        /*
        Now, the counter basically counts up by 1 every clock cycle.
        But when we send the count to the vbuddy for display, we do not send the raw hexadecimal count.
        Instead, we send the count which has been converted into bcd format
        Example, when the count is 0x0009, we display 0009
        However, once the count is 0x000A, instead of displaying 000A, we display 0010
        And so on. Internally the counter module doesn't care about the bcd, and it doesn't even 'see' the bin2bcd module
        So the counter itself just keeps on counting up to A, B, C, D etc.
        Just that the overall output of the top module is the count value which has been converted into bcd
        So now we just send the 3 bcd digits to the 7 seg display to display
        */
        vbdHex(3, int(top -> bcd) >> 8 & 0xF);
        vbdHex(2, int(top -> bcd) >> 4 & 0xF);
        vbdHex(1, int(top -> bcd) & 0xF);
        vbdCycle(i + 1);  // increment cycle number on vbuddy

        if (Verilated::gotFinish() || vbdGetkey() == 113) exit(0);  // exit if completed or 'q' key pressed

    }

    tfp -> close();
    exit(0);
}