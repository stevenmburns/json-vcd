package json_vcd

import org.scalatest.{ Matchers, FlatSpec, GivenWhenThen}

import chisel3._
import chisel3.util._
import chisel3.iotesters._

import scala.io.Source

class TopTester( factory: () => GCDtop) extends GenericTest {
  behavior of s"GCDInner"
  it should "compile and execute without expect violations" in {
    chisel3.iotesters.Driver.execute( factory, optionsManager) { c =>
      new PeekPokeTester(c) {
        val max_iter = 1000
        var iter = 0
        while ( peek(c.io.done) == 0 && iter < max_iter) {
          step(1)
          iter += 1
        }
      }
    } should be (true)
  }
}
class TopTest extends TopTester( () => new GCDtop)

