`timescale 1ps / 1ps

module tbench_GCDInner ;
wire io_v ;
wire [15:0] io_z ;
reg io_e ;
reg [15:0] io_b ;
reg [15:0] io_a ;
reg reset ;
reg clock ;
initial #45002 $finish;
initial $dumpvars;
initial begin
    #0  io_e = 1'b0;
    #11000  io_e = 1'b1;
    #2000  io_e = 1'b0;
end
initial begin
    #0  io_b = 16'b0000000000000000;
    #11000  io_b = 16'b0000000000011011;
end
initial begin
    #0  io_a = 16'b0000000000000000;
    #11000  io_a = 16'b0000000101101000;
end
initial begin
    #0  reset = 1'b1;
    #11000  reset = 1'b0;
end
initial begin
     clock = 1'b1;
    forever #1000  clock = ~ clock ;
end
GCDInner GCDInner (clock,reset,io_a,io_b,io_e,io_z,io_v);
endmodule
