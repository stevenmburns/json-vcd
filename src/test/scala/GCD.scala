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

        val BigZero = BigInt(0)

        val bufferedSource = Source.fromFile(fn)
        val mapper = new ObjectMapper() with ScalaObjectMapper
        mapper.registerModule(DefaultScalaModule)
        val m = mapper.readValue[Seq[Map[String,Any]]](bufferedSource.reader())
        bufferedSource.close

        val state_map = scala.collection.mutable.Map.empty[String,BigInt]

        val input_map = scala.collection.mutable.Map.empty[String,UInt]
        val output_map = scala.collection.mutable.Map.empty[String,UInt]


        input_map("io_a") = c.io.a
        input_map("io_b") = c.io.b
        input_map("io_e") = c.io.e
        output_map("io_z") = c.io.z
        output_map("io_v") = c.io.v

        for { mm <- m} {
          val t = mm("time") match {
            case t : Integer => t
            case _ => throw new Exception()
          }

          var reset_ = Option.empty[BigInt]
          var clock = Option.empty[BigInt]

          val inputs = scala.collection.mutable.ArrayBuffer.empty[(String,UInt,BigInt)]

          mm("changes") match {
            case c : Seq[Map[String,String]] =>
              for { mmm <- c} {
                mmm match {
                  case q : Map[String,String] =>
                    val variable : String = q("variable")
                    val value : String = q("value")
                    if ( variable == "reset") {
                      reset_ = Some(BigInt(value,2))
                      state_map("reset") = BigInt(value,2)
                      println(s"reset changed to $value")
                    } else if ( variable == "clock") {
                      clock = Some(BigInt(value,2))
                    } else if ( input_map.contains(variable)) {
                      val t = (variable,input_map(variable),BigInt(value,2))
                      inputs += t
                    } else if ( output_map.contains(variable)) {
                      state_map(variable) = BigInt(value,2)
                    } else {
                      throw new Exception
                    }
                  case _ => throw new Exception()
                }
              }
            case _ => throw new Exception()
          }

          assert(t % 1000 == 0)

          assert( clock.isDefined)

          def value_is_zero( x : Option[BigInt]) =
            x match {
              case Some(BigZero) => true
              case _ => false
            }

          if ( clock.get == 1) {
            assert( t % 2000 == 0) 
            if (value_is_zero( state_map.get("reset"))) {
              step(1)
            } else {
              reset()
            }
          } else {
            assert( t % 2000 == 1000) 
          }

          if ( value_is_zero( state_map.get("reset")) &&
               !value_is_zero( reset_)) {
              for { (k,v) <- state_map } {
                if ( k != "reset") {
                  expect( output_map(k), v)
                }
              }
          }

          for { (k,d,v) <- inputs} {
            poke( d, v)
          }

        }
      }
    } should be (true)
  }
}
class JsonTest extends JsonTester( () => new GCDInner, "out.json")

