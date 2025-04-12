#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {

    int binds(sol::state_view &lua, sol::metatable &readonly_metatable) {

        bind_enums(lua, readonly_metatable);
        bind_common(lua, readonly_metatable);
        bind_params(lua, readonly_metatable);
        bind_profile(lua, readonly_metatable);
        bind_types(lua, readonly_metatable);
        bind_data(lua, readonly_metatable);
        bind_services(lua, readonly_metatable);

        return ::ok;

    }

}