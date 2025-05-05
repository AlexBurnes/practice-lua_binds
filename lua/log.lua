--- Logging

local log = {}

---@type string module name
log.module = "log"
---@type string module version
log.version = "0.2.0"

---Output formatted log message to console or logfile
---@param fmt string printf format
---@varags formattable values
function log.lg(fmt, ...)
    lg_(string.format(fmt, ...))
end

--- Output formatted debug message to console or logfile
---@param fmt string printf format
---@varags formattable values
function log.ld(fmt, ...)
    ld_(string.format(fmt, ...))
end

--- Output formatted warning message to console or logfile
---@param fmt string printf format
---@varags formattable values
function log.lw(fmt, ...)
    lw_(string.format(fmt, ...))
end

--- Output formatted error message to console or logfile
---@param fmt string printf format
---@varags formattable values
function log.le(fmt, ...)
    le_(string.format(fmt, ...))
    return ERROR
end

return log

