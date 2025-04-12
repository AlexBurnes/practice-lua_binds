#include <sol/sol.hpp>
#include "lua/bind.hpp"

namespace Lua {

    int bind_enums(sol::state_view &lua, sol::metatable& readonly_metatable) {
        //FIXME declare enums, but some steering values are not enums :) a just struct with static numbers
        //lua.new_enum<my_enum>( "my_enum", { {"a", my_enum:: a}, { "b", my_enum::b } } );.

        lua.new_enum<LogicReturn>("Return", {
            { "Error",    LogicReturn::Error    },
            { "Ok",       LogicReturn::Ok       },
            { "Failure",  LogicReturn::Failure  },
            { "Continue", LogicReturn::Continue }
        });

        lua.new_enum<LogicWhat>("What", {
            { "Module",   LogicWhat::Module   },
            { "Coren",    LogicWhat::Coren    },
            { "Steering", LogicWhat::Steering },
            { "Diameter", LogicWhat::Diameter },
            { "LBSgw",    LogicWhat::LBSgw    },
            { "DBCache",  LogicWhat::DBCache  },
            { "Profile",  LogicWhat::Profile  },
            { "Counters", LogicWhat::Counters },
        });

        lua.new_enum<LogicEvent>("Module", {
            {"Result"        , LogicEvent::Result        },
            {"NodeSearch"    , LogicEvent::NodeSearch    },
            {"ProfileLock"   , LogicEvent::ProfileLock   },
            {"ProfileUnLock" , LogicEvent::ProfileUnLock },
            {"CountersGet"   , LogicEvent::CountersGet   }
        });

        lua.new_enum<::steering::Event::Value>("Steering", {
            { "LocationUpdate"     , ::steering::Event::Value::LocationUpdate        },
            { "LogicAnswer"        , ::steering::Event::Value::LogicAnswer           },
            { "SubscriberRequest"  , ::steering::Event::Value::SubscriberDataRequest },
            { "SubscriberResponse" , ::steering::Event::Value::SubscriberDataAnswer  },
            { "LocationResult"     , ::steering::Event::Value::LocationResult        }
        });

        lua.new_enum<::steeringDiameter::Event::Value>("Diameter", {
            { "LocationUpdate"     , ::steeringDiameter::Event::Value::Request               },
            { "LogicAnswer"        , ::steeringDiameter::Event::Value::Response              },
            { "LocationResult"     , ::steeringDiameter::Event::Value::Result                },
            { "SubscriberRequest"  , ::steeringDiameter::Event::Value::SubscriberDataRequest },
            { "SubscriberResponse" , ::steeringDiameter::Event::Value::SubscriberDataAnswer  },
            { "ErrorMessage"       , ::steeringDiameter::Event::Value::ErrorMessage          }
        });

        lua.new_enum<RuleParamTypes>("RuleParamTypes", {
            { "OneNetworkAlive"                  , RuleParamTypes::OneNetworkAlive                  },
            { "FirstTimeinCountry"               , RuleParamTypes::FirstTimeinCountry               },
            { "MaxRedirectionTimePerSession"     , RuleParamTypes::MaxRedirectionTimePerSession     },
            { "MaxULRejectsPerSeries4OneNodeLTE" , RuleParamTypes::MaxULRejectsPerSeries4OneNodeLTE },
            { "MaxULRejectsPerSeries4OneNode"    , RuleParamTypes::MaxULRejectsPerSeries4OneNode    },
            { "MaxNetworkRejects"                , RuleParamTypes::MaxNetworkRejects                },
            { "TimeoutBetweenRedirectionSession" , RuleParamTypes::TimeoutBetweenRedirectionSession },
            { "T1_mode"                          , RuleParamTypes::T1_mode                          },
            { "BorderRoamLBSTimeout"             , RuleParamTypes::BorderRoamLBSTimeout             },
            { "BorderRoamRestrictTimeout"        , RuleParamTypes::BorderRoamRestrictTimeout        },
            { "BorderRoamMaxRejects"             , RuleParamTypes::BorderRoamMaxRejects             },
            { "BorderRoamMaxRequests"            , RuleParamTypes::BorderRoamMaxRequests            },
            { "CS_alive"                         , RuleParamTypes::CS_alive                         },
            { "PS_alive"                         , RuleParamTypes::PS_alive                         },
            { "LTE_alive"                        , RuleParamTypes::LTE_alive                        },
            { "IDR_alive"                        , RuleParamTypes::IDR_alive                        }
        });

        lua.new_enum<Steering::RegistrationType>( "SteeringRegistrationType", {
            { "gsm_gprs", Steering::RegistrationType::gsm_gprs },
            { "lte",      Steering::RegistrationType::lte }
        });

        lua["OperatorNodeType"] = create_readonly_table_with(lua, readonly_metatable,
              "VLR"  , OperatorNodeType::VLR
            , "GT"   , OperatorNodeType::GT
            , "SGSN" , OperatorNodeType::SGSN
            , "MMEH" , OperatorNodeType::MMEH
            , "MMER" , OperatorNodeType::MMER
            , "HLR"  , OperatorNodeType::HLR
            , "HSS"  , OperatorNodeType::HSS
            , "MME"  , OperatorNodeType::MME
            , "HUB"  , OperatorNodeType::HUB
        );

        lua["OperatorNodeSource"] = create_readonly_table_with(lua, readonly_metatable,
              "Admin"  , OperatorNodeSource::Admin
            , "System" , OperatorNodeSource::System
            , "Upload" , OperatorNodeSource::Upload
        );

        lua["OpCodeType"] = create_readonly_table_with(lua, readonly_metatable,
              "GSM_CS" , OpCodeType::GSM_CS
            , "GSM_PS" , OpCodeType::GSM_PS
            , "LTE"    , OpCodeType::LTE
        );

        lua["NetworkType"] = create_readonly_table_with(lua, readonly_metatable,
              "GSM_CS"  , NetworkType::GSM_CS
            , "GSM_PS"  , NetworkType::GSM_PS
            , "LTE"     , NetworkType::LTE
            , "GPRS_5G" , NetworkType::GPRS_5G
        );

        lua["PriorityType"] = create_readonly_table_with(lua, readonly_metatable,
              "Prefered"         , PriorityType::Prefered
            , "Unwellcom"        , PriorityType::Unwellcom
            , "Forbidden"        , PriorityType::Forbidden
            , "PreferedPercent"  , PriorityType::PreferedPercent
            , "UnwellcomPercent" , PriorityType::UnwellcomPercent
        );

        lua["RuleType"] = create_readonly_table_with(lua, readonly_metatable,
              "Prefered"   , RuleType::Prefered
            , "Percentage" , RuleType::Percentage
        );

        lua["ParamValueType"] = create_readonly_table_with(lua, readonly_metatable,
              "Integer" , ParamValueType::Integer
            , "String"  , ParamValueType::String
            , "Bool"    , ParamValueType::Bool
        );

        lua["SubscriberType"] = create_readonly_table_with(lua, readonly_metatable,
              "Outbound" , SubscriberType::Outbound
            , "Inbound"  , SubscriberType::Inbound
        );

        lua["CauseType"] = create_readonly_table_with(lua, readonly_metatable,
              "WhiteImsi"                         , CauseType::WhiteImsi
            , "WhiteNode"                         , CauseType::WhiteNode
            , "NodeNotFound"                      , CauseType::NodeNotFound
            , "RuleNotFound"                      , CauseType::RuleNotFound
            , "ManualModeT1"                      , CauseType::ManualModeT1
            , "FirstCountryRegistration"          , CauseType::FirstCountryRegistration
            , "ChangeNodeAtSingleOperator"        , CauseType::ChangeNodeAtSingleOperator
            , "ManualModeTimeout"                 , CauseType::ManualModeTimeout
            , "MaxRedirectionTimePerSession"      , CauseType::MaxRedirectionTimePerSession
            , "TimeoutBetweenRedirectionSession"  , CauseType::TimeoutBetweenRedirectionSession
            , "AbnormalTimeout"                   , CauseType::AbnormalTimeout
            , "PreferedNetworkPriorityRule"       , CauseType::PreferedNetworkPriorityRule
            , "PreferedNetworkPercentageRule"     , CauseType::PreferedNetworkPercentageRule
            , "CSAlive"                           , CauseType::CSAlive
            , "PSAlive"                           , CauseType::PSAlive
            , "LTEAlive"                          , CauseType::LTEAlive
            , "IDRAlive"                          , CauseType::IDRAlive
            , "UncoveredNetwork"                  , CauseType::UncoveredNetwork
            , "NetworkNotSupported"               , CauseType::NetworkNotSupported
            , "InboundOK"                         , CauseType::InboundOK
            , "InboundNodeNotFound"               , CauseType::InboundNodeNotFound
            , "TimeResync"                        , CauseType::TimeResync
            , "ManualModeK3"                      , CauseType::ManualModeK3
            , "NoRulesForCountry"                 , CauseType::NoRulesForCountry
            , "SoRDisabled"                       , CauseType::SoRDisabled
            , "SoRCountryDisabled"                , CauseType::SoRCountryDisabled
            , "Timeout"                           , CauseType::Timeout
            , "RestrictedNetwork"                 , CauseType::RestrictedNetwork
            , "UnwelcomeNetworkPriorityRule"      , CauseType::UnwelcomeNetworkPriorityRule
            , "UnwelcomeNetworkPercentageRule"    , CauseType::UnwelcomeNetworkPercentageRule
            , "WrongLU_NoIMSI"                    , CauseType::WrongLU_NoIMSI
            , "WrongLU_NoNode"                    , CauseType::WrongLU_NoNode
            , "WrongLU_IncorrectNode"             , CauseType::WrongLU_IncorrectNode
            , "LockImsi"                          , CauseType::LockImsi
            , "LogicError"                        , CauseType::LogicError
            , "BorderRoaming"                     , CauseType::BorderRoaming
            , "OperatorIsClosed"                  , CauseType::OperatorIsClosed
            , "ULRTimeout"                        , CauseType::ULRTimeout
            , "RedisError"                        , CauseType::RedisError
        );

        lua["LBSCauseType"] = create_readonly_table_with(lua, readonly_metatable,
              "Success"                   , LBSCauseType::Success
            , "InternalError"             , LBSCauseType::InternalError
            , "CoreNError"                , LBSCauseType::CoreNError
            , "NodeNotFound"              , LBSCauseType::NodeNotFound
            , "AgeTooOld"                 , LBSCauseType::AgeTooOld
            , "ATITimeout"                , LBSCauseType::ATITimeout
            , "ImsiUnknown"               , LBSCauseType::ImsiUnknown
            , "NetworkError"              , LBSCauseType::NetworkError
            , "ProfileChanged"            , LBSCauseType::ProfileChanged
            , "ProfileNotReady"           , LBSCauseType::ProfileNotReady
            , "UnexpectedLBSResponce"     , LBSCauseType::UnexpectedLBSResponce
            , "WrongMapCode"              , LBSCauseType::WrongMapCode
            , "WrongAge"                  , LBSCauseType::WrongAge
            , "WrongDiameterCode"         , LBSCauseType::WrongDiameterCode
            , "UnexpectedServiceResponse" , LBSCauseType::UnexpectedServiceResponse
            , "ULProcessingDone"          , LBSCauseType::ULProcessingDone
            , "NoBorderRulesFound"        , LBSCauseType::NoBorderRulesFound
            , "SubsriberNotAtHome"        , LBSCauseType::SubsriberNotAtHome
            , "NoSuitedBorderRule"        , LBSCauseType::NoSuitedBorderRule
            , "BorderRuleWithoutOperator" , LBSCauseType::BorderRuleWithoutOperator
            , "BorderRuleIsAllowed"       , LBSCauseType::BorderRuleIsAllowed
            , "ToManyBorderRejects"       , LBSCauseType::ToManyBorderRejects
            , "ForbiddenByBorderRule"     , LBSCauseType::ForbiddenByBorderRule
            , "UndefinedNode"             , LBSCauseType::UndefinedNode
            , "NoLBSMapData"              , LBSCauseType::NoLBSMapData
            , "NoLTERegistration"         , LBSCauseType::NoLTERegistration
        );

        return ok;
    }
}