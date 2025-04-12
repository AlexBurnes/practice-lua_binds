local func = require("func")
local lu = require("luaunit", "3.4.1")
local log = require("log", '0.2.0')
local dump = require("dump", '0.2.0')
local steer_data = require("steerdata", '0.1.0')

TestFunc = {}

function TestFunc.test_await()
    coroutine.yield = function(...) return ... end
    local rc, what, event, result = func.await(Return.Ok, What.Module, Module.NodeSearch, "testing await");
    log.ld("await result: rc %d, what %d, event %d: result:\n%s", rc, what, event, dump.dump(result))
    lu.assertEquals(rc, Return.Ok)
    lu.assertEquals(what, What.Module)
    lu.assertEquals(event, Module.NodeSearch)
    lu.assertEquals(type(result), "table")
    lu.assertEquals(result[1], "testing await")
end

function TestFunc.test_func_relay()
    coroutine.yield = function(...) return ... end
    local rc, what, event, result = func.relay(true, CauseType.LogicFailure, "test relay %d", 1)
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_func_reject()
    coroutine.yield = function(...) return ... end
    local rc, what, event, result = func.reject(true, CauseType.LogicFailure, "test reject %d", 1)
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_func_vlr_search_not_found()
    local rc, node_id, operator_id, country_id = func.vlr_search(NetworkType.GSM_PS, "12345", "12345")
    log.lg("found result %d (ok %d, error %d) node %d operator %d country %d",
        rc, Return.Ok, Return.Error, node_id, operator_id, country_id
    )
    lu.assertEquals(rc, Return.Error)
    lu.assertEquals(node_id, -1)
    lu.assertEquals(operator_id, -1)
    lu.assertEquals(country_id, -1)
end

function TestFunc.test_func_mcc_search_not_found()
    local rc, node_id, operator_id, country_id = func.mcc_search("12", "123", "lur.test.host", "lur.test.realm")
    log.lg("found result %d (ok %d, error %d) node %d operator %d country %d",
        rc, Return.Ok, Return.Error, node_id, operator_id, country_id
    )
    lu.assertEquals(rc, Return.Error)
    lu.assertEquals(node_id, -1)
    lu.assertEquals(operator_id, -1)
    lu.assertEquals(country_id, -1)
end

function TestFunc.test_profile_lock_and_get()
    local rc = func.profile_lock_and_get()
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_send_relay()
    local rc = func.send_relay(CauseType.SoRDisabled, "test send relay %s", "test")
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_save()
    local rc = func.save(0, ctx.event_time)
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_check_session_out()
    local rc = func.check_session_out()
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_time_to_string()
    local time_string = func.time_to_string(ctx.event_time.sec);
    lu.assertIsTrue(time_string:len() > 0)
end

function TestFunc.test_local_time()
    local time_local = func.local_time(ctx.event_time.sec, 0);
    lu.assertIsTrue(time_local > 0)
end

function TestFunc.test_imsi_white_by_prefix()
    local found = func.imsi_white_by_prefix(ctx.imsi);
    lu.assertEquals(found, nil)
end

function TestFunc.test_imsi_white_by_range()
    local found = func.imsi_white_by_range(ctx.imsi);
    lu.assertEquals(found, nil)
end

function TestFunc.test_vlr_white()
    local found = func.vlr_white(ctx.vlr_white, OperatorNodeType.VLR, "12345676890");
    lu.assertEquals(found, nil)
end

function TestFunc.test_session_duration()
    local duration = func.session_duration(ctx.attempt_time, ctx.attempt_time)
    lu.assertEquals(duration, 0)
end

function TestFunc.test_lbs_send_request()
    local rc = func.lbs_send_request(LBSGWServiceType.Diameter)
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_border_save()
    local rc = func.border_save(LBSCauseType.Success)
    lu.assertEquals(rc, Return.Ok)
end

function TestFunc.test_border_search_locations()
    local found_locations = func.border_search_locations("123", "123", "123")
    log.ld("found_locations %s", dump.dump(found_locations))
    lu.assertEquals(found_locations:size(), 0)
end

function TestFunc.test_rule_contain_locations()
    local found_locations = func.border_search_locations("123", "123", "123")
    local border_rule = border_rule_t:new()
    local contain_location = func.rule_contain_locations(border_rule.locations, found_locations)
    lu.assertEquals(contain_location, false)
end

function TestFunc.test_rule_contain_imsi_groups()
    local rule = rule_t:new()
    local contain_groups = func.rule_contain_imsi_groups(rule.imsi_lists, ctx.profiles.imsi_lists)
    lu.assertEquals(contain_groups, false)
end

function TestFunc.test_imsi_groups_intersection()
    local rule = rule_t:new()
    local group_id = func.imsi_groups_intersection(rule.imsi_lists, ctx.profiles.imsi_lists)
    lu.assertEquals(group_id, 0)
end

function TestFunc.test_border_rule_filters_create()
    local rules_filtered = func.border_rule_filters_create()
    log.ld("rules_filtered: %s", dump.dump(rules_filtered))
    lu.assertNotEquals(rules_filtered, nil)
end

function TestFunc.test_border_find_high_priority_rule()
    local rules_filtered = func.border_rule_filters_create()
    log.ld("rules_filtered: %s", dump.dump(rules_filtered))
    local rc, rule = func.border_find_high_priority_rule(true, rules_filtered, -1)
    lu.assertEquals(rc, Return.Ok)
    lu.assertEquals(rule, nil)
end


lu.LuaUnit.verbosity = 2
return lu.LuaUnit.run()