`timescale 1ps / 1ps

module json_testbench_server( clock, reset, io_a, io_b, io_e, io_v, io_z, done);

input wire clock;
   
input wire io_v ;
input wire [15:0] io_z ;
output reg io_e ;
output reg [15:0] io_b ;
output reg [15:0] io_a ;
output reg reset ;
output reg done ;
   

import "DPI-C" function void bench(output int reset,
                                   output int io_a,
                                   output int io_b,
                                   output int io_e,
                                   input int clock,
                                   input int  io_z,
                                   input int  io_v);

import "DPI-C" function int bench_done();

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
   if ( reset) begin
      done <= 1'b0;
   end
   else begin
      if ( bench_done()) begin
         done <= 1'b1;
      end
   end

   bench( _reset, _io_a, _io_b, _io_e, _clock, _io_z, _io_v);
   reset <= _reset;
   io_a <= _io_a;
   io_b <= _io_b;
   io_e <= _io_e;
end

endmodule
