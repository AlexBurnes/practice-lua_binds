---Dump lua variable, tables and userdata types in tree format
local dump = {}

---@type string module name
dump.module = "dump"
---@type string module version
dump.version = "0.2.0"

    ---@param fn function
    ---@return object of Dump class
    function dump:new(fn)
        o = o or {}
        setmetatable(o, self)
        self.__index = self
        self.l = lvs.new()
        self.msg = nil
        if (fn) then
            self.fn = fn
        else
            self.msg = ""
            self.fn = function(s) self.msg = self.msg .. s .. "\n" return end
        end
        return self
    end

    ---@param s string
    ---@return type returns by self.fn
    function dump:collect(s)
        return self.fn(s)
    end

    ---@private
    ---@param l lvs
    ---@param level integer
    ---@param name string
    ---@param var table
    ---@param last boolean
    ---@return nil
    function dump:dump_table(level, name, var, last)
        self:collect(string.format("%s %s:", self.l:func(level, last), name))
        local f, t, k = pairs(var)
        local v
        local last_ = false
        k, v = f(t, k)
        if (k == nil) then
            last_ = true;
        end
        while (k ~= nil) do
            local k_, v_ = f(t, k)
            if (k_ == nil) then
                last_ = true;
            end
            --FIXME avoid recurse
            if (k ~= "__index") then
                self:dmp(level+1, k, v, last_)
            end
            k, v = f(t, k)
        end
        return
    end

    ---@private
    ---@param l lvs
    ---@param level integer
    ---@param name string
    ---@param var userdata type of binded sol::container any C++ stl compatible iterated container
    ---@param last boolean
    ---@return nil
    function dump:dump_container(level, name, var, last)
        self:collect(string.format("%s %s (%d):", self.l:func(level, last), name, var:size()))
        local f, t, k = var:pairs()
        local v
        local last_ = false
        k, v = f(t, k)
        if (k == nil) then
            last_ = true;
        end
        while (k ~= nil) do
            local k_, v_ = f(t, k)
            if (k_ == nil) then
                last_ = true;
            end
            self:dmp(level+1, k, v, last_)
            k, v = k_, v_
        end
        return
    end

    ---@private
    ---@param l lvs
    ---@param level integer
    ---@param name string
    ---@param var userdata
    ---@param last boolean
    ---@return nil
    function dump:dump_userdata(level, name, var, last)
        local metatable = getmetatable(var)
        if (metatable == nil) then
            return
        end
        local type_name = ""
        if (metatable["__type"] ~= nil) then
            type_name = metatable["__type"]["name"]
        end
        self:collect(string.format("%s %s: %s", self.l:func(level, last), name, type_name))
        local f, t, k = pairs(metatable)
        local v
        if (t == nil) then
            return
        end

        local filtered = {}
        k, v = f(t, k)
        while (k ~= nil) do
            if (string.sub(k, 1, 1) == "_" or k == "new" or k == nil) then
                goto filter_next
            end
            if (type(v) == "userdata") then
                local v_meta = getmetatable(v)
                if (v_meta == nil) then
                    goto filter_next
                end
            end
            filtered[k] = v
            ::filter_next::
            k, v = f(t, k)
        end

        f, t, k = pairs(filtered)
        k, v = f(t, k)
        local last_ = false
        if (k == nil) then
            last_ = true;
        end
        while (k ~= nil) do
            local k_, v_ = f(t, k)
            if (k_ == nil) then
                last_ = true
            end
            local var_type = type(v)
            if (var_type == "function") then
                self:dmp(level+1, k, var[k], last_)
            else
                self:dmp(level+1, k, v, last_)
            end
            k, v = f(t, k)
        end
        return
    end

    ---@private
    ---@param l lvs
    ---@param level integer
    ---@param name string
    ---@param var string
    ---@param last boolean
    ---@return nil
    function dump:dmp(level, name, var, last)
        local var_type = type(var)

        if (var_type == "string") then
            return self:collect(string.format("%s %s: '%s'", self.l:func(level, last), name, var))
        end
        if (var_type == "number") then
            return self:collect(string.format("%s %s: %d", self.l:func(level, last), name, var))
        end
        if (var_type == "boolean") then
            return self:collect(string.format("%s %s: %s", self.l:func(level, last), name, tostring(var)))
        end
        if (var_type == "nil") then
            return self:collect(string.format("%s %s: nil", self.l:func(level, last), name))
        end

        if (var_type == "table") then
            return self:dump_table(level, name, var, last)
        end

        if (var_type == "userdata") then
            -- detect type of userdata, is it a container?
            local var_meta = getmetatable(var)
            if (var_meta["pairs"] ~= nil and type(var_meta["pairs"]) == "function") then
                -- type of container at var_meta["__name"]
                return self:dump_container(level, name, var, last)
            end
            return self:dump_userdata(level, name, var, last)
        end

        return self:collect(string.format("%s %s: %s", self.l:func(level, last), name, var_type))
    end

    ---@param var type variable of any type
    ---@param fn function optional to output dump data to it line by line
    ---@return string tree name value representation of variable
    function dump.dump(var, fn)
        local dumper = dump:new(fn)
        dumper:dmp(0, "variable", var, true)
        return dumper.msg;
    end

return dump