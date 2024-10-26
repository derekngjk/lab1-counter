// include header files
#include "Vcounter.h"
#include "verilated.h"
#include "verilated_vcd_c.h"


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

    // set the initial conditions of the top-level module
    // setting initial rst to 1 and initial en to 0 is an arbitrary choice
    top -> clk = 1;
    // test 1: initial testing
    // top -> rst = 1;
    // top -> en = 0;
    // test 2: stop counting for 3 cycles once we reach 0x9, then resume counting
    top -> rst = 0;
    top -> en = 1;
    int pause_cycles = 3;  // number of cycles to pause once we hit 0x9

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

        // test 1: initial testing
        // for testing purposes, we make rst = 1 on cycle 0, 1 and 15
        // top -> rst = i < 2 | i == 15;
        // for testing purposes, we only enable after 6 cycles (when i becomes 5)
        // top -> en = i > 4;

        // test 2: stop counting for 3 cycles once the counter reaches 0x9, then resume counting
        /*
        If the counter has reached 0x9, we set en to false if pause_cycles - 1 is positive. pause_cycles starts from 3
        Note that after we dump 0x8 into the .vcd file, we then set top -> count = 0x9 using top -> eval
        We then set en = 0, so that on the next cycle, we dump 0x9, but then top -> count stays at 0x9
        Note that the reason why we check pause_cycles - 1 is because, recall that we evaluate before we set en to be 1
        So after we have paused for 3 cycles, if we set en to be 1, this will only be evaluated on the next cycle
        Leading to another cycle delay before the count increments again. So we account for this delay by checking pause_cycles - 1
        */
        // if (top -> count == 0x9) {
        //     top -> en = !(pause_cycles - 1 > 0);
        //     pause_cycles--;
        // }
        // else {
        //     top -> en = 1;
        // }

        // test 3: reset whenever the number hits 0x5 to test the async reset
        // so during the cycle when we dump 4, after we dump clk = 0 and set top -> clk = 1 and evaluate,
        // we have top -> count = 0x5. We then set top -> rst = 1 over here.
        // then on the next cycle, we will dump 0x5 and clk = 1, and we set top -> clk = 0.
        // when we evaluate, because top -> rst is now 1, we immediately set top -> count = 0.
        // so we will dump clk = 0 and count = 0, i.e. we expect to see count go to 0 on the falling edge of clk,
        // instead of rising edge as in the synchronous case.
        // it will then stay at 0 for another cycle since top -> rst is still 1
        // so all in all, the count will stay at 0 for 1.5 periods of clk, before it goes up to 1 again.

        if (top -> count == 0x5) {
            top -> rst = 1;
        }
        else {
            top -> rst = 0;
        }


        if (Verilated::gotFinish()) exit(0);

    }

    tfp -> close();
    exit(0);
}