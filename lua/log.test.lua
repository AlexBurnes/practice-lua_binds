local log = require("log")
local lu = require("luaunit", "3.4.1")

TestLog = {}

    function TestLog:test_log_version()
        lu.assertEquals(log.version, "0.2.0")
    end

    function TestLog:test_log_output()
        log.lg("test log")
        lu.assertFalse(false)
    end

    function TestLog:test_log_format_string()
        local s = "string"
        log.lg("test string '%s'", s)
        lu.assertFalse(false)
    end

    function TestLog:test_log_format_int()
        local n = 42
        log.lg("test int %d", n)
        lu.assertFalse(false)
    end

    function TestLog:test_log_debug()
        log.ld("test debug")
        lu.assertFalse(false)
    end

    function TestLog:test_log_warning()
        log.lw("test warning")
        lu.assertFalse(false)
    end

    function TestLog:test_log_error()
        log.le("test error")
        lu.assertFalse(false)
    end

lu.LuaUnit.verbosity = 2
return lu.LuaUnit.run()