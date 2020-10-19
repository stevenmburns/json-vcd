#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

#include <set>
#include <map>
#include <optional>

//#include "svdpi.h"

using namespace nlohmann;

class Bench {
public:

  bool _first;

  json j;
  json::iterator it;

  int state_reset;
  std::map<std::string,std::optional<unsigned int> > state_values;

  Bench() : _first(true) {
    std::string filename = "/nfs/site/disks/scl.work.50/ash/users/smburns/json-vcd/out.json";
    std::ifstream ifs(filename);
    if (ifs.fail()) {
      std::cout << "Cannot open file: " << filename << " for reading." << std::endl;
      assert(false);
    } else {
      j = json::parse(ifs);
      ifs.close();
      it = j.begin();
    }

    state_values.insert( std::make_pair("io_z", std::optional<unsigned int>()));
    state_values.insert( std::make_pair("io_v", std::optional<unsigned int>()));
  }

  bool done() {
    return it == j.end();
  }

  void process_time( int& reset, int& io_a, int& io_b, int& io_e, int clock, int io_z, int io_v) {
    int time = it->at("time");
    std::cout << "Time: " << time << std::endl;


    bool reset_change = false;

    std::optional<unsigned int> optClock;

    std::map<std::string,int*> output_values;
    output_values.insert(std::make_pair( "io_a", &io_a));
    output_values.insert(std::make_pair( "io_b", &io_b));
    output_values.insert(std::make_pair( "io_e", &io_e));

    std::map<std::string,int> input_values;
    input_values.insert(std::make_pair( "io_v", io_v));
    input_values.insert(std::make_pair( "io_z", io_z));

    const auto & j2 = it->at("changes");
    for (json::const_iterator it2 = j2.begin(); it2 != j2.end(); ++it2) {
      std::string var(it2->at("variable"));
      std::string val(it2->at("value"));

      std::cout << "\t" << var << " " << val << std::endl;
      if ( var == "reset") {
        reset = std::stoi(val,0,2);
        state_reset = reset;
        reset_change = true;
      }
      {
        auto it3 = output_values.find(var);
        if ( it3 != output_values.end()) {
          *(it3->second) = std::stoi(val,0,2);
        }
      }
      {
        auto it3 = state_values.find(var);
        if ( it3 != state_values.end()) {
          //Both of theses seem to work (doesn't make sense given below)
          //*(it3->second) = std::stoi(val,0,2);
          it3->second = std::optional<unsigned int>(std::stoi(val,0,2));
        }
      }
      if ( var == "clock") {
        //This doesn't work
        //*optClock = std::stoi(val,0,2);
        optClock = std::optional<unsigned int>(std::stoi(val,0,2));
      }

    }

    assert(optClock);

    // We want the first operation to be when clock is high
    if ( _first) {
      _first = false;
      if ( clock == 0 && *optClock == 1 || clock == 1 && *optClock == 0) {
        // If clock isn't not what is expected, wait until the next call
        return;
      }
    }

    assert( time % 1000 == 0);
    if ( clock == 1) {
      assert( time % 2000 == 0);
    } else {
      assert( time % 2000 == 1000);
    }

    if ( clock == 0 && state_reset == 0 && !reset_change) {
      for (auto it3 = input_values.begin(); it3 != input_values.end(); ++it3) {
        std::cout << "Checking " << it3->first << std::endl;
        assert( it3->second == *state_values[it3->first]);
      }
    }

    ++it;
  }


};

static Bench bench_object;

extern "C" {
  void bench( int *reset,
              int *io_a,
              int *io_b,
              int *io_e,
              int clock,
              int io_z,
              int io_v) {
    std::cout << "Clock: " << clock << std::endl;
    if ( !bench_object.done()) {
      bench_object.process_time(*reset,*io_a,*io_b,*io_e,clock,io_z,io_v);
    }
  }
}

extern "C" {
  int bench_done( int *reset) {
    return bench_object.done();
  }
}
               
