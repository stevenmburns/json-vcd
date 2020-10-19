`timescale 1ps / 1ps

module json_testbench_client();
wire io_v ;
wire [15:0] io_z ;
wire io_e ;
wire [15:0] io_b ;
wire [15:0] io_a ;
wire reset ;
reg clock ;
wire done;

initial begin
    clock = 1'b1;
    forever #1000  clock = ~ clock ;
end

json_testbench_server json_testbench_server( .clock(clock),
                                             .reset(reset),
                                             .io_a(io_a),
                                             .io_b(io_b),
                                             .io_e(io_e),
                                             .io_z(io_z),
                                             .io_v(io_v),
                                             .done(done));

always_ff @(posedge(clock)) begin
   if (done) begin
      $finish;
   end
end

GCDInner GCDInner( .clock(clock),
                   .reset(reset),
                   .io_a(io_a),
                   .io_b(io_b),
                   .io_e(io_e),
                   .io_z(io_z),
                   .io_v(io_v));

endmodule
