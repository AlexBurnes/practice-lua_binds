#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {
    int bind_services(sol::state_view &lua, sol::metatable& readonly_metatable) {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // CoreN
        lua["CorenErrorCode"] = create_readonly_table_with(lua, readonly_metatable,
            "Success"                                    , MsgProto1::CoreNErrorCode::success,
            "Unknown_error"                              , MsgProto1::CoreNErrorCode::unknown_error,
            "Coren_svc_node_not_found"                   , MsgProto1::CoreNErrorCode::coren_svc_node_not_found,
            "Svc_not_found"                              , MsgProto1::CoreNErrorCode::svc_not_found,
            "Svc_and_or_conn_not_found"                  , MsgProto1::CoreNErrorCode::svc_and_or_conn_not_found,
            "Coren_svc_message_is_empty"                 , MsgProto1::CoreNErrorCode::coren_svc_message_is_empty,
            "Coren_svc_body_constrait_validation_failed" ,
                MsgProto1::CoreNErrorCode::coren_svc_body_constrait_validation_failed,
            "Coren_svc_body_decode_message_failed"       ,
                MsgProto1::CoreNErrorCode::coren_svc_body_decode_message_failed,
            "Coren_svc_reg_host_src_is_incorrect"        ,
                MsgProto1::CoreNErrorCode::coren_svc_reg_host_src_is_incorrect,
            "Session_flags_arent_correct"                , MsgProto1::CoreNErrorCode::session_flags_arent_correct,
            "Session_already_exists"                     , MsgProto1::CoreNErrorCode::session_already_exists,
            "Session_not_found"                          , MsgProto1::CoreNErrorCode::session_not_found,
            "Session_addresses_is_incorrect"             , MsgProto1::CoreNErrorCode::session_addresses_is_incorrect,
            "Session_closed_is_mandatory"                , MsgProto1::CoreNErrorCode::session_closed_is_mandatory,
            "Session_timeout"                            , MsgProto1::CoreNErrorCode::session_timeout,
            "Service_high_rate"                          , MsgProto1::CoreNErrorCode::service_high_rate
        );

        lua["CorenEvent"] = create_readonly_table_with(lua, readonly_metatable,
              "Receive"     , CoreN::Event::Receive,
              "Close"       , CoreN::Event::Close,
              "Error"       , CoreN::Event::Error,
              "SessionsOut" , CoreN::Event::SessionsOut,
              "Connect"     , CoreN::Event::Connect,
              "Register"    , CoreN::Event::Register,
              "Unregister"  , CoreN::Event::Unregister,
              "Disconnect"  , CoreN::Event::Disconnect,
              "Stat"        , CoreN::Event::Stat,
              "WriteReady"  , CoreN::Event::WriteReady,
              "ReadReady"   , CoreN::Event::ReadReady
        );

        auto coren_error = lua.new_usertype<CoreN::coren_error_t>("coren_error_t");
        coren_error.set("code"    , &CoreN::coren_error_t::code    );
        coren_error.set("message" , &CoreN::coren_error_t::message );
        coren_error.set("sid"     , &CoreN::coren_error_t::sid     );

        lua.new_enum<coren::Flags>( "CorenFlags", {
            { "Continue" , coren::Flags::session_continue },
            { "Start"    , coren::Flags::session_start    },
            { "Stop"     , coren::Flags::session_stop     },
            { "Once"     , coren::Flags::session_once     }
        });

        auto coren_address = lua.new_usertype<coren::Address>("coren_address_t");
        coren_address.set("service" , &coren::Address::service );
        coren_address.set("host"    , &coren::Address::host    );
        coren_address.set("conid"   , &coren::Address::conid   );

        auto coren_message = lua.new_usertype<coren::Message>("coren_message_t");
        coren_message.set("source"      , &coren::Message::source      );
        coren_message.set("destination" , &coren::Message::destination );
        coren_message.set("label"       , &coren::Message::label       );
        //body is a void reference, body_size doesn't matter for lua logic
        //coren_message.set("body"        , &coren::Message::body        );
        //coren_message.set("body_size"   , &coren::Message::body_size   );
        coren_message.set("session_id"  , &coren::Message::session_id  );
        coren_message.set("flags"       , &coren::Message::flags       );
        coren_message.set("timeout"     , &coren::Message::timeout     );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Steering

        lua["SteeringEvent"] = create_readonly_table_with(lua, readonly_metatable,
            "LocationUpdate"        , steering::Event::LocationUpdate,
            "LogicAnswer"           , steering::Event::LogicAnswer,
            "LocationResult"        , steering::Event::LocationResult,
            "SubscriberDataRequest" , steering::Event::SubscriberDataRequest,
            "SubscriberDataAnswer"  , steering::Event::SubscriberDataAnswer
        );

        auto steering_msisdn = lua.new_usertype<steering::msisdn_t>("steering_msisdn_t");
        steering_msisdn.set("nai"    , &steering::msisdn_t::nai);
        steering_msisdn.set("npi"    , &steering::msisdn_t::npi);
        steering_msisdn.set("number" , &steering::msisdn_t::number);

        auto steering_gsm_gprs = lua.new_usertype<steering::gsm_gprs_t>("steering_gsm_gprs_t");
        steering_gsm_gprs.set("vlr"         , &steering::gsm_gprs_t::vlr         );
        steering_gsm_gprs.set("op_code"     , &steering::gsm_gprs_t::op_code     );
        steering_gsm_gprs.set("source_gt"   , &steering::gsm_gprs_t::source_gt   );
        steering_gsm_gprs.set("dest_gt"     , &steering::gsm_gprs_t::dest_gt     );
        steering_gsm_gprs.set("map_version" , &steering::gsm_gprs_t::map_version );

        auto steering_lte_number = lua.new_usertype<steering::lte_number_t>("steering_lte_number_t");
        steering_lte_number.set("mcc"       , &steering::lte_number_t::mcc       );
        steering_lte_number.set("mnc"       , &steering::lte_number_t::mnc       );
        steering_lte_number.set("mme_host"  , &steering::lte_number_t::mme_host  );
        steering_lte_number.set("mme_realm" , &steering::lte_number_t::mme_realm );

        auto steering_location_update = lua.new_usertype<steering::location_update_t>("steering_location_update_t");
        steering_location_update.set("imsi"              , &steering::location_update_t::imsi              );
        steering_location_update.set("registration_type" , &steering::location_update_t::registration_type );
        steering_location_update.set("unix_time"         , &steering::location_update_t::unix_time         );
        steering_location_update.set("registration"      , &steering::location_update_t::registration      );

        auto steering_location_result = lua.new_usertype<steering::location_result_t>("steering_location_result");
        steering_location_result.set("code"      , &steering::location_result_t::code      );
        steering_location_result.set("text"      , &steering::location_result_t::text      );
        steering_location_result.set("unix_time" , &steering::location_result_t::unix_time );
        steering_location_result.set("imsi"      , &steering::location_result_t::imsi      );
        steering_location_result.set("msisdn"    , &steering::location_result_t::msisdn    );
        steering_location_result.set("vlr"       , &steering::location_result_t::vlr       );
        steering_location_result.set("op_code"   , &steering::location_result_t::op_code   );

        auto steering_logic_answer = lua.new_usertype<steering::logic_answer_t>("steering_logic_answer_t");
        steering_logic_answer.set("code"            , &steering::logic_answer_t::code            );
        steering_logic_answer.set("text"            , &steering::logic_answer_t::text            );
        steering_logic_answer.set("unix_time"       , &steering::logic_answer_t::unix_time       );
        steering_logic_answer.set("logic_session"   , &steering::logic_answer_t::logic_session   );
        steering_logic_answer.set("diagnostic_code" , &steering::logic_answer_t::diagnostic_code );
        steering_logic_answer.set("source_gt"       , &steering::logic_answer_t::source_gt       );

        auto steering_sub_data_req = lua.new_usertype<steering::sub_data_req_t>("steering_sub_data_req_t");
        steering_sub_data_req.set("imsi"          , &steering::sub_data_req_t::imsi          );
        steering_sub_data_req.set("unix_time"     , &steering::sub_data_req_t::unix_time     );
        steering_sub_data_req.set("mme_host"      , &steering::sub_data_req_t::mme_host      );
        steering_sub_data_req.set("mme_realm"     , &steering::sub_data_req_t::mme_realm     );
        steering_sub_data_req.set("logic_session" , &steering::sub_data_req_t::logic_session );

        auto steering_sub_data_ans = lua.new_usertype<steering::sub_data_ans_t>("steering_sub_data_ans_t");
        steering_sub_data_ans.set("code"        , &steering::sub_data_ans_t::code        );
        steering_sub_data_ans.set("text"        , &steering::sub_data_ans_t::text        );
        steering_sub_data_ans.set("imsi"        , &steering::sub_data_ans_t::imsi        );
        steering_sub_data_ans.set("age_of_info" , &steering::sub_data_ans_t::age_of_info );
        steering_sub_data_ans.set("unix_time"   , &steering::sub_data_ans_t::unix_time   );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Diameter

        lua["DiameterEvent"] = create_readonly_table_with(lua, readonly_metatable,
            "LocationUpdate"        , steeringDiameter::Event::Request,
            "LogicAnswer"           , steeringDiameter::Event::Response,
            "LocationResult"        , steeringDiameter::Event::Result,
            "SubscriberDataRequest" , steeringDiameter::Event::SubscriberDataRequest,
            "SubscriberDataAnswer"  , steeringDiameter::Event::SubscriberDataAnswer,
            "ErrorMessage"          , steeringDiameter::Event::ErrorMessage
        );

        auto diameter_location_update =
            lua.new_usertype<steeringDiameter::location_update_t>("diameter_location_update_t");
        diameter_location_update.set("imsi"      , &steeringDiameter::location_update_t::imsi      );
        diameter_location_update.set("unix_time" , &steeringDiameter::location_update_t::unix_time );
        diameter_location_update.set("mcc"       , &steeringDiameter::location_update_t::mcc       );
        diameter_location_update.set("mnc"       , &steeringDiameter::location_update_t::mnc       );
        diameter_location_update.set("mme_host"  , &steeringDiameter::location_update_t::mme_host  );
        diameter_location_update.set("mme_realm" , &steeringDiameter::location_update_t::mme_realm );

        auto diameter_location_result =
            lua.new_usertype<steeringDiameter::location_result_t>("diameter_location_result_t");
        diameter_location_result.set("code"             , &steeringDiameter::location_result_t::code             );
        diameter_location_result.set("error_diagnostic" , &steeringDiameter::location_result_t::error_diagnostic );
        diameter_location_result.set("text"             , &steeringDiameter::location_result_t::text             );
        diameter_location_result.set("unix_time"        , &steeringDiameter::location_result_t::unix_time        );
        diameter_location_result.set("logic_session"    , &steeringDiameter::location_result_t::logic_session    );
        diameter_location_result.set("volte"            , &steeringDiameter::location_result_t::volte            );

        auto diameter_hss_result =
            lua.new_usertype<steeringDiameter::hss_result_t>("diameter_hss_result_t");
        diameter_hss_result.set("code"      , &steeringDiameter::hss_result_t::code      );
        diameter_hss_result.set("unix_time" , &steeringDiameter::hss_result_t::unix_time );
        diameter_hss_result.set("msisdn"    , &steeringDiameter::hss_result_t::msisdn    );
        diameter_hss_result.set("volte"     , &steeringDiameter::hss_result_t::volte     );

        auto diameter_data_req =
            lua.new_usertype<steeringDiameter::sub_data_req_t>("diameter_sub_data_req_t");
        diameter_data_req.set("imsi"      , &steeringDiameter::sub_data_req_t::imsi      );
        diameter_data_req.set("unix_time" , &steeringDiameter::sub_data_req_t::unix_time );
        diameter_data_req.set("mme_host"  , &steeringDiameter::sub_data_req_t::mme_host  );
        diameter_data_req.set("mme_realm" , &steeringDiameter::sub_data_req_t::mme_realm );

        auto diameter_sub_data_ans =
            lua.new_usertype<steeringDiameter::sub_data_ans_t>("diameter_sub_data_ans_t");
        diameter_sub_data_ans.set("code"        , &steeringDiameter::sub_data_ans_t::code        );
        diameter_sub_data_ans.set("text"        , &steeringDiameter::sub_data_ans_t::text        );
        diameter_sub_data_ans.set("age_of_info" , &steeringDiameter::sub_data_ans_t::age_of_info );
        diameter_sub_data_ans.set("unix_time"   , &steeringDiameter::sub_data_ans_t::unix_time   );

        auto diameter_error_message =
            lua.new_usertype<steeringDiameter::error_message_t>("diameter_error_message_t");
        diameter_error_message.set("code" , &steeringDiameter::error_message_t::code );
        diameter_error_message.set("text" , &steeringDiameter::error_message_t::text );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // lbsgw

        lua.new_enum<lbs_gw::ServiceType>( "LBSGWServiceType", {
            { "LoopBack",        lbs_gw::ServiceType::LoopBack },
            { "Diameter",        lbs_gw::ServiceType::Diameter },
            { "Map",             lbs_gw::ServiceType::Map },
            { "Default",         lbs_gw::ServiceType::Default },
            { "DiameterThanMap", lbs_gw::ServiceType::DiameterThanMap },
            { "SwitchControl",   lbs_gw::ServiceType::SwitchControl },
            { "Max",             lbs_gw::ServiceType::Max }
        });

        lua.new_enum<lbs_gw::code_e>( "LBSGWErrorCode", {
            { "ok",    lbs_gw::code_e::code_ok    },
            { "error", lbs_gw::code_e::code_error }
        });

        lua["LBSGWEvent"] = create_readonly_table_with(lua, readonly_metatable,
            "Request"  , lbs_gw::Event::Request,
            "Response" , lbs_gw::Event::Response
        );

        auto lbsgw_request = lua.new_usertype<lbs_gw::request_t>("lbsgw_request_t");
        lbsgw_request.set("service"       , &lbs_gw::request_t::service       );
        lbsgw_request.set("imsi"          , &lbs_gw::request_t::imsi          );
        lbsgw_request.set("mgt"           , &lbs_gw::request_t::mgt           );
        lbsgw_request.set("mme_host"      , &lbs_gw::request_t::mme_host      );
        lbsgw_request.set("mme_realm"     , &lbs_gw::request_t::mme_realm     );
        lbsgw_request.set("msisdn"        , &lbs_gw::request_t::msisdn        );
        lbsgw_request.set("logic_session" , &lbs_gw::request_t::logic_session );
        lbsgw_request.set("event_time"    , &lbs_gw::request_t::event_time    );

        auto lbsgw_response = lua.new_usertype<lbs_gw::response_t>("lbsgw_response_t");
        lbsgw_response.set("service"       , &lbs_gw::response_t::service       );
        lbsgw_response.set("code"          , &lbs_gw::response_t::code          );
        lbsgw_response.set("age"           , &lbs_gw::response_t::age           );
        lbsgw_response.set("lac"           , &lbs_gw::response_t::lac           );
        lbsgw_response.set("cell"          , &lbs_gw::response_t::cell          );
        lbsgw_response.set("eci"           , &lbs_gw::response_t::eci           );
        lbsgw_response.set("msc"           , &lbs_gw::response_t::msc           );
        lbsgw_response.set("vlr"           , &lbs_gw::response_t::vlr           );
        lbsgw_response.set("sgsn"          , &lbs_gw::response_t::sgsn          );
        lbsgw_response.set("mme"           , &lbs_gw::response_t::mme           );
        lbsgw_response.set("error"         , &lbs_gw::response_t::error         );
        lbsgw_response.set("event_time"    , &lbs_gw::response_t::event_time    );
        lbsgw_response.set("map_param_1"   , &lbs_gw::response_t::map_param_1   );
        lbsgw_response.set("map_param_2"   , &lbs_gw::response_t::map_param_2   );
        lbsgw_response.set("map_param_3"   , &lbs_gw::response_t::map_param_3   );
        lbsgw_response.set("ida_age"       , &lbs_gw::response_t::ida_age       );
        lbsgw_response.set("ida_code"      , &lbs_gw::response_t::ida_code      );
        lbsgw_response.set("ida_text"      , &lbs_gw::response_t::ida_text      );
        lbsgw_response.set("ida_eci"       , &lbs_gw::response_t::ida_eci       );
        lbsgw_response.set("ida_mme_host"  , &lbs_gw::response_t::ida_mme_host  );
        lbsgw_response.set("ida_mme_realm" , &lbs_gw::response_t::ida_mme_realm );

        return ok;
    }
}