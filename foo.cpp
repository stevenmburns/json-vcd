#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace nlohmann;


int main()
{
  
  std::string filename = "x.json";
  std::ifstream ifs(filename);
  if (ifs.fail()) {
    std::cout << "Cannot open file: " << filename << " for reading." << std::endl;
    return 1;
  }
  auto j = json::parse(ifs);

  std::cout << "Type of j: " << typeid(j).name() << std::endl;

  ifs.close();

  for (json::iterator it = j.begin(); it != j.end(); ++it) {
    std::cout << "Time: " << it->at("time") << std::endl;

    auto &j2 = it->at("changes");
    for (json::iterator it2 = j2.begin(); it2 != j2.end(); ++it2) {
      std::cout << "\t" << it2->at("variable") << " " << it2->at("value") << std::endl;
    }
  }


  return 0;
}
  
