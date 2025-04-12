local t = require('test')
local lu = require('luaunit', '3.4.1')

TestTest = {}

    function TestTest:test_version()
        lu.assertEquals(t.version, '0.1.0')
    end

    function TestTest:test_some_test_function()
        local rc = test.Some_Test_Function()
        lu.assertEquals(rc, ok)
    end

    function TestTest:test_coroutine_wrap()
        local status, rc = coroutine.wrap(function() return 1 end)()
        lu.assertEquals(status, true)
        lu.assertEquals(rc, 1)
    end

    function TestTest:test_coroutine_create()
        local co = coroutine.create(function(rc) return rc end)
        lu.assertIsCoroutine(co)
    end

    function TestTest:test_coroutine_resume()
        local co = coroutine.create(function(rc) return rc end)
        local status, rc = coroutine.resume(co, 42)
        lu.assertEquals(status, true)
        lu.assertEquals(coroutine.status(co), "dead")
        lu.assertEquals(rc, 42)
    end

    function TestTest:test_coroutine_yield()
        local co = coroutine.create(function(rc) local crc = coroutine.yield(rc) return crc end)
        local status, rc = coroutine.resume(co, 2)
        lu.assertEquals(status, true)
        lu.assertEquals(coroutine.status(co), "suspended")
        lu.assertEquals(rc, 2)
        status, rc = coroutine.resume(co, 3)
        lu.assertEquals(rc, 3)
        lu.assertEquals(status, true)
        lu.assertEquals(coroutine.status(co), "dead")
    end

    function TestTest:test_coroutine_yield_redefine()
        coroutine.yield = function(...) return ... end
        local rc = coroutine.yield(4)
        lu.assertEquals(rc, 4)
    end

    function TestTest:test_coroutine_yield_redefine_with_create()
        coroutine.yield = function(...) return ... end
        local co = coroutine.create(function(rc) local crc = coroutine.yield(rc) return crc end)
        local status, rc = coroutine.resume(co, 2)
        lu.assertEquals(status, true)
        lu.assertEquals(coroutine.status(co), "suspended")
        lu.assertEquals(rc, 2)
        status, rc = coroutine.resume(co, 3)
        lu.assertEquals(rc, 3)
        lu.assertEquals(status, true)
        lu.assertEquals(coroutine.status(co), "dead")
    end

    function TestTest:test_for_repeat()
        lu.assertEquals(t:for_repeat(5), 3)
        lu.assertEquals(t:for_repeat(6), 3)
        lu.assertEquals(t:for_repeat(7), 4)
        lu.assertEquals(t:for_repeat(10), 4)
        lu.assertEquals(t:for_repeat(100), 4)
    end

    function TestTest:test_for_complete()
        lu.assertEquals(t:for_complete(5), 3)
        lu.assertEquals(t:for_complete(6), 3)
        lu.assertEquals(t:for_complete(7), 4)
        lu.assertEquals(t:for_complete(10), 4)
        lu.assertEquals(t:for_complete(100), 4)
    end


    function TestTest:test_for_continue()
        lu.assertEquals(t:for_continue(5), 3)
        lu.assertEquals(t:for_continue(6), 3)
        lu.assertEquals(t:for_continue(7), 4)
        lu.assertEquals(t:for_continue(10), 4)
        lu.assertEquals(t:for_continue(100), 4)
    end

lu.LuaUnit.verbosity = 2
return lu.LuaUnit.run()