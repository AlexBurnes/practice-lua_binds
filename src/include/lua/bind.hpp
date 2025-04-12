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
    //!@fn bind common parameters
    int bind_params(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind profile and context
    int bind_profile(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind entity structures in types.hpp
    int bind_types(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind steer data
    int bind_data(sol::state_view &lua, sol::metatable& readonly_metatable);
    //!@fn bind protolol structure and enums
    int bind_services(sol::state_view &lua, sol::metatable& readonly_metatable);

    //!@fn bind buildin functions
    int bind_functions(sol::state_view &lua);

}


