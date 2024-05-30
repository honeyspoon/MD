module;

#include <chrono>
#include <ctime>
#include <format>
#include <iostream>
#include <string_view>

export module mlog;

import std;

export namespace mlog {

enum class LogLevel { INFO, WARN, ERROR };

template <typename... Args>
inline void log(LogLevel level, std::string_view fmt, Args&&... args) {
  std::string level_str;
  std::string color;

  switch (level) {
    case LogLevel::INFO:
      level_str = "INFO";
      color = "\033[32m";  // Green
      break;
    case LogLevel::WARN:
      level_str = "WARN";
      color = "\033[33m";  // Yellow
      break;
    case LogLevel::ERROR:
      level_str = "ERROR";
      color = "\033[31m";  // Red
      break;
  }
  std::string RESET = "\033[0m";

  auto now = std::chrono::system_clock::now();
  auto now_time_t = std::chrono::system_clock::to_time_t(now);
  auto now_tm = *std::localtime(&now_time_t);
  std::string timestamp(20, '\0');
  std::strftime(timestamp.data(), timestamp.size(), "%Y-%m-%d %H:%M:%S",
                &now_tm);

  auto args_tuple = std::make_format_args(args...);
  std::string message = std::vformat(fmt, args_tuple);

  std::cerr << timestamp << " - " << color << level_str << RESET << " - "
            << message << std::endl;
}

template <typename... Args>
inline void info(std::string_view fmt, Args&&... args) {
  log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(std::string_view fmt, Args&&... args) {
  log(LogLevel::WARN, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(std::string_view fmt, Args&&... args) {
  log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
}

}  // namespace mlog
