#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {
    int bind_data(sol::state_view &lua, sol::metatable& readonly_metatable) {

        auto com_params =
            lua.new_usertype<com_params_t>("com_params_t", "id", &com_params_t::id);
        auto time_zones =
            lua.new_usertype<timezones_t>("time_zones_t", "id", &timezones_t::id);
        auto countries =
            lua.new_usertype<countries_t>("countries_t", "id", &countries_t::id);
        auto operators =
            lua.new_usertype<operators_t>("operators_t", "id", &operators_t::id);
        auto operator_options =
            lua.new_usertype<operator_options_t>("operator_options_t", "id", &operator_options_t::id);
        auto operator_links =
            lua.new_usertype<operator_links_t>("operator_links_t", "id", &operator_links_t::id);
        auto operator_nodes =
            lua.new_usertype<operator_nodes_t>("operator_nodes_t", "id", &operator_nodes_t::id);
        auto operator_node_ranges =
            lua.new_usertype<operator_node_ranges_t>("operator_node_ranges_t", "id", &operator_node_ranges_t::id);
        auto operator_node_prefixes =
            lua.new_usertype<operator_node_prefixes_t>("operator_node_prefixes_t", "id", &operator_node_prefixes_t::id);
        auto operator_prefixes =
            lua.new_usertype<operator_prefixes_t>("operator_prefixes_t", "id", &operator_prefixes_t::id);
        auto operator_ranges =
            lua.new_usertype<operator_ranges_t>("operator_ranges_t", "id", &operator_ranges_t::id);
        auto operator_mccs =
            lua.new_usertype<operator_mccs_t>("operator_mccs_t", "id", &operator_mccs_t::id);
        auto white_nodes =
            lua.new_usertype<white_nodes_t>("white_nodes_t", "id", &white_nodes_t::id);
        auto white_imsies =
            lua.new_usertype<white_imsies_t>("white_imsies_t", "id", &white_imsies_t::id);
        auto imsi_lists =
            lua.new_usertype<imsi_lists_t>("imsi_lists_t", "id", &imsi_lists_t::id);
        auto imsi_groups =
            lua.new_usertype<imsi_groups_t>("imsi_groups_t", "id", &imsi_groups_t::id);
        auto imsi_group_links =
            lua.new_usertype<imsi_group_links_t>("imsi_group_links_t", "id", &imsi_group_links_t::id);
        auto imsi_prefixes =
            lua.new_usertype<imsi_prefixes_t>("imsi_prefixes_t", "id", &imsi_prefixes_t::id);
        auto imsi_ranges =
            lua.new_usertype<imsi_ranges_t>("imsi_ranges_t", "id", &imsi_ranges_t::id);
        auto schedules =
            lua.new_usertype<schedules_t>("schedules_t", "id", &schedules_t::id);
        auto schedule_wdays =
            lua.new_usertype<schedule_wdays_t>("schedule_wdays_t", "id", &schedule_wdays_t::id);
        auto schedule_times =
            lua.new_usertype<schedule_times_t>("schedule_times_t", "id", &schedule_times_t::id);
        auto rules =
            lua.new_usertype<rules_t>("rules_t", "id", &rules_t::id);
        auto rule_params =
            lua.new_usertype<rule_params_t>("rule_params_t", "id", &rule_params_t::id);
        auto rule_groups =
            lua.new_usertype<rule_groups_t>("rule_groups_t", "id", &rule_groups_t::id);
        auto rule_operators =
            lua.new_usertype<rule_operators_t>("rule_operators_t", "id", &rule_operators_t::id);
        auto rule_operator_networks =
            lua.new_usertype<rule_operator_networks_t>("rule_operator_networks_t", "id", &rule_operator_networks_t::id);
        auto rule_operator_answers =
            lua.new_usertype<rule_operator_answers_t>("rule_operator_answers_t", "id", &rule_operator_answers_t::id);
        auto answer_types =
            lua.new_usertype<answer_types_t>("answer_types_t", "id", &answer_types_t::id);
        auto answer_codes =
            lua.new_usertype<answer_codes_t>("answer_codes_t", "id", &answer_codes_t::id);
        auto ban_steers =
            lua.new_usertype<ban_steers_t>("ban_steers_t", "id", &ban_steers_t::id);
        auto border_rules =
            lua.new_usertype<border_rules_t>("border_rules_t", "id", &border_rules_t::id);
        auto border_locations =
            lua.new_usertype<border_locations_t>("border_locations_t", "id", &border_locations_t::id);
        auto locations =
            lua.new_usertype<locations_t>("locations_t", "id", &locations_t::id);

        //hash_map<std::string, bool>
        auto operator_default_options =
            lua.new_usertype<operator_default_options_t>("operator_default_options_t");

        //hash_map<unsigned int, std::shared_ptr<std::vector<unsigned int>>>
        auto country_operators =
            lua.new_usertype<country_operators_t>("country_operators_t");

        //hash_map<int, bool>
        auto home_operators =
            lua.new_usertype<home_operators_t>("home_operators_t");

        //hash_map<std::string, unsigned int>
        auto imsi_white =
            lua.new_usertype<imsi_white_t>("imsi_white_t");

        // hash_map<std::string, imsi_lists_ptr> shared_ptr uint16t
        auto lists_by_imsi =
            lua.new_usertype<lists_by_imsi_t>("lists_by_imsi_t");

        // hash_map<std::string, unsigned int>, the same type as imsi_white_t
        //auto group_by_name =
        //    lua.new_usertype<group_by_name_t>("group_by_name_t");

        //hash_map<unsigned int, std::vector<unsigned int>>
        auto groups_by_group =
            lua.new_usertype<groups_by_group_t>("groups_by_group_t");

        //hash_map<int, std::vector<int>>, the same type as  country_operators_t
        auto country_rules =
            lua.new_usertype<country_rules_t>("country_rules_t");

        //hash_map<int, std::shared_ptr<std::vector<int>>> the same type as  country_operators_t
        auto operator_rules =
            lua.new_usertype<operator_rules_t>("operator_rules_t");

        //hash_map<unsigned short, ban_steer_t *>
        auto ban_countries =
            lua.new_usertype<ban_countries_t>("ban_countries_t");

        //hash_map<int, std::shared_ptr<std::vector<int>>> the same as country_operators_t
        //auto location_border_rules =
        //    lua.new_usertype<location_border_rules_t>("location_border_rules_t");

        //hash_map<std::string, std::shared_ptr<std::vector<int>>>
        auto location_lists =
            lua.new_usertype<location_lists_t>("location_lists_t");

        auto steer_data = lua.new_usertype<SteeringData::Values>("SteeringData");

        steer_data.set("ImsiGroupVoLTE", sol::readonly(&SteeringData::Values::ImsiGroupVoLTE));
        steer_data.set("ImsiGroupOTA", sol::readonly(&SteeringData::Values::ImsiGroupOTA));
        steer_data.set("com_params", sol::readonly(&SteeringData::Values::com_params));
        steer_data.set("time_zones", sol::readonly(&SteeringData::Values::time_zones));
        steer_data.set("countries", sol::readonly(&SteeringData::Values::countries));
        steer_data.set("operators", sol::readonly(&SteeringData::Values::operators));
        steer_data.set("operator_options", sol::readonly(&SteeringData::Values::operator_options));
        steer_data.set("operator_default_options", sol::readonly(&SteeringData::Values::operator_default_options));
        steer_data.set("operator_links", sol::readonly(&SteeringData::Values::operator_links));
        steer_data.set("operator_nodes", sol::readonly(&SteeringData::Values::operator_nodes));
        steer_data.set("operator_node_ranges", sol::readonly(&SteeringData::Values::operator_node_ranges));
        steer_data.set("operator_node_prefixes", sol::readonly(&SteeringData::Values::operator_node_prefixes));
        steer_data.set("operator_prefixes", sol::readonly(&SteeringData::Values::operator_prefixes));
        steer_data.set("operator_ranges", sol::readonly(&SteeringData::Values::operator_ranges));
        steer_data.set("operator_mccs", sol::readonly(&SteeringData::Values::operator_mccs));
        steer_data.set("white_nodes", sol::readonly(&SteeringData::Values::white_nodes));
        steer_data.set("country_operators", sol::readonly(&SteeringData::Values::country_operators));
        steer_data.set("home_operators", sol::readonly(&SteeringData::Values::home_operators));
        steer_data.set("white_imsies", sol::readonly(&SteeringData::Values::white_imsies));
        steer_data.set("white_imsi_prefixes", sol::readonly(&SteeringData::Values::white_imsi_prefixes));
        steer_data.set("white_imsi_ranges", sol::readonly(&SteeringData::Values::white_imsi_ranges));
        steer_data.set("imsi_lists", sol::readonly(&SteeringData::Values::imsi_lists));
        steer_data.set("imsi_groups", sol::readonly(&SteeringData::Values::imsi_groups));
        steer_data.set("imsi_group_links", sol::readonly(&SteeringData::Values::imsi_group_links));
        steer_data.set("imsi_prefixes", sol::readonly(&SteeringData::Values::imsi_prefixes));
        steer_data.set("imsi_ranges", sol::readonly(&SteeringData::Values::imsi_ranges));
        steer_data.set("imsi_white", sol::readonly(&SteeringData::Values::imsi_white));
        steer_data.set("lists_by_imsi", sol::readonly(&SteeringData::Values::lists_by_imsi));
        steer_data.set("group_by_name", sol::readonly(&SteeringData::Values::group_by_name));
        steer_data.set("group_by_prefix", sol::readonly(&SteeringData::Values::group_by_prefix));
        steer_data.set("group_by_range", sol::readonly(&SteeringData::Values::group_by_range));
        steer_data.set("groups_by_group", sol::readonly(&SteeringData::Values::groups_by_group));
        steer_data.set("schedules", sol::readonly(&SteeringData::Values::schedules));
        steer_data.set("schedule_wdays", sol::readonly(&SteeringData::Values::schedule_wdays));
        steer_data.set("schedule_times", sol::readonly(&SteeringData::Values::schedule_times));
        steer_data.set("rules", sol::readonly(&SteeringData::Values::rules));
        steer_data.set("country_rules", sol::readonly(&SteeringData::Values::country_rules));
        steer_data.set("rule_params", sol::readonly(&SteeringData::Values::rule_params));
        steer_data.set("rule_groups", sol::readonly(&SteeringData::Values::rule_groups));
        steer_data.set("rule_operators", sol::readonly(&SteeringData::Values::rule_operators));
        steer_data.set("operator_rules", sol::readonly(&SteeringData::Values::operator_rules));
        steer_data.set("rule_operator_networks", sol::readonly(&SteeringData::Values::rule_operator_networks));
        steer_data.set("rule_operator_answers", sol::readonly(&SteeringData::Values::rule_operator_answers));
        steer_data.set("answer_types", sol::readonly(&SteeringData::Values::answer_types));
        steer_data.set("answer_codes", sol::readonly(&SteeringData::Values::answer_codes));
        steer_data.set("ban_steers", sol::readonly(&SteeringData::Values::ban_steers));
        steer_data.set("ban_countries", sol::readonly(&SteeringData::Values::ban_countries));
        steer_data.set("border_rules", sol::readonly(&SteeringData::Values::border_rules));
        steer_data.set("border_countries", sol::readonly(&SteeringData::Values::border_countries));
        steer_data.set("border_params", sol::readonly(&SteeringData::Values::border_params));
        steer_data.set("border_groups", sol::readonly(&SteeringData::Values::border_groups));
        steer_data.set("border_locations", sol::readonly(&SteeringData::Values::border_locations));
        steer_data.set("location_border_rules", sol::readonly(&SteeringData::Values::location_border_rules));
        steer_data.set("border_operator_networks", sol::readonly(&SteeringData::Values::border_operator_networks));
        steer_data.set("border_operators", sol::readonly(&SteeringData::Values::border_operators));
        steer_data.set("operator_border_rules", sol::readonly(&SteeringData::Values::operator_border_rules));
        steer_data.set("border_operator_answers", sol::readonly(&SteeringData::Values::border_operator_answers));
        steer_data.set("locations", sol::readonly(&SteeringData::Values::locations));
        steer_data.set("location_lists", sol::readonly(&SteeringData::Values::location_lists));

        return ok;
    }
}