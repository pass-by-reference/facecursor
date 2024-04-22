#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <chrono>
#include <iomanip>

namespace utils {

inline std::string get_now_formatted()
{
  const auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  const std::string format = "%m_%d-%H:%M";
  ss << std::put_time(std::localtime(&time), format.c_str());

  return ss.str();
}

} // namespace utils

#endif // UTILS_H_