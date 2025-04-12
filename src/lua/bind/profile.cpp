#include <sol/sol.hpp>
#include "lua/bind.hpp"
#include "context.hpp"

namespace Lua {
    int bind_profile(sol::state_view &lua, sol::metatable& readonly_metatable) {

        auto logic_answer = lua.new_usertype<steering::logic_answer_t>("logic_answer_t");
        logic_answer.set("code",            &steering::logic_answer_t::code);
        logic_answer.set("text",            &steering::logic_answer_t::text);
        logic_answer.set("unix_time",       &steering::logic_answer_t::unix_time);
        logic_answer.set("logic_session",   &steering::logic_answer_t::logic_session);
        logic_answer.set("diagnostic_code", &steering::logic_answer_t::diagnostic_code);

        auto imsi_profile = lua.new_usertype<ImsiProfile>("ImsiProfile");
        imsi_profile.set("imsi"             , sol::readonly(&ImsiProfile::imsi        ));
        imsi_profile.set("network_type"     , sol::readonly(&ImsiProfile::network_type));
        imsi_profile.set("country_id"       , sol::readonly(&ImsiProfile::country_id  ));
        imsi_profile.set("attempt_time"     , sol::readonly(&ImsiProfile::attempt_time));

        imsi_profile.set("logic_session"    , &ImsiProfile::logic_session   );
        imsi_profile.set("state"            , &ImsiProfile::state           );
        imsi_profile.set("active"           , &ImsiProfile::active          );
        imsi_profile.set("network_attempt"  , &ImsiProfile::network_attempt );
        imsi_profile.set("network_attempts" , &ImsiProfile::network_attempts);
        imsi_profile.set("node_attempt"     , &ImsiProfile::node_attempt    );
        imsi_profile.set("first_time"       , &ImsiProfile::first_time      );
        imsi_profile.set("manual_suspect"   , &ImsiProfile::manual_suspect  );
        imsi_profile.set("manual_mode"      , &ImsiProfile::manual_mode     );
        imsi_profile.set("abnormal"         , &ImsiProfile::abnormal        );
        imsi_profile.set("cause_type"       , &ImsiProfile::cause_type      );
        imsi_profile.set("code"             , &ImsiProfile::code            );
        imsi_profile.set("foreign"          , &ImsiProfile::foreign         );
        imsi_profile.set("start_time"       , &ImsiProfile::start_time      );
        imsi_profile.set("relay_time"       , &ImsiProfile::relay_time      );
        imsi_profile.set("msisdn"           , &ImsiProfile::msisdn          );
        imsi_profile.set("node"             , &ImsiProfile::node            );
        imsi_profile.set("source_node"      , &ImsiProfile::source_node     );
        imsi_profile.set("dest_node"        , &ImsiProfile::dest_node       );
        imsi_profile.set("operator_id"      , &ImsiProfile::operator_id     );
        imsi_profile.set("node_id"          , &ImsiProfile::node_id         );
        imsi_profile.set("rule_id"          , &ImsiProfile::rule_id         );
        imsi_profile.set("rule_type"        , &ImsiProfile::rule_type       );
        imsi_profile.set("protocol_version" , &ImsiProfile::protocol_version);
        imsi_profile.set("imsi_group"       , &ImsiProfile::imsi_group      );

        auto imsi_profiles = lua.new_usertype<ImsiProfiles>("ImsiProfiles");
        imsi_profiles.set("profile"          , &ImsiProfiles::profile         );
        imsi_profiles.set("active_profile"   , &ImsiProfiles::active_profile  );
        imsi_profiles.set("prev_profile"     , &ImsiProfiles::prev_profile    );
        imsi_profiles.set("other_profiles"   , &ImsiProfiles::other_profiles  );
        imsi_profiles.set("lbs"              , &ImsiProfiles::lbs             );
        imsi_profiles.set("state"            , &ImsiProfiles::state           );
        imsi_profiles.set("attempt"          , &ImsiProfiles::attempt         );
        imsi_profiles.set("imsi_lists"       , &ImsiProfiles::imsi_lists      );
        imsi_profiles.set("prev_imsi_lists"  , &ImsiProfiles::prev_imsi_lists );
        imsi_profiles.set("imsi_volte"       , &ImsiProfiles::imsi_volte      );
        imsi_profiles.set("rule_id"          , &ImsiProfiles::rule_id         );

        auto location_update_context = lua.new_usertype<LocationUpdateContext>("LocationUpdateContext");
        location_update_context.set("l"                , sol::readonly(&LocationUpdateContext::l));
        location_update_context.set("d"                , sol::readonly(&LocationUpdateContext::d));
        location_update_context.set("imsi"             , sol::readonly(&LocationUpdateContext::imsi));
        location_update_context.set("sid"              , sol::readonly(&LocationUpdateContext::sid));
        location_update_context.set("attempt_time"     , sol::readonly(&LocationUpdateContext::attempt_time));
        location_update_context.set("event_time"       , sol::readonly(&LocationUpdateContext::event_time));
        location_update_context.set("registration_type", sol::readonly(&LocationUpdateContext::registration_type));
        location_update_context.set("node"             , sol::readonly(&LocationUpdateContext::node));
        location_update_context.set("source_node"      , sol::readonly(&LocationUpdateContext::source_node));
        location_update_context.set("dest_node"        , sol::readonly(&LocationUpdateContext::dest_node));
        location_update_context.set("mcc"              , sol::readonly(&LocationUpdateContext::mcc));
        location_update_context.set("mnc"              , sol::readonly(&LocationUpdateContext::mnc));
        location_update_context.set("protocol_version" , sol::readonly(&LocationUpdateContext::protocol_version));

        location_update_context.set("session_timeout"  , &LocationUpdateContext::session_timeout);
        location_update_context.set("attempt_timeout"  , &LocationUpdateContext::attempt_timeout);
        location_update_context.set("relay_timeout"    , &LocationUpdateContext::relay_timeout);
        location_update_context.set("request_timeout"  , &LocationUpdateContext::request_timeout);
        location_update_context.set("action_state"     , &LocationUpdateContext::action_state);
        location_update_context.set("network_type"     , &LocationUpdateContext::network_type);
        location_update_context.set("vlr_founded"      , &LocationUpdateContext::vlr_founded);
        location_update_context.set("foreign"          , &LocationUpdateContext::foreign);
        location_update_context.set("node_id"          , &LocationUpdateContext::node_id);
        location_update_context.set("country_id"       , &LocationUpdateContext::country_id);
        location_update_context.set("operator_id"      , &LocationUpdateContext::operator_id);
        location_update_context.set("logic_answers"    , &LocationUpdateContext::logic_answers);
        location_update_context.set("operator_rate"    , &LocationUpdateContext::operator_rate);
        location_update_context.set("msisdn"           , &LocationUpdateContext::msisdn);
        location_update_context.set("cause_type"       , &LocationUpdateContext::cause_type);
        location_update_context.set("cause_text"       , &LocationUpdateContext::cause_text);
        location_update_context.set("hlr_result"       , &LocationUpdateContext::hlr_result);
        location_update_context.set("node_changed"     , &LocationUpdateContext::node_changed);
        location_update_context.set("operator_changed" , &LocationUpdateContext::operator_changed);
        location_update_context.set("logic_session"    , &LocationUpdateContext::logic_session);
        location_update_context.set("attempt"          , &LocationUpdateContext::attempt);
        location_update_context.set("dur_session"      , &LocationUpdateContext::dur_session);
        location_update_context.set("profiles"         , &LocationUpdateContext::profiles);
        location_update_context.set("answer_type"      , &LocationUpdateContext::answer_type);
        location_update_context.set("vlr_white"        , &LocationUpdateContext::vlr_white);
        return ok;
    }
}