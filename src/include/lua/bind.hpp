#pragma once

#include <sol/sol.hpp>
#include "types.hpp"

namespace Lua {

    //!@fn bind all other binds
    int binds(sol::state_view &lua, sol::metatable &readonly_metatable);

    //!@fn bind common functions
    int bind_common(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind enums defined it types.hpp
    int bind_enums(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind entity structures in types.hpp
    int bind_types(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind data
    int bind_data(sol::state_view &lua, sol::metatable& readonly_metatable);

    //!@fn bind buildin functions
    int bind_functions(sol::state_view &lua);

}


