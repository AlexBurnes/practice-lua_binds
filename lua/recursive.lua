--- Testing lua script dependency

local recursive = {}

---@type string module name
recursive.module = "recursive"
---@type string module version
recursive.version = "0.1.0"

---@require log module
recursive.log = require("log", "0.2.0")

return recursive