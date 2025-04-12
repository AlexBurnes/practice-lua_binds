#include <fmt/format.h>
#include <sol/sol.hpp>
#include <chrono>

class Bench {
    std::chrono::high_resolution_clock::time_point start, end;
    const std::string what;
    const size_t records;
    public:
        Bench(std::string_view what, const size_t records)
        : 
            start{std::chrono::high_resolution_clock::now()},
            what{what}, 
            records{records}
        {}

        ~Bench() {
            end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
            fmt::print("{} {} in {}s avg {}ns {}r/s\n",
                what, records, duration, (duration / records) * 1e9, 1 / (duration / records)
            );
        }
};

//https://sol2.readthedocs.io/en/latest/exceptions.html
int exception_handler(
    lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description
) {
    if (maybe_exception) {
        const std::exception& ex = *maybe_exception;
        fmt::print("An exception occurred, exception '{}'\n", ex.what());
    }
    else {
        std::string desc{description.data(), description.size()};
        fmt::print("An exception occurred, description '{}'\n", desc);
    }
    return sol::stack::push(L, description);
}

int check_script(sol::protected_function_result load_result) {
    if (not load_result.valid()) {
        sol::error err = load_result;
        fmt::print("failed load error: {}\n", err.what());
        return 1;
    }
    return 0;
}

int main() {
    uint tests = 1e6;
    sol::state lua;
    lua.open_libraries(
        sol::lib::base, sol::lib::jit, sol::lib::string, sol::lib::coroutine, sol::lib::math, sol::lib::os
    );
    lua.set_exception_handler(&exception_handler);
    lua.set_function("throw", [](){throw std::runtime_error("exception!");});
    lua["_SOL_VERSION"]=SOL_VERSION_STRING;
    auto load_result = lua.script(R"(
        local jit_version
        if type(jit) == 'table' then jit_version = jit.version else jit_version = "not loaded" end
        return "lua ".._VERSION..", sol2 ".._SOL_VERSION..", jit "..jit_version
    )");
    std::string full_version = load_result;
    fmt::print("loaded {}\n", full_version);

    lua["lg_"] = [](const uint level, const char *msg){
        fmt::print("{}\n", msg);
    };
    check_script(lua.script(R"(
        function lg(level, fmt, ...)
            string.format(fmt, ...)
        end
    )"));

    lua["log_quiet"] = [](const bool quiet)->void {};
    auto add = [](const uint a, const uint b)->uint {
        return a + b + b + b;
    };
    lua["add_cpp"] = add;
    lua["loop_cpp"] = [add](const uint tests)->uint {
        uint a = 0;
        for (auto i = 0; i < tests; ++i)  {
            a = add(a, 1);
        }
        return a;
    };
    check_script(lua.script(R"(
        function test_add(tests)
            add = function (a,b)
                return a + b + b + b
            end
            local y = os.clock()
            local a = 0
            for i = 1, tests do
                a = add(a, 1)
            end
            lg(1, "tests %d, a = %d, elapsed time: %.2f", tests, a, os.clock() - y)
            return
        end
    )"));
    check_script(lua.script(R"(
        function test_add_cpp(tests)
            local y = os.clock()
            local a = 0
            for i = 1, tests do
                a = add_cpp(a, 1)
            end
            lg(1, "tests %d, a = %d, elapsed time: %.2f", tests, a, os.clock() - y)
            return
        end
    )"));
    check_script(lua.script(R"(
        function test_cpp(tests)
            local y = os.clock()
            local a = loop_cpp(tests);
            lg(1, "tests %d, a = %d, elapsed time: %.2f", tests, a, os.clock() - y)
            return
        end
    )"));
    check_script(lua.script(R"(
        function test_log(tests)
            --log_quiet(true)
            local y = os.clock()
            for i = 1, tests do
                lg(1, "test [%d]", i);
            end
            --log_quiet(false)
            lg(1, "tests %d, elapsed time: %.2f", tests, os.clock() - y)
            return
        end
    )"));
    fmt::print("bench lua add()");
    {
        Bench bench("add", tests);
        auto function = lua["test_add"];
        function(tests);
    }
    fmt::print("bench lua add_cpp()");
    {
        Bench bench("add_cpp", tests);
        auto function = lua["test_add_cpp"];
        function(tests);
    }
    fmt::print("bench lua test_cpp()");
    {
        Bench bench("test_cpp", tests);
        auto function = lua["test_cpp"];
        function(tests);
    }
    fmt::print("bench lua test_log()");
    {
        Bench bench("test_log", tests);
        auto function = lua["test_log"];
        function(tests);
    }
    fmt::print( "done");
    return 0;
}
