#pragma once

template <typename T, typename... Args>
void print(const T &value, const Args &...args)
{
    std::cout << "" << value << " ";
    ((std::cout << args << " "), ...);
    std::cout << std::endl;
}