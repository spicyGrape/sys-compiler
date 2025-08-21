#pragma once
#include <string>
#include <vector>
class Assembly {
public:
  Assembly() = default;
  std::vector<std::string> globalIdnet;
  std::vector<std::string> text;
  std::vector<std::string> assemblyPiece;
};
