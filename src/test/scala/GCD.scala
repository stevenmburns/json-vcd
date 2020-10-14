package json_vcd

import org.scalatest.{ Matchers, FlatSpec, GivenWhenThen}

import chisel3._
import chisel3.util._
import chisel3.iotesters._

import scala.io.Source

import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.module.scala.DefaultScalaModule
import com.fasterxml.jackson.module.scala.experimental.ScalaObjectMapper

class Tester( factory: () => GCDInner) extends GenericTest {
  behavior of s"GCDInner"
  it should "compile and execute without expect violations" in {
    chisel3.iotesters.Driver.execute( factory, optionsManager) { c =>
      new PeekPokeTester(c) {
        poke( c.io.a, 360)
        poke( c.io.b, 27)
        poke( c.io.e, 1)
        step(1)
        poke( c.io.e, 0)
        while ( peek( c.io.v) == 0) {
          step(1)
        }
        expect( c.io.z, 9)
      }
    } should be (true)
  }
}
class Test extends Tester( () => new GCDInner)

class JsonTester( factory: () => GCDInner, fn : String) extends GenericTest {
  behavior of s"GCDInner"
  it should "compile and execute without expect violations" in {
    chisel3.iotesters.Driver.execute( factory, optionsManager) { c =>
      new PeekPokeTester(c) {

        val bufferedSource = Source.fromFile(fn)
        val mapper = new ObjectMapper() with ScalaObjectMapper
        mapper.registerModule(DefaultScalaModule)
        val m = mapper.readValue[Seq[Map[String,Any]]](bufferedSource.reader())
        bufferedSource.close

        var reset_state = Option.empty[BigInt]


        for { mm <- m} {
          val t = mm("time") match {
            case t : Integer => t
            case _ => throw new Exception()
          }

          var reset_ = Option.empty[BigInt]
          var clock = Option.empty[BigInt]
          var io_a = Option.empty[BigInt]
          var io_b = Option.empty[BigInt]
          var io_e = Option.empty[BigInt]
          var io_z = Option.empty[BigInt]
          var io_v = Option.empty[BigInt]

          mm("changes") match {
            case c : Seq[Map[String,String]] =>
              for { mmm <- c} {
                mmm match {
                  case q : Map[String,String] =>
                    val variable : String = q("variable")
                    val value : String = q("value")

                    if ( variable == "reset") {
                      reset_ = Some(BigInt(value,2))
                      reset_state = Some(BigInt(value,2))
                    }
                    if ( variable == "clock") {
                      clock = Some(BigInt(value,2))
                    }
                    if ( variable == "io_a") {
                      io_a = Some(BigInt(value,2))
                    }
                    if ( variable == "io_b") {
                      io_b = Some(BigInt(value,2))
                    }
                    if ( variable == "io_e") {
                      io_e = Some(BigInt(value,2))
                    }
                    if ( variable == "io_z") {
                      io_z = Some(BigInt(value,2))
                    }
                    if ( variable == "io_v") {
                      io_v = Some(BigInt(value,2))
                    }

                  case _ => throw new Exception()
                }
              }
            case _ => throw new Exception()
          }

          assert(t % 1000 == 0)

          if ( clock.get == 1) {
            assert( t % 2000 == 0) 
            step(1)

            for { r <- reset_state} {
              if ( r == 0) {
                for { x <- io_v} { expect( c.io.v, x)}
                for { x <- io_z} { expect( c.io.z, x)}
              }
            }

          } else {

            assert( t % 2000 == 1000) 

            for { x <- reset_} { if ( x == 1) reset() }
            for { x <- io_a} { poke( c.io.a, x) }
            for { x <- io_b} { poke( c.io.b, x) }
            for { x <- io_e} { poke( c.io.e, x) }

          }
        }
      }
    } should be (true)
  }
}
class JsonTest extends JsonTester( () => new GCDInner, "out.json")

