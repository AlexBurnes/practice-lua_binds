local R = require("recursive")
local lu = require("luaunit", "3.4.1")

TestRecursive = {}

    function TestRecursive:test_version()
        lu.assertEquals(R.version, "0.1.0")
    end

    function TestRecursive:test_recursive()
        lu.assertNotNil(R.log.version)
    end

lu.LuaUnit.verbosity = 2
return lu.LuaUnit.run()