// include header files
#include "Vcounter.h"
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
    Vcounter* top = new Vcounter;
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
    for (i = 0; i < 300; i++) {
        
        // on each cycle, we toggle clock two times so that clock switches between 0 and 1 on each cycle
        for (clk = 0; clk < 2; clk++) {
            // dump the variables into the vcd file
            tfp -> dump(2 * i + clk);  // units in ps
            top -> clk = !top -> clk;
            // eval basically sends the corresponding input signals into the module to update the output
            top -> eval();
        }

        /*
        after we have performed top -> eval, we have updated top -> count
        We then want to send top -> count to the vbd 7seg display to see what the actual count value is
        note: in the vbdHex function, the first argument is simply which digit we are displaying the data on
        so vbdHex(1, xxx) means we are displaying the value xxx on digit 1 of the 7 seg display
        digits indexed from 1 to 5 (right to left)
        So example: int(top -> count) >> 8 & 0xF isolates bits 9:12 of top -> count and displays it on digit 3
        */
        // vbdHex(4, (int(top -> count) >> 12) & 0xF);
        // vbdHex(3, (int(top -> count) >> 8) & 0xF);
        // vbdHex(2, (int(top -> count) >> 4) & 0xF);
        // vbdHex(1, int(top -> count) & 0xF);
        /*
        part 2: instead of using vbdHex for 7 seg display, we can also use vbd plot to plot the count value
        How the vbdPlot function works is that we pass in the count value, as well as 0 to 255 which is the bounds
        The vbdPlot function will internally scale top -> count to between 0 and 255, and plot it on the screen
        As we keep cycling and plotting, the x value will increase so we just see a straight line
        Once the x value reaches 240, the screen will clear and it will continue plotting from x = 0
        */
        vbdPlot(int(top -> count), 0, 255);
        vbdCycle(i + 1);  // increment cycle number on vbuddy

        top -> en = vbdFlag();

        if (Verilated::gotFinish() || vbdGetkey() == 113) exit(0);  // exit if completed or 'q' key pressed

    }

    tfp -> close();
    exit(0);
}