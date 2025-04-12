local steer = {}

---@type string module name
steer.module="steer"
---@type string module version
steer.version="0.1.0"

---@require log
log = require("log", "0.2.0")
---@require dump
dump = require("dump", "0.2.0")
---@require func
func = require("func", "0.1.0")

---@require steerdata
---steer_data = require("steerdata", "0.1.0")

--[[

    Steer roam logic

--]]


return steer