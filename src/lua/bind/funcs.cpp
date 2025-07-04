#include <sol/sol.hpp>
#include "lua/bind.hpp"
#include "lua/buildin.hpp"

class test_some {
    public:
        std::string test;
};

int some_function(test_some& some, std::string value) {
        some.test = value;
        return ok;
}

namespace Lua {
    int bind_functions(sol::state_view &lua) {
        lua.new_usertype<test_some>("some");
        lua["some_function"] = &some_function;
        return ok;
    }
}
