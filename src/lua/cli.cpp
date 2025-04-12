#include "lua/state.hpp"
#include "lua/bind.hpp"
#include "lua/module.hpp"
#include "context.hpp"

namespace Lua {

    int State::init(std::string script_code) {
        lua.open_libraries(
            sol::lib::base, sol::lib::jit, sol::lib::string, sol::lib::coroutine, sol::lib::math, sol::lib::os,
            sol::lib::table, sol::lib::io, sol::lib::bit32, sol::lib::ffi
        );
        lua.set_exception_handler(&exception_handler);

        lua.set_function("throw", [](){throw std::runtime_error("exception!");});
        lua.set_function("require", [this] (std::string module, std::string version) -> sol::table& {
            ld(1, "require module {} version {}", module, version);
            auto module_key = fmt::format("{}:{}", module, version);
            if (loaded_modules.count(module_key)) {
                lw("requied module {} version {} already loaded, check loop dependency", module, version);
                return loaded_modules[module_key];
            }
            auto module_code = Module.logic->GetCode(module, version);
            if (module_code == "") {
                le("required module {} version {} is not loaded into database", module, version);
                throw std::runtime_error("exception!");
            }
            auto load_result = lua.script(*module_code, [](lua_State*, sol::protected_function_result pfr){
                return pfr;
            });
            unless (load_result.valid()) {
                sol::error err = load_result;
                le("failed load module {}, error: {}", module, err.what());
                throw std::runtime_error("exception!");
            }
            try {
                loaded_modules[module_key] = load_result;
                return loaded_modules[module_key];
            }
            catch (...) {
                le("failed load lua module '{}', wrong return, expect lua module", module);
                throw std::runtime_error("exception!");
            }
        });

        unless (binds(lua, readonly_metatable)) {
            le("failed bind userdata structures");
            return error;
        }

        unless (load(script_code)) {
            return error;
        }

        unless (bind_functions(lua)) {
            le("failed bind userdata structures");
            return error;
        }


        return ok;
    }

    int relay(location_update_context_ptr &ctx, bool active, CauseType cause, std::string &&text) {
        return ok;
    }

    int reject(location_update_context_ptr &ctx, bool active, CauseType cause, std::string &&text) {
        return ok;
    }

    std::tuple<int, int, int, int> vlr_search(
        location_update_context_ptr &ctx, const NetworkType network_type, const std::string& node,
        const std::string& source, bool strict
    ) {
        lg(1, "vlr_search network type {} node {} source {}", +network_type, node, source);
        return {error, -1, -1, -1};
    };

    std::tuple<int, int, int, int> mcc_search(
        location_update_context_ptr &ctx, const std::string& mcc, const std::string& mnc,
        const std::string& mme_host, const std::string &mme_realm
    ) {
        lg(1, "mcc_search mcc {} mnc {} host {} realm {}", mcc, mnc, mme_host, mme_realm);
        return {error, -1, -1, -1};
    }

    int profile_lock_and_get(location_update_context_ptr &ctx) {
        //mock redis lock?
        return ok;
    }

    int send_relay(location_update_context_ptr &ctx, CauseType cause, std::string text) {
        return ok;
    };

    LogicReturn save(location_update_context_ptr &ctx, int result_code, timespec_t *event_time) {
        return LogicReturn::Ok;
    }

    LogicReturn check_session_out(location_update_context_ptr &ctx) {
        return LogicReturn::Ok;
    }

    time_t local_time(time_t time, int offset) {
        tm_t tm_utc;
        memset(&tm_utc, 0, sizeof(tm_t));
        gmtime_r((const time_t *)&time, &tm_utc);
        time_t time_utc = mktime(&tm_utc);
        return time_utc + offset;
    }

    std::string time_to_string(time_t time) {
        return utils::to_string(time);
    }

    sol::object imsi_white_by_prefix(
        struct SteeringData::Values& steer_data, const std::string& imsi,
        sol::this_state lua
    ) {
        return sol::nil;
    }

    sol::object imsi_white_by_range(
        struct SteeringData::Values& steer_data, const std::string& imsi,
        sol::this_state lua
    ) {
        return sol::nil;
    }

    sol::object vlr_white(
        vlr_white_ptr &vlr_white, struct SteeringData::Values& steer_data,
        const OperatorNodeType node_type, const std::string& node,
        sol::this_state lua
    ){
        return sol::nil;
    }

    // Load steer data from redis if exists
    SteeringData::Values& load_steer_data() {
        lg(1, "wait while steer data is initiated");
        lg::quiet(true);
        Module.cache = std::make_unique<lur::redis::entity::Cache>(
            Module.redis, Module.base_changes, Module.com_parameter_handlers
        );
        Module.cache->init_entities(SteeringData);
        lg::quiet(false);
        return SteeringData.values;
    }

    LogicReturn lbs_send_request(location_update_context_ptr &ctx, lbs_gw::ServiceType service) {
        return LogicReturn::Ok;
    }

    LogicReturn border_save(location_update_context_ptr &ctx, LBSCauseType cause_type) {
        return LogicReturn::Ok;
    }

    std::vector<int> border_search_locations(
        location_update_context_ptr &ctx, struct SteeringData::Values& steer_data,
        const std::string& lac, const std::string& cell, const std::string& eci
    ) {
        std::vector<int> found_locations;
        return found_locations;
    }

    bool rule_contain_locations(const hash_map<unsigned int, bool>& rule_locations, const std::vector<int>& locations) {
        return false;
    }

    bool rule_contain_imsi_groups(
        const std::vector<unsigned int>&rule_groups, const std::vector<group_id_t>& imsi_groups
    ) {
        return false;
    }

    group_id_t imsi_groups_intersection(
        const std::vector<unsigned int>&rule_groups, const std::vector<group_id_t>& imsi_groups
    ) {
        return 0;
    }

    std::tuple<LogicReturn, border_rule_t*> border_find_high_priority_rule(
        bool filter_by_suited_networks, location_update_context_ptr& ctx,
        const std::vector<border_rule_filter_t>& rules_filtered, int prev_rule_id
    ) {
        //border_rule_t* rule = nullptr;
        return {LogicReturn::Ok, nullptr};
    }

    std::vector<border_rule_filter_t> border_rule_filters_create() {
        return {};
    }

} // namespace Lua
