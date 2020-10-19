`timescale 1ps / 1ps

module json_testbench();
wire io_v ;
wire [15:0] io_z ;
reg io_e ;
reg [15:0] io_b ;
reg [15:0] io_a ;
reg reset ;
reg clock ;

initial $dumpvars;

initial begin
     clock = 1'b1;
    forever #1000  clock = ~ clock ;
end

import "DPI-C" function void bench(output int reset,
                                   output int io_a,
                                   output int io_b,
                                   output int io_e,
                                   input int clock,
                                   input int  io_z,
                                   input int  io_v);

int                                          _reset;
int                                          _io_a;   
int                                          _io_b;   
int                                          _io_e;   
int                                          _clock;
int                                          _io_z;   
int                                          _io_v;   


   assign _clock = clock;
   assign _io_z = io_z;   
   assign _io_v = io_v;   

always_ff @(clock) begin
   if ( bench_done()) begin
      $finish;
   end;

   bench( _reset, _io_a, _io_b, _io_e, _clock, _io_z, _io_v);
   reset <= _reset;
   io_a <= _io_a;
   io_b <= _io_b;
   io_e <= _io_e;
end

GCDInner GCDInner (clock,reset,io_a,io_b,io_e,io_z,io_v);
endmodule
