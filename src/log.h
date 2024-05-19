#ifndef LOG_H
#define LOG_H

#include <iostream>

template <typename T, typename... Args>
void print(const T &value, const Args &...args) {
  std::cout << value;
  using expander = int[];
  (void)expander{0, (void(std::cout << ' ' << args), 0)...};
  std::cout << std::endl;
}

#endif // LOG_H
