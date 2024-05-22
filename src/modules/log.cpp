export module log;

import std;

export template <typename T, typename... Args>
void println(T arg, Args... args) {
  std::cout << arg;
  ((std::cout << ' ' << args), ...);
  std::cout << std::endl;
}

// TODO: remove once std::print work on arm
