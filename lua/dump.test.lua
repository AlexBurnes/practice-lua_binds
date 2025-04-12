local dump = require("dump")
local lu = require("luaunit", "3.4.1")
local log = require("log", '0.2.0')

TestDump = {}

    function TestDump:test_dump_string()
        local t = "string"
        local s = dump.dump(t)
        lu.assertNotNil(s)
        lu.assertStrContains(s, "variable: 'string'")
    end

    function TestDump:test_dump_number()
        local t = 42
        local s = dump.dump(t)
        lu.assertNotNil(s)
        lu.assertStrContains(s, "variable: 42")
    end

    function TestDump:test_dump_table()
        local t = {}
        t["s"] = "string"
        t["n"] = 42
        local s = dump.dump(t)
        lu.assertStrContains(s, "s: 'string'")
        lu.assertStrContains(s, "n: 42")
        lu.assertNotNil(s)
    end

    function TestDump:test_dump_array()
        local t = {}
        table.insert(t, 1)
        table.insert(t, 2)
        table.insert(t, 3)
        local s = dump.dump(t)
        lu.assertNotNil(s)
        lu.assertStrContains(s, "1: 1")
        lu.assertStrContains(s, "2: 2")
        lu.assertStrContains(s, "3: 3")
    end

    function TestDump:test_empty_table()
        local t = {}
        local s = dump.dump(t)
        lu.assertNotNil(s)
    end

    function TestDump:test_container()
        local s = dump.dump(ctx.profiles.profile.network_attempts)
        lu.assertNotNil(s)
        lu.assertStrContains(s, "1: 1")
        lu.assertStrContains(s, "2: 2")
        lu.assertStrContains(s, "3: 3")
        lu.assertStrContains(s, "4: 4")
        lu.assertStrContains(s, "5: 5")
    end

    function TestDump:test_dump_function()
        local t = {}
        t["f"] = function(...) end
        local s = dump.dump(t)
        lu.assertStrContains(s, "f: function")
        lu.assertNotNil(s)
    end

    function TestDump:test_dump_userdata()
        -- ctx is global variable is location update context
        local s = dump.dump(ctx)
        lu.assertNotNil(s)
        lu.assertStrContains(s, "variable: LocationUpdateContext")
        lu.assertStrContains(s, "imsi: '123456789012345'")
    end

    function TestDump:test_dump_nil()
        local s = dump.dump(nil)
        lu.assertNotNil(s)
        lu.assertStrContains(s, "variable: nil")
    end

    function TestDump:test_dump_log()
        local s = dump.dump(ctx, log.lg)
        lu.assertNil(s)
    end

lu.LuaUnit.verbosity = 2
return lu.LuaUnit.run()