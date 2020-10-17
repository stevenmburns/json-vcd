#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

//#include "svdpi.h"

using namespace nlohmann;

class Bench {
public:

  int _bad;
  json j;
  json::iterator it;

  int state_reset;
  int state_io_v;
  int state_io_z;

  Bench() {
    _bad = 0;
    std::string filename = "out.json";
    std::ifstream ifs(filename);
    if (ifs.fail()) {
      std::cout << "Cannot open file: " << filename << " for reading." << std::endl;
      _bad = 1;
    } else {
      j = json::parse(ifs);
      ifs.close();
      it = j.begin();
    }
  }

  bool done() {
    return it == j.end();
  }

  void process_time( int& reset, int& io_a, int& io_b, int& io_e, int clock, int io_z, int io_v) {
    int time = it->at("time");
    std::cout << "Time: " << time << std::endl;

    assert( time % 1000 == 0);
    if ( clock == 1) {
      assert( time % 2000 == 0);
    } else {
      assert( time % 2000 == 1000);
    }


    bool reset_change = false;


    auto &j2 = it->at("changes");
    for (json::iterator it2 = j2.begin(); it2 != j2.end(); ++it2) {
      std::cout << "\t" << it2->at("variable") << " " << it2->at("value") << std::endl;
      std::string val(it2->at("value"));
      if ( it2->at("variable") == "reset") {
        reset = std::stoi(val,0,2);
        state_reset = reset;
        reset_change = true;
      }
      if ( it2->at("variable") == "io_a") {
        io_a = std::stoi(val,0,2);
      }
      if ( it2->at("variable") == "io_b") {
        io_b = std::stoi(val,0,2);
      }
      if ( it2->at("variable") == "io_e") {
        io_e = std::stoi(val,0,2);
      }
      if ( it2->at("variable") == "io_v") {
        state_io_v = std::stoi(val,0,2);
      }
      if ( it2->at("variable") == "io_z") {
        state_io_z = std::stoi(val,0,2);
      }
    }

    if ( clock == 0 && state_reset == 0 && !reset_change) {
      std::cout << "io_v: " << state_io_v << " " << io_v << std::endl;
      std::cout << "io_z: " << state_io_z << " " << io_z << std::endl;
      assert( state_io_v == io_v);
      assert( state_io_z == io_z);
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
               
#if 0
{
  

  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    std::cout << "Time: " << it->at("time") << std::endl;

    auto &j2 = it->at("changes");
    for (json::iterator it2 = j2.begin(); it2 != j2.end(); ++it2) {
      std::cout << "\t" << it2->at("variable") << " " << it2->at("value") << std::endl;
    }
  }


  return 0;
}
#endif  
