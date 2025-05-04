#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {
    int bind_common(sol::state_view &lua, sol::metatable& readonly_metatable) {

        // set log and debug levels for lua
        lua["log_level"]   = 3;
        lua["debug_level"] = 1;

        // declare log function lg
        lua["lg_"]=[](const char *msg){
            lg(0, "{}", msg);
        };

        // declare debug function ld
        lua["ld_"]=[](const char *msg){
            lg(0, "{yellow}{}", msg);
        };

        // declare warning function lw
        lua["lw_"]=[](const char *msg){
            lw("{}", msg);
        };

        // declare error function le
        lua["le_"]=[](const char *msg){
            le("{}", msg);
        };

        lua.new_usertype<utils::lvs>("lvs");
        lua["lvs"]["func"] = [](utils::lvs& self, int level, bool last) {
            return self.str(level, last);
        };

        return ok;
    }
}
