#include <sol/sol.hpp>
#include "lua/bind.hpp"
#include "lua/buildin.hpp"

namespace Lua {
    int bind_functions(sol::state_view &lua) {
        //FIXME make a table buildin
        lua["relay_"]                    = Lua::relay;
        lua["send_relay_"]               = Lua::send_relay;
        lua["reject_"]                   = Lua::reject;
        lua["vlr_search_"]               = Lua::vlr_search;
        lua["mcc_search_"]               = Lua::mcc_search;
        lua["save_"]                     = Lua::save;
        lua["profile_lock_and_get_"]     = Lua::profile_lock_and_get;
        lua["check_session_out_"]        = Lua::check_session_out;
        lua["time_to_string_"]           = Lua::time_to_string;
        lua["local_time_"]               = Lua::local_time;
        lua["imsi_white_by_prefix_"]     = Lua::imsi_white_by_prefix;
        lua["imsi_white_by_range_"]      = Lua::imsi_white_by_range;
        lua["vlr_white_"]                = Lua::vlr_white;
        lua["load_steer_data_"]          = Lua::load_steer_data;
        lua["lbs_send_request_"]         = Lua::lbs_send_request;
        lua["border_save_"]              = Lua::border_save;
        lua["border_search_locations_"]  = Lua::border_search_locations;
        lua["rule_contain_locations_"]   = Lua::rule_contain_locations;
        lua["rule_contain_imsi_groups_"] = Lua::rule_contain_imsi_groups;
        lua["imsi_groups_intersection_"] = Lua::imsi_groups_intersection;
        lua["border_priority_rule_"]     = Lua::border_find_high_priority_rule;
        lua["border_filters_create_"]    = Lua::border_rule_filters_create;
        return ok;
    }
}