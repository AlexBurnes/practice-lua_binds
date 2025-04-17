#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {
    int bind_types(sol::state_view &lua, sol::metatable& readonly_metatable) {

        //FIXME declare all required user data LocationUpdateContext
        // ok here is an working and right example of declare references to class members
        // https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_advanced.cpp#L81
        auto timeval_context = lua.new_usertype<timespec_t>("timespec_t");
        timeval_context.set("sec", &timespec_t::tv_sec);
        timeval_context.set("nsec", &timespec_t::tv_nsec);


        return ok;
    }
}
