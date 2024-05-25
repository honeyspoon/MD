module;

#include <map>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <array>

export module book;

export namespace book {

struct Level {
  int price;
  int volume;
};

struct Order {
  int price;
  int volume;
  uint8_t side; // 0 = bid, 1 = ask
};

struct OrderBook {
  std::map<int, Level> bids;
  std::map<int, Level> asks;
  std::unordered_map<uint64_t, Order> orders;
};

  
void print_book(const OrderBook& book) {
    std::cout << "Bids:\n";
    std::cout << std::setw(10) << "Price" << std::setw(10) << "Volume" << '\n';
    for (const auto& [price, level] : book.bids) {
        std::cout << std::setw(10) << price << std::setw(10) << level.volume << '\n';
    }

    std::cout << "\nAsks:\n";
    std::cout << std::setw(10) << "Price" << std::setw(10) << "Volume" << '\n';
    for (const auto& [price, level] : book.asks) {
        std::cout << std::setw(10) << price << std::setw(10) << level.volume << '\n';
    }
}


} // namespace book
