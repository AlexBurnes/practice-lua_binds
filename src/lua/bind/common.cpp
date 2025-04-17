#include <sol/sol.hpp>
#include "lua/bind.hpp"
#include "context.hpp"

namespace Lua {
    int bind_common(sol::state_view &lua, sol::metatable& readonly_metatable) {

        // set log and debug levels for lua
        lua["log_level"]   = 3;
        lua["debug_level"] = 1;

        // declare log function lg
        lua["lg_"]=[](location_update_context_ptr &ctx, const char *msg){
            lg(0, "[{}] [imsi:{}] [{}] {}",
                ctx->sid, ctx->imsi, ctx->logic_session, msg
            );
        };

        // declare debug function ld
        lua["ld_"]=[](location_update_context_ptr &ctx, const char *msg){
            lg(0, "{yellow}[{}] [imsi:{}] [{}] {}",
                ctx->sid, ctx->imsi, ctx->logic_session, msg
            );
        };

        // declare warning function lw
        lua["lw_"]=[](location_update_context_ptr &ctx, const char *msg){
            lw("[{}] [imsi:{}] [{}] {}",
                ctx->sid, ctx->imsi, ctx->logic_session, msg
            );
        };

        // declare error function le
        lua["le_"]=[](location_update_context_ptr &ctx, const char *msg){
            le("[{}] [imsi:{}] [{}] {}",
                ctx->sid, ctx->imsi, ctx->logic_session, msg
            );
        };

        lua.new_usertype<utils::lvs>("lvs");
        lua["lvs"]["func"] = [](utils::lvs& self, int level, bool last) {
            return self.str(level, last);
        };

        return ok;
    }
}
