
package json_vcd

import chisel3._
import chisel3.util._

class json_testbench_server extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val clock = Input(Clock())
    val reset = Output(Bool())
    val io_a = Output(UInt(16.W))
    val io_b = Output(UInt(16.W))
    val io_e = Output(Bool())
    val io_v = Input(Bool())
    val io_z = Input(UInt(16.W))
    val done = Output(Bool())
  })
  setResource("/verilog/json_testbench_server.v")
}

class GCDtop extends Module {
  val io = IO(new Bundle {
    val done = Output(Bool())
  })

  val tb = Module(new json_testbench_server)
  val dut = Module(new GCDInner)

  io.done := tb.io.done
  tb.io.clock := clock
  dut.reset := tb.io.reset
  dut.io.a := tb.io.io_a
  dut.io.b := tb.io.io_b
  dut.io.e := tb.io.io_e
  tb.io.io_v := dut.io.v
  tb.io.io_z := dut.io.z
}
