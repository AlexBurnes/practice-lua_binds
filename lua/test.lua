test = {}

local log = require("log", "0.2.0")

test.module = "test"
test.version = "0.1.0"

    function test:Some_Test_Function()
    --[[
        Multiline comment for testing covering

    ]]--
        obj = some.new();
        some_function("test", "value")
        log.le("some code that executing")
        return ok
    end

    --FIXME benchmark both implementations

    function test:for_repeat(n)
        -- return count of odd numbers from 1 to n, but s <= 4
        -- continue and break implementation without goto
        local s = 0
        local complete for i = 1, n do repeat
            if i % 2 == 0 then
                -- continue
                break
            end
            if s >= 4 then
                -- break
                complete = true break
            end
            s = s + 1
        until true if complete then break end end
        return s
    end

    function test:for_complete(n)
        -- return count of odd numbers from 1 to n, but s <= 4
        -- implementation using lables and goto
        local s = 0
        repeat for i = 1, n do
            if i % 2 == 0 then
                -- continue
                goto loop
            end
            if s >= 4 then
                -- break
                goto complete
            end
            s = s + 1
        ::loop:: end ::complete:: until true
        return s
    end

    function test:for_continue(n)
        -- return count of odd numbers from 1 to n, but s <= 4
        -- but break exists so this implementation using one lable and goto
        local s = 0
        for i = 1, n do
            if i % 2 == 0 then
                --continue, why is 'loop', couse 'loop end' has meaning and readed as end of the loop
                goto loop
            end
            if s >= 4 then
                break
            end
            s = s + 1
        ::loop:: end
        return s
    end


return test
