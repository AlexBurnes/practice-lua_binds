#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {
    int bind_types(sol::state_view &lua, sol::metatable& readonly_metatable) {

        //FIXME declare all required user data LocationUpdateContext
        // ok here is an working and right example of declare references to class members
        // https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_advanced.cpp#L81
        auto timeval_context = lua.new_usertype<timespec_t>("timespec_t");
        timeval_context.set("sec", &timespec_t::tv_sec);
        timeval_context.set("nsec", &timespec_t::tv_nsec);

        auto prefix = lua.new_usertype<prefix::Prefix<int>>("prefix_t");
        lua["prefix_t"]["search"] = [](prefix::Prefix<int>& s, const std::string& imsi) { return s.search(imsi);};

        auto country = lua.new_usertype<Country>("country_t");
        country.set("id"     , sol::readonly(&Country::id));
        country.set("name"   , sol::readonly(&Country::name));
        country.set("a2"     , sol::readonly(&Country::a2));
        country.set("a3"     , sol::readonly(&Country::a3));
        country.set("tz_id"  , sol::readonly(&Country::tz_id));
        country.set("border" , sol::readonly(&Country::border));

        auto time_zone = lua.new_usertype<TimeZone>("time_zone_t");
        time_zone.set("id"     , sol::readonly(&TimeZone::id));
        time_zone.set("name"   , sol::readonly(&TimeZone::name));
        time_zone.set("utc"    , sol::readonly(&TimeZone::utc));
        time_zone.set("offset" , sol::readonly(&TimeZone::offset));

        auto operator_node = lua.new_usertype<OperatorNode>("operator_node_t");
        operator_node.set("id"          , sol::readonly(&OperatorNode::id));
        operator_node.set("operator_id" , sol::readonly(&OperatorNode::operator_id));
        operator_node.set("type_id"     , sol::readonly(&OperatorNode::type_id));
        operator_node.set("value"       , sol::readonly(&OperatorNode::value));
        operator_node.set("source"      , sol::readonly(&OperatorNode::source));
        operator_node.set("valid"       , sol::readonly(&OperatorNode::valid));

        auto operator_node_range = lua.new_usertype<OperatorNodeRange>("operator_node_range_t");
        operator_node_range.set("id"          , sol::readonly(&OperatorNodeRange::id));
        operator_node_range.set("operator_id" , sol::readonly(&OperatorNodeRange::operator_id ));
        operator_node_range.set("type_id"     , sol::readonly(&OperatorNodeRange::type_id));
        operator_node_range.set("range_start" , sol::readonly(&OperatorNodeRange::range_start ));
        operator_node_range.set("range_end"   , sol::readonly(&OperatorNodeRange::range_end));
        operator_node_range.set("source"      , sol::readonly(&OperatorNodeRange::source));
        operator_node_range.set("valid"       , sol::readonly(&OperatorNodeRange::valid));


        auto operator_node_prefix = lua.new_usertype<OperatorNodePrefix>("operator_node_prefix_t");
        operator_node_prefix.set("id"          , sol::readonly(&OperatorNodePrefix::id));
        operator_node_prefix.set("operator_id" , sol::readonly(&OperatorNodePrefix::operator_id));
        operator_node_prefix.set("type_id"     , sol::readonly(&OperatorNodePrefix::type_id));
        operator_node_prefix.set("prefix"      , sol::readonly(&OperatorNodePrefix::prefix));
        operator_node_prefix.set("source"      , sol::readonly(&OperatorNodePrefix::source));
        operator_node_prefix.set("valid"       , sol::readonly(&OperatorNodePrefix::valid));

        auto operator_prefix = lua.new_usertype<OperatorPrefix>("operator_prefix_t");
        operator_prefix.set("id"          , sol::readonly(&OperatorPrefix::id));
        operator_prefix.set("operator_id" , sol::readonly(&OperatorPrefix::operator_id));
        operator_prefix.set("country_id"  , sol::readonly(&OperatorPrefix::country_id));
        operator_prefix.set("type_id"     , sol::readonly(&OperatorPrefix::type_id));
        operator_prefix.set("prefix"      , sol::readonly(&OperatorPrefix::prefix));
        operator_prefix.set("valid"       , sol::readonly(&OperatorPrefix::valid));


        auto operator_range = lua.new_usertype<OperatorRange>("operator_range_t");
        operator_range.set("id"          , sol::readonly(&OperatorRange::id));
        operator_range.set("operator_id" , sol::readonly(&OperatorRange::operator_id));
        operator_range.set("country_id"  , sol::readonly(&OperatorRange::country_id));
        operator_range.set("type_id"     , sol::readonly(&OperatorRange::type_id));
        operator_range.set("range_start" , sol::readonly(&OperatorRange::range_start));
        operator_range.set("range_end"   , sol::readonly(&OperatorRange::range_end));
        operator_range.set("valid"       , sol::readonly(&OperatorRange::valid));

        auto operator_mcc = lua.new_usertype<OperatorMCC>("operator_mcc_t");
        operator_mcc.set("id"      , sol::readonly(&OperatorMCC::id));
        operator_mcc.set("id"      , sol::readonly(&OperatorMCC::id));
        operator_mcc.set("mcc"     , sol::readonly(&OperatorMCC::mcc));
        operator_mcc.set("mnc"     , sol::readonly(&OperatorMCC::mnc));
        operator_mcc.set("type_id" , sol::readonly(&OperatorMCC::type_id));
        operator_mcc.set("valid"   , sol::readonly(&OperatorMCC::valid));

        auto white_node = lua.new_usertype<WhiteNode>("white_node_t");
        white_node.set("id"          , sol::readonly(&WhiteNode::id));
        white_node.set("operator_id" , sol::readonly(&WhiteNode::operator_id));
        white_node.set("node_type"   , sol::readonly(&WhiteNode::node_type));
        white_node.set("value"       , sol::readonly(&WhiteNode::value));
        white_node.set("valid"       , sol::readonly(&WhiteNode::valid));

        auto com_operator = lua.new_usertype<Operator>("operator_t");
        com_operator.set("id"         , sol::readonly(&Operator::id));
        com_operator.set("name"       , sol::readonly(&Operator::name));
        com_operator.set("tadig"      , sol::readonly(&Operator::tadig));
        com_operator.set("own"        , sol::readonly(&Operator::own));
        com_operator.set("country_id" , sol::readonly(&Operator::country_id));
        com_operator.set("closed"     , sol::readonly(&Operator::closed));
        com_operator.set("vlr_white"  , sol::readonly(&Operator::vlr_white));
        com_operator.set("volte"      , sol::readonly(&Operator::volte));

        auto operator_link = lua.new_usertype<OperatorLink>("operator_link_t");
        operator_link.set("id"           , sol::readonly(&OperatorLink::id));
        operator_link.set("operator_id"  , sol::readonly(&OperatorLink::operator_id));
        operator_link.set("option_name"  , sol::readonly(&OperatorLink::option_name));
        operator_link.set("option_value" , sol::readonly(&OperatorLink::option_value));

        auto operator_option = lua.new_usertype<OperatorOption>("operator_option_t");
        operator_option.set("id"            , sol::readonly(&OperatorOption::id));
        operator_option.set("option_name"   , sol::readonly(&OperatorOption::option_name));
        operator_option.set("default_value" , sol::readonly(&OperatorOption::default_value));

        auto white_imsi = lua.new_usertype<WhiteIMSI>("white_imsi_t");
        white_imsi.set("id"   , sol::readonly(&WhiteIMSI::id));
        white_imsi.set("imsi" , sol::readonly(&WhiteIMSI::imsi));

        auto white_imsi_prefix = lua.new_usertype<WhiteIMSIPrefix>("white_imsi_prefix_t");
        white_imsi_prefix.set("id"     , sol::readonly(&WhiteIMSIPrefix::id));
        white_imsi_prefix.set("prefix" , sol::readonly(&WhiteIMSIPrefix::prefix));

        auto white_imsi_range = lua.new_usertype<WhiteIMSIRange>("white_imsi_range_t");
        white_imsi_range.set("id"          , sol::readonly(&WhiteIMSIRange::id));
        white_imsi_range.set("range_start" , sol::readonly(&WhiteIMSIRange::range_start));
        white_imsi_range.set("range_end"   , sol::readonly(&WhiteIMSIRange::range_end));

        auto list_imsi = lua.new_usertype<ListIMSI>("list_imsi_t");
        list_imsi.set("id"      , sol::readonly(&ListIMSI::id));
        list_imsi.set("list_id" , sol::readonly(&ListIMSI::list_id));
        list_imsi.set("imsi"    , sol::readonly(&ListIMSI::imsi));

        auto group_imsi = lua.new_usertype<GroupIMSI>("group_imsi_t");
        group_imsi.set("id"           , sol::readonly(&GroupIMSI::id));
        group_imsi.set("name"         , sol::readonly(&GroupIMSI::name));
        group_imsi.set("system_group" , sol::readonly(&GroupIMSI::system_group));
        group_imsi.set("level"        , sol::readonly(&GroupIMSI::level));

        auto group_link = lua.new_usertype<GroupLink>("group_link_t");
        group_link.set("id"              , sol::readonly(&GroupLink::id));
        group_link.set("parent_group_id" , sol::readonly(&GroupLink::parent_group_id));
        group_link.set("group_id"        , sol::readonly(&GroupLink::group_id));

        auto prefix_imsi = lua.new_usertype<PrefixIMSI>("prefix_imsi_t");
        prefix_imsi.set("id"      , sol::readonly(&PrefixIMSI::id));
        prefix_imsi.set("list_id" , sol::readonly(&PrefixIMSI::list_id));
        prefix_imsi.set("prefix"  , sol::readonly(&PrefixIMSI::prefix));

        auto range_imsi = lua.new_usertype<RangeIMSI>("range_imsi_t");
        range_imsi.set("id"      , sol::readonly(&RangeIMSI::id));
        range_imsi.set("list_id" , sol::readonly(&RangeIMSI::list_id));
        range_imsi.set("begin"   , sol::readonly(&RangeIMSI::begin));
        range_imsi.set("end"     , sol::readonly(&RangeIMSI::end));

        auto schedule_time = lua.new_usertype<ScheduleTime>("schedule_time_t");
        schedule_time.set("id"        , sol::readonly(&ScheduleTime::id));
        schedule_time.set("wday_id"   , sol::readonly(&ScheduleTime::wday_id));
        schedule_time.set("time_from" , sol::readonly(&ScheduleTime::time_from));
        schedule_time.set("from"      , sol::readonly(&ScheduleTime::from));
        schedule_time.set("time_till" , sol::readonly(&ScheduleTime::time_till));
        schedule_time.set("till"      , sol::readonly(&ScheduleTime::till));

        auto schedule_wday = lua.new_usertype<ScheduleWday>("schedule_wday_t");
        schedule_wday.set("id"          , sol::readonly(&ScheduleWday::id));
        schedule_wday.set("schedule_id" , sol::readonly(&ScheduleWday::schedule_id));
        schedule_wday.set("times"       , sol::readonly(&ScheduleWday::times));

        auto schedule = lua.new_usertype<Schedule>("schedule_t");
        schedule.set("id"    , sol::readonly(&Schedule::id));
        schedule.set("name"  , sol::readonly(&Schedule::name));
        schedule.set("wdays" , sol::readonly(&Schedule::wdays));


        auto lbs_type = lua.new_usertype<LBS>("lbs_t");
        lbs_type.set("service"         , &LBS::service        );
        lbs_type.set("code"            , &LBS::code           );
        lbs_type.set("age"             , &LBS::age            );
        lbs_type.set("time"            , &LBS::time           );
        lbs_type.set("node"            , &LBS::node           );
        lbs_type.set("lac"             , &LBS::lac            );
        lbs_type.set("cell"            , &LBS::cell           );
        lbs_type.set("eci"             , &LBS::eci            );
        lbs_type.set("mme"             , &LBS::mme            );
        lbs_type.set("rule_id"         , &LBS::rule_id        );
        lbs_type.set("reject_attempt"  , &LBS::reject_attempt );
        lbs_type.set("border_attempt"  , &LBS::border_attempt );
        lbs_type.set("network_type"    , &LBS::network_type   );
        lbs_type.set("response"        , &LBS::response       );
        lbs_type.set("imsi"            , &LBS::imsi           );
        lbs_type.set("logic_session"   , &LBS::logic_session  );
        lbs_type.set("cause_type"      , &LBS::cause_type     );
        lbs_type.set("operator_id"     , &LBS::operator_id    );
        lbs_type.set("mgt"             , &LBS::mgt            );

        auto rule = lua.new_usertype<Rule>("rule_t");
        rule.set("id"                                 , sol::readonly(&Rule::id));
        rule.set("type_id"                            , sol::readonly(&Rule::type_id));
        rule.set("country_id"                         , sol::readonly(&Rule::country_id));
        rule.set("time_begin"                         , sol::readonly(&Rule::time_begin));
        rule.set("begin"                              , sol::readonly(&Rule::begin));
        rule.set("time_end"                           , sol::readonly(&Rule::time_end));
        rule.set("end"                                , sol::readonly(&Rule::end));
        rule.set("schedule_id"                        , sol::readonly(&Rule::schedule_id));
        rule.set("enabled"                            , sol::readonly(&Rule::enabled));
        rule.set("level"                              , sol::readonly(&Rule::level));
        rule.set("distribution_duration"              , sol::readonly(&Rule::distribution_duration));
        rule.set("imsi_group"                         , sol::readonly(&Rule::imsi_group));
        rule.set("group_is_intersection"              , sol::readonly(&Rule::group_is_intersection));
        rule.set("imsi_lists"                         , sol::readonly(&Rule::imsi_lists));
        rule.set("operators"                          , sol::readonly(&Rule::operators));
        rule.set("operators_rule"                     , sol::readonly(&Rule::operators_rule));
        rule.set("OneNetworkAlive"                    , sol::readonly(&Rule::OneNetworkAlive));
        rule.set("FirstTimeinCountry"                 , sol::readonly(&Rule::FirstTimeinCountry));
        rule.set("MaxRedirectionTimePerSession"       , sol::readonly(&Rule::MaxRedirectionTimePerSession));
        rule.set("MaxULRejectsPerSeries4OneNodeLTE"   , sol::readonly(&Rule::MaxULRejectsPerSeries4OneNodeLTE));
        rule.set("MaxULRejectsPerSeries4OneNode"      , sol::readonly(&Rule::MaxULRejectsPerSeries4OneNode));
        rule.set("MaxNetworkRejects"                  , sol::readonly(&Rule::MaxNetworkRejects));
        rule.set("TimeoutBetweenRedirectionSession"   , sol::readonly(&Rule::TimeoutBetweenRedirectionSession));
        rule.set("T1_mode"                            , sol::readonly(&Rule::T1_mode));
        rule.set("CS_alive"                           , sol::readonly(&Rule::CS_alive));
        rule.set("PS_alive"                           , sol::readonly(&Rule::PS_alive));
        rule.set("LTE_alive"                          , sol::readonly(&Rule::LTE_alive));
        rule.set("IDR_alive"                          , sol::readonly(&Rule::IDR_alive));
        rule.set("isOneNetworkAlive"                  , sol::readonly(&Rule::isOneNetworkAlive));
        rule.set("isFirstTimeinCountry"               , sol::readonly(&Rule::isFirstTimeinCountry));
        rule.set("isMaxRedirectionTimePerSession"     , sol::readonly(&Rule::isMaxRedirectionTimePerSession));
        rule.set("isMaxULRejectsPerSeries4OneNodeLTE" , sol::readonly(&Rule::isMaxULRejectsPerSeries4OneNodeLTE));
        rule.set("isMaxULRejectsPerSeries4OneNode"    , sol::readonly(&Rule::isMaxULRejectsPerSeries4OneNode));
        rule.set("isMaxNetworkRejects"                , sol::readonly(&Rule::isMaxNetworkRejects));
        rule.set("isTimeoutBetweenRedirectionSession" , sol::readonly(&Rule::isTimeoutBetweenRedirectionSession));
        rule.set("isT1_mode"                          , sol::readonly(&Rule::isT1_mode));
        rule.set("isCS_alive"                         , sol::readonly(&Rule::isCS_alive));
        rule.set("isPS_alive"                         , sol::readonly(&Rule::isPS_alive));
        rule.set("isLTE_alive"                        , sol::readonly(&Rule::isLTE_alive));
        rule.set("isIDR_alive"                        , sol::readonly(&Rule::isIDR_alive));

        auto border_rule = lua.new_usertype<BorderRule>("border_rule_t");
        border_rule.set("id"                          , sol::readonly(&BorderRule::id));
        border_rule.set("country_id"                  , sol::readonly(&BorderRule::country_id));
        border_rule.set("time_begin"                  , sol::readonly(&BorderRule::time_begin));
        border_rule.set("begin"                       , sol::readonly(&BorderRule::begin));
        border_rule.set("time_end"                    , sol::readonly(&BorderRule::time_end));
        border_rule.set("end"                         , sol::readonly(&BorderRule::end));
        border_rule.set("enabled"                     , sol::readonly(&BorderRule::enabled));
        border_rule.set("allowed"                     , sol::readonly(&BorderRule::allowed));
        border_rule.set("level"                       , sol::readonly(&BorderRule::level));
        border_rule.set("imsi_lists"                  , sol::readonly(&BorderRule::imsi_lists));
        border_rule.set("imsi_group"                  , sol::readonly(&BorderRule::imsi_group));
        border_rule.set("group_is_intersection"       , sol::readonly(&BorderRule::group_is_intersection));
        border_rule.set("locations"                   , sol::readonly(&BorderRule::locations));
        border_rule.set("operators"                   , sol::readonly(&BorderRule::operators));
        border_rule.set("operators_rule"              , sol::readonly(&BorderRule::operators_rule));
        border_rule.set("BorderRoamLBSTimeout"        , sol::readonly(&BorderRule::BorderRoamLBSTimeout));
        border_rule.set("BorderRoamRestrictTimeout"   , sol::readonly(&BorderRule::BorderRoamRestrictTimeout));
        border_rule.set("BorderRoamMaxRejects"        , sol::readonly(&BorderRule::BorderRoamMaxRejects));
        border_rule.set("BorderRoamMaxRequests"       , sol::readonly(&BorderRule::BorderRoamMaxRequests));
        border_rule.set("isBorderRoamLBSTimeout"      , sol::readonly(&BorderRule::isBorderRoamLBSTimeout));
        border_rule.set("isBorderRoamRestrictTimeout" , sol::readonly(&BorderRule::isBorderRoamRestrictTimeout));
        border_rule.set("isBorderRoamMaxRejects"      , sol::readonly(&BorderRule::isBorderRoamMaxRejects));
        border_rule.set("isBorderRoamMaxRequests"     , sol::readonly(&BorderRule::isBorderRoamMaxRequests));


        auto rule_filter = lua.new_usertype<RuleFilter>("rule_filter_t");
        rule_filter.set("rule"            , &RuleFilter::rule            );
        rule_filter.set("suited_networks" , &RuleFilter::suited_networks );

        auto border_rule_filter = lua.new_usertype<border_rule_filter_t>("border_rule_filter_t");
        border_rule_filter.set("rule"            , &BorderRuleFilter::rule            );
        border_rule_filter.set("suited_networks" , &BorderRuleFilter::suited_networks );

        auto rule_param = lua.new_usertype<RuleParam>("rule_param_t");
        rule_param.set("id"         , sol::readonly(&RuleParam::id));
        rule_param.set("rule_id"    , sol::readonly(&RuleParam::rule_id));
        rule_param.set("code"       , sol::readonly(&RuleParam::code));
        rule_param.set("value"      , sol::readonly(&RuleParam::value));
        rule_param.set("type_id"    , sol::readonly(&RuleParam::type_id));
        rule_param.set("param_type" , sol::readonly(&RuleParam::param_type));

        auto rule_group = lua.new_usertype<RuleGroup>("rule_group_t");
        rule_group.set("id"       , sol::readonly(&RuleGroup::id));
        rule_group.set("rule_id"  , sol::readonly(&RuleGroup::rule_id));
        rule_group.set("group_id" , sol::readonly(&RuleGroup::group_id));

        auto border_rule_location = lua.new_usertype<BorderRuleLocation>("border_rule_location_t");
        border_rule_location.set("id"          , sol::readonly(&BorderRuleLocation::id));
        border_rule_location.set("rule_id"     , sol::readonly(&BorderRuleLocation::rule_id));
        border_rule_location.set("location_id" , sol::readonly(&BorderRuleLocation::location_id));

        auto location = lua.new_usertype<Location>("location_t");
        location.set("id"          , sol::readonly(&Location::id));
        location.set("location_id" , sol::readonly(&Location::location_id));
        location.set("lac"         , sol::readonly(&Location::lac));
        location.set("cellid"      , sol::readonly(&Location::cellid));
        location.set("eci"         , sol::readonly(&Location::eci));

        auto answer_type = lua.new_usertype<AnswerType>("answer_type_t");
        answer_type.set("id"           , sol::readonly(&AnswerType::id));
        answer_type.set("network_type" , sol::readonly(&AnswerType::network_type));
        answer_type.set("answer_name"  , sol::readonly(&AnswerType::answer_name));
        answer_type.set("finality"     , sol::readonly(&AnswerType::finality));
        answer_type.set("codes"        , sol::readonly(&AnswerType::codes));

        auto answer_code = lua.new_usertype<AnswerCode>("answer_code_t");
        answer_code.set("id"               , sol::readonly(&AnswerCode::id));
        answer_code.set("answer_type_id"   , sol::readonly(&AnswerCode::answer_type_id));
        answer_code.set("protocol_version" , sol::readonly(&AnswerCode::protocol_version));
        answer_code.set("answer_code"      , sol::readonly(&AnswerCode::answer_code));
        answer_code.set("diagnostic_code"  , sol::readonly(&AnswerCode::diagnostic_code));

        auto ban_steer = lua.new_usertype<BanSteer>("ban_steer_t");
        ban_steer.set("id"         , sol::readonly(&BanSteer::id));
        ban_steer.set("country_id" , sol::readonly(&BanSteer::country_id));
        ban_steer.set("cause_id"   , sol::readonly(&BanSteer::cause_id));
        ban_steer.set("note"       , sol::readonly(&BanSteer::note));
        ban_steer.set("start_time" , sol::readonly(&BanSteer::start_time));
        ban_steer.set("end_time"   , sol::readonly(&BanSteer::end_time));

        auto oper_rule = lua.new_usertype<OperRule>("oper_rule_t");
        oper_rule.set("id"                      , sol::readonly(&OperRule::id));
        oper_rule.set("rule_id"                 , sol::readonly(&OperRule::rule_id));
        oper_rule.set("operator_id"             , sol::readonly(&OperRule::operator_id));
        oper_rule.set("priority_type"           , sol::readonly(&OperRule::priority_type));
        oper_rule.set("priority_rate"           , sol::readonly(&OperRule::priority_rate));
        oper_rule.set("priority_rate_corrected" , sol::readonly(&OperRule::priority_rate_corrected));
        oper_rule.set("allowed"                 , sol::readonly(&OperRule::allowed));
        oper_rule.set("counter_sum"             , sol::readonly(&OperRule::counter_sum));
        oper_rule.set("networks"                , sol::readonly(&OperRule::networks));
        oper_rule.set("answer_types"            , sol::readonly(&OperRule::answer_types));

        auto oper_rule_network = lua.new_usertype<OperRuleNetwork>("oper_rule_network_t");
        oper_rule_network.set("id"              , sol::readonly(&OperRuleNetwork::id));
        oper_rule_network.set("rule_oper_id"    , sol::readonly(&OperRuleNetwork::rule_oper_id));
        oper_rule_network.set("network_type_id" , sol::readonly(&OperRuleNetwork::network_type_id));
        oper_rule_network.set("enabled"         , sol::readonly(&OperRuleNetwork::enabled));

        auto oper_rule_answer = lua.new_usertype<OperRuleAnswer>("oper_rule_answer_t");
        oper_rule_answer.set("id"                   , sol::readonly(&OperRuleAnswer::id));
        oper_rule_answer.set("rule_oper_network_id" , sol::readonly(&OperRuleAnswer::rule_oper_network_id));
        oper_rule_answer.set("answer_type_id"       , sol::readonly(&OperRuleAnswer::answer_type_id));
        return ok;
    }
}