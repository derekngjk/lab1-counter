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
    // set the initial ld to 0 and v to 0
    top -> ld = 0;
    top -> v = 0;

    // use vbdSetMode(1) such that once the register is read, the flag will reset to 0
    vbdSetMode(1);

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
        on each cycle, we check the value of the vbd flag. If it is 1, reading it will reset it to 0
        suppose we press the switch on cycle i = 5. Then, we will dump clk = 0 and set top -> clock = 1 and eval
        Afterwards, we then set top -> ld = 1 and top -> v = vbdValue
        But on cycle 6, we still dump the count value BEFORE we set top -> v = vbdvalue
        We dump clk = 1 and set top -> clk = 0 and eval, but this does not do anything since it is falling edge
        Afterwards, we dump clk = 0 and set top -> clk = 1 and eval, and now this will then set top -> count = v
        But this is only dumped on the next cycle
        So basically, if e.g. we pressed the switch on i = 5, there will be one cycle lag and the preloaded value
        will only appear on cycle i = 7.
        */
        // if we didn't press the switch at all, then this just remains as 0 and does nothing
        // if we pressed the switch, then we set top -> ld = 1, and the flag resets to 0 so top -> ld becomes 0 on next cycle
        top -> ld = vbdFlag();
        top -> v = vbdValue();

        // vbdPlot(int(top -> count), 0, 255);

        // part 2: use 7 seg display to make it easier to see the single-stepping action
        vbdHex(4, int(top -> count) >> 12 & 0xF);
        vbdHex(3, int(top -> count) >> 8 & 0xF);
        vbdHex(2, int(top -> count) >> 4 & 0xF);
        vbdHex(1, int(top -> count) & 0xF);

        vbdCycle(i + 1);  // increment cycle number on vbuddy

        if (Verilated::gotFinish() || vbdGetkey() == 113) exit(0);  // exit if completed or 'q' key pressed

    }

    tfp -> close();
    exit(0);
}