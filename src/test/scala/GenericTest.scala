package json_vcd

import org.scalatest.{ Matchers, FlatSpec, GivenWhenThen}

import chisel3._
import chisel3.util._
import chisel3.iotesters._

import scala.io.Source

class GenericTest extends FlatSpec with Matchers {
  val extra = Seq("-sverilog -notice /nfs/site/disks/scl.work.50/ash/users/smburns/json-vcd/dpi_bench.o")

  val optionsManager = new TesterOptionsManager {
    testerOptions = testerOptions.copy(
//      backendName="treadle"
//      backendName="verilator"
      backendName="vcs"
      ,isVerbose=true
      ,moreVcsFlags=extra
//      ,vcsCommandEdits="""s/\+vcs\+initreg\+random //"""
    )
  }
}
