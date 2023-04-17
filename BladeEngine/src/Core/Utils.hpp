#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace BladeEngine {
class Utils {
public:
  static std::vector<char> ReadFile_vec(const std::string &filename);
  static std::string ReadFile_str(const std::string &filename);
};
} // namespace BladeEngine
