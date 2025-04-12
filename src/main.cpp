#include <cxxopts.hpp>
#include <fmt/core.h>

int main(const int argc, const char *argv[]) {

    cxxopts::Options options("luacpp", "Lua C++ binding example application");

    options.custom_help("[OPTIONS] script.lua");
    options.add_options()
        ("h,help",      "Print help and exit")
        ("d,directory", "Lua scripts directory", cxxopts::value<std::string>()->default_value("./lua"))
        ("c,coverage",  "Print script test coverage")
        ("t,test",      "Testing script")
    ;
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        fmt::print("{}", options.help());
        exit(0);
    }

    fmt::print("done\n");

}