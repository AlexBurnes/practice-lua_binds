#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {

    int bind_enums(sol::state_view &lua, sol::metatable& readonly_metatable) {
        //FIXME declare enums, but some steering values are not enums :) a just struct with static numbers
        //lua.new_enum<my_enum>( "my_enum", { {"a", my_enum:: a}, { "b", my_enum::b } } );.

        return ok;
    }
}
