---Build in module C++ functions, this module define wrapper funcrion for them
local func = {}

---@type string module name
func.module = "func"
---@type string module version
func.version = "0.1.0"

local log = require("log", '0.2.0')

---@note changing this function require changes in C++

---@async
---@param active bool
---@param cause CauseType
---@param fmt string format
---@params any
---@return LogicResult
function func.relay(active, cause, fmt, ...)
    relay_(ctx, active, cause, string.format(fmt, ...))
    return func.await(Return.Ok, "relay");
end

---@async
---@param active bool
---@param cause CauseType
---@param fmt string format
---@params any
---@return LogicResult
function func.reject(active, cause, fmt, ...)
    reject_(ctx, active, cause, string.format(fmt, ...))
    return func.await(Return.Ok, "reject");
end

---@param network_type NetworkType
---@param node string
---@param source string
---@param node_id int out
---@param country_id int out
---@param operator_id int out
---@param strict bool, source mode true stricted, false relaxed
---@return LogicResult
---| Return.Ok when node found
---| Return.Error otherwise
---@return node_id int when found | -1 otherwise
---@return operator_id int when found| -1 otherwise
---@return country_id int when found | -1 otherwise
function func.vlr_search(network_type, node, source)
    local rc, node_id, operator_id, country_id = vlr_search_(ctx, network_type, node, source, false)
    log.lg("vlr search result %d, node %d operator %d country %d", rc, node_id, operator_id, country_id)
    if rc then
        return Return.Error, node_id, operator_id, country_id
    end
    return Return.Ok, node_id, operator_id, country_id
end

---@param network_type NetworkType
---@param mcc string
---@param mnc string
---@param mme_host string
---@param mme_realm string
---@param node_id int out
---@param country_id int out
---@param operator_id int out
---@param strict bool, source mode true stricted, false relaxed
---@return LogicResult
---| Return.Ok when node found
---| Return.Error otherwise
---@return node_id int when found | -1 otherwise
---@return operator_id int when found| -1 otherwise
---@return country_id int when found | -1 otherwise
function func.mcc_search(mcc, mnc, mme_host, mme_realm)
    local rc, node_id, operator_id, country_id = mcc_search_(ctx, mcc, mnc, mme_host, mme_realm)
    log.lg("mcc search result %d, node %d operator %d country %d", rc, node_id, operator_id, country_id)
    if rc then
        return Return.Error, node_id, operator_id, country_id
    end
    return Return.Ok, node_id, operator_id, country_id
end

---@async in C++
---@return Return
function func.profile_lock_and_get()
    return profile_lock_and_get_(ctx)
end

---@param cause CauseType
---@param fmt string format
---@params any
---@return Return
function func.send_relay(cause, fmt, ...)
    return send_relay_(ctx, cause, string.format(fmt, ...))
end

---@param result_code int
---@param event_time timespec
---@return Return Ok|Error
function func.save(result_code, event_time)
    return save_(ctx, result_code, event_time)
end

---@async
---@params variadic
---@return rc Return
---@return what What
---@return event Event
---@return table<any> depends on What and Event
function func.await(...)
    local result = table.pack(coroutine.yield(...))
    local rc = table.remove(result, 1)
    local what = table.remove(result, 1)
    local event = table.remove(result, 1)
    return rc, what, event, result
end

function func.check_session_out()
    return check_session_out_(ctx)
end

function func.time_to_string(time_t)
    return time_to_string_(time_t)
end

function func.local_time(time_t, offset)
    return local_time_(time_t, offset)
end

function func.imsi_white_by_prefix(imsi)
    return imsi_white_by_prefix_(steer_data._data, imsi)
end

function func.imsi_white_by_range(imsi)
    return imsi_white_by_range_(steer_data._data, imsi)
end

---@param node_type OperatorNodeType
---@param node string
---@returns int|nil  if found return node id, nil either
function func.vlr_white(white, node_type, node)
    return vlr_white_(white, steer_data._data, node_type, node)
end

---@param start_time timespec_t
---@param las_time timespec_t
---@return number duration
function func.session_duration(start_time, last_time)
    return (last_time.sec - start_time.sec) * 1e3 + (last_time.nsec - start_time.nsec) / 1e6
end

function func.lbs_send_request(service_type)
    return lbs_send_request_(ctx, service_type)
end

function func.border_save(cause_type)
    return border_save_(ctx, cause_type)
end

function func.border_search_locations(lac, cell, eci)
    return border_search_locations_(ctx, steer_data._data, lac, cell, eci)
end

function func.rule_contain_locations(rule_locations, found_locations)
    return rule_contain_locations_(rule_locations, found_locations)
end

function func.rule_contain_imsi_groups(rule_groups, imsi_groups)
    return rule_contain_imsi_groups_(rule_groups, imsi_groups)
end

function func.imsi_groups_intersection(rule_groups, imsi_groups)
    return imsi_groups_intersection_(rule_groups, imsi_groups)
end

function func.border_find_high_priority_rule(filter_by_suited_networks, rules, prev_rule_id)
    return border_priority_rule_(filter_by_suited_networks, ctx, rules, prev_rule_id)
end

function func.border_rule_filters_create()
    return border_filters_create_()
end

return func
