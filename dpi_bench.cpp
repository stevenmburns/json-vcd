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

  int _bad;
  json j;
  json::iterator it;

  int state_reset;
  std::map<std::string,std::optional<unsigned int> > state_values;



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

    state_values.insert( std::make_pair("io_z", std::optional<unsigned int>()));
    state_values.insert( std::make_pair("io_v", std::optional<unsigned int>()));

  }

  bool done() {
    return _bad || it == j.end();
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

    std::map<std::string,int*> input_values;
    input_values.insert(std::make_pair( "io_a", &io_a));
    input_values.insert(std::make_pair( "io_b", &io_b));
    input_values.insert(std::make_pair( "io_e", &io_e));

    std::map<std::string,int> output_values;
    output_values.insert(std::make_pair( "io_v", io_v));
    output_values.insert(std::make_pair( "io_z", io_z));


    const auto & j2 = it->at("changes");
    for (json::const_iterator it2 = j2.begin(); it2 != j2.end(); ++it2) {
      std::cout << "\t" << it2->at("variable") << " " << it2->at("value") << std::endl;
      std::string val(it2->at("value"));
      if ( it2->at("variable") == "reset") {
        reset = std::stoi(val,0,2);
        state_reset = reset;
        reset_change = true;
      }
      {
        auto it3 = input_values.find(it2->at("variable"));
        if ( it3 != input_values.end()) {
          *(it3->second) = std::stoi(val,0,2);
        }
      }
      {
        auto it3 = state_values.find(it2->at("variable"));
        if ( it3 != state_values.end()) {
          *(it3->second) = std::stoi(val,0,2);
        }
      }

    }

    if ( clock == 0 && state_reset == 0 && !reset_change) {
      for (auto it3 = output_values.begin(); it3 != output_values.end(); ++it3) {
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
               
