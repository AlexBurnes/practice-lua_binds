#include <cxxopts.hpp>
#include <fmt/core.h>
#include <iostream>
#include <fstream>
#include <memory>

#include "typedef.h"
#include "log/log.hpp"
#include "script.hpp"

#define PROJECT_VERSION "0.1.0"

int main(const int argc, const char *argv[]) {

    lg::log_init(lg::Levels::ANY, 1, 0, true);

    std::string script_dir = std::getenv("LUACPP_SCRIPT_DIR")
        ? std::getenv("LUACPP_SCRIPT_DIR")
        : "var/lua/"
    ;

    cxxopts::Options options("luacpp", "luacpp " PROJECT_VERSION "Lua C++ binding example application");

    options.custom_help("[OPTIONS]");
    options.positional_help("script.lua");
    options.add_options()
        ("h,help",      "Print help and exit")
        ("v,version",   "Print version and exit")
        ("d,directory", "Lua scripts directory", cxxopts::value<std::string>()->default_value(script_dir))
        ("c,coverage",  "Print script test coverage")
        ("t,test",      "Testing script")
        ("script",      "Lua script to run", cxxopts::value<std::string>())
    ;
    options.parse_positional({"script"});
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        fmt::print("{}", options.help());
        fmt::print("Environment:\n");
        fmt::print("\tLUACPP_SCRIPT_DIR  {}\n",
            std::getenv("LUACPP_SCRIPT_DIR")
                ? std::getenv("LUACPP_SCRIPT_DIR")
                : ""
        );
        return success;
    }

    if (result.count("version")) {
        lg(1, "luacpp " PROJECT_VERSION);
        return success;
    }

    if (result.count("directory")) {
        script_dir = result["directory"].as<std::string>();
    }

    if (result.count("script") == 0) {
        le("not enouph arguments, see help -h|--help");
        return failure;
    }

    bool test = false;
    bool coverage = false;

    if (result.count("test")) {
        test = true;
    }

    if (result.count("coverage")) {
        coverage = true;
    }

    auto script_file = result["script"].as<std::string>();
    unless (script_load(script_file, script_dir, test, coverage)) {
        le("failed load and run script {}", script_file);
    }

    lr("{} done", script_file);

    return success;

}
