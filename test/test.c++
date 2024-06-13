#include "envparse/parse.h++"

#include <iostream>

auto main(int, char* argv[]) -> int {
    std::cout << envparse::parse<envparse::not_set_option::left_blank>(argv[1])
              << '\n'
              << envparse::parse(argv[1]) << '\n';
}
