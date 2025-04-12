#pragma once

#include <sol/sol.hpp>
#include "types.hpp"
#include "context.hpp"

namespace Lua {

    //!@fn buildin functions
    int relay(location_update_context_ptr &ctx, bool active, CauseType cause, std::string &&text);
    int reject(location_update_context_ptr &ctx, bool active, CauseType cause, std::string &&text);
    std::tuple<int, int, int, int> vlr_search(
        location_update_context_ptr &ctx, const NetworkType network_type, const std::string& node,
        const std::string& source, bool strict
    );
    std::tuple<int, int, int, int> mcc_search(
        location_update_context_ptr &ctx, const std::string& mcc, const std::string& mnc,
        const std::string& mme_host, const std::string &mme_realm
    );

    int profile_lock_and_get(location_update_context_ptr &ctx);
    LogicReturn save(location_update_context_ptr &ctx, int result_code, timespec_t *event_time);

    int send_relay(location_update_context_ptr &ctx, CauseType cause, std::string text);

    LogicReturn check_session_out(location_update_context_ptr &ctx);

    time_t local_time(time_t time, int offset);

    std::string time_to_string(time_t time);

    sol::object imsi_white_by_prefix(
        struct SteeringData::Values& steer_data, const std::string& imsi, sol::this_state lua
    );

    sol::object imsi_white_by_range(
        struct SteeringData::Values& steer_data, const std::string& imsi, sol::this_state lua
    );

    sol::object vlr_white(
        vlr_white_ptr &vlr_white, struct SteeringData::Values& steer_data,
        const OperatorNodeType node_type, const std::string& node,
        sol::this_state lua
    );

    LogicReturn lbs_send_request(location_update_context_ptr &ctx, lbs_gw::ServiceType service);

    LogicReturn border_save(location_update_context_ptr &ctx, LBSCauseType cause_type);

    std::vector<int> border_search_locations(
        location_update_context_ptr &ctx, struct SteeringData::Values& steer_data,
        const std::string& lac, const std::string& cell, const std::string& eci
    );

    bool rule_contain_locations(const hash_map<unsigned int, bool>& rule_locations, const std::vector<int>& locations);

    bool rule_contain_imsi_groups(
        const std::vector<unsigned int>&rule_groups, const std::vector<group_id_t>& imsi_groups
    );

    group_id_t imsi_groups_intersection(
        const std::vector<unsigned int>&rule_groups, const std::vector<group_id_t>& imsi_groups
    );

    std::tuple<LogicReturn, border_rule_t*> border_find_high_priority_rule(
        bool filter_by_suited_networks, location_update_context_ptr& ctx,
        const std::vector<border_rule_filter_t>& rules_filtered, int prev_rule_id
    );

    std::vector<border_rule_filter_t> border_rule_filters_create();

    SteeringData::Values& load_steer_data();

}


