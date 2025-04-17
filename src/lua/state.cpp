#include "lua/state.hpp"

namespace Lua {

    sol::protected_function entry_state(sol::state_view lua, std::string entry_function) {
        sol::protected_function throw_function = lua["throw"];
        auto entries = utils::split(entry_function, '.');
        if (entries.size()==1) return lua[entry_function];
        int i = 0;
        sol::table table;
        for(auto &entry : entries) {
            i++;
            ld(1, "get state for entry {}", entry);
            if (i == entries.size()) {
                return table[entry];
            }
            table = lua[entry];
            if (table == sol::nil) {
                le("failed found entry '{}'", entry_function);
                return throw_function;
            }
        }
        return throw_function;
    }

    std::string Error(sol::protected_function_result& result) {
        sol::error err = result;
        le("lua error: {}", err.what());
        return err.what();
    }

    //https://sol2.readthedocs.io/en/latest/exceptions.html
    int exception_handler(
        lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description
    ) {
        if (maybe_exception) {
            const std::exception& ex = *maybe_exception;
            le("An exception occurred, exception '{}'", ex.what());
        }
        else {
            std::string desc{description.data(), description.size()};
            le("An exception occurred, description '{}'", desc);
        }
        return sol::stack::push(L, description);
    }

    int check_load(sol::protected_function_result load_result) {
        unless (load_result.valid()) {
            sol::error err = load_result;
            le("failed load error: {}", err.what());
            return ::error;
        }
        return ok;
    }

    sol::metatable create_readonly_table(sol::state_view &lua) {
        auto readonly_metatable = lua.create_table_with();
        readonly_metatable[sol::meta_function::new_index] = [](lua_State* L){
            return luaL_error(L, "attempt to set readonly property");
        };
        readonly_metatable[sol::meta_function::index] = readonly_metatable;
        return readonly_metatable;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // load and check lua logic script
    int State::load(std::string &script_code) {

        auto load_result = lua.script(*script_code, [](lua_State*, sol::protected_function_result pfr){
            return pfr;
        });
        unless (load_result.valid()) {
            sol::error err = load_result;
            le("failed load logic lua script, error: {}", err.what());
            return ::error;
        }
        try {
            sol::table table = load_result;
            lua["lur"] = load_result;
            return ok;
        }
        catch (...) {
            le("failed load lua module 'lur', wrong return, expect lua module");
            throw std::runtime_error("exception!");
        }
        return error;
    }

    int State::check_load(sol::protected_function_result load_result) {
        unless (load_result.valid()) {
            sol::error err = load_result;
            le("failed load error: {}", err.what());
            return ::error;
        }
        return ok;
    }

    int State::version(sol::state &lua) {
        std::lock_guard<std::mutex> state_guard(mutex_);
        lua["_SOL_VERSION"]=SOL_VERSION_STRING;
        auto load_result = lua.script(R"(
            local jit_version
            if type(jit) == 'table' then jit_version = jit.version else jit_version = "not loaded" end
            return "logic "..lur.version..", lua ".._VERSION..", sol2 ".._SOL_VERSION..", jit "..jit_version

        )");
        unless (load_result.valid()) {
            sol::error err = load_result;
            le("failed get version of lua logic script, error: {}", err.what());
            return ::error;
        }

        std::string full_version = load_result;
        lg(1, "loaded {}", full_version);

        load_result = lua.script(R"(
            return lur.version
        )");
        unless (load_result.valid()) {
            sol::error err = load_result;
            le("failed get version of lua logic script, error: {}", err.what());
            return ::error;
        }

        script_version = load_result;
        return ok;
    }

    size_t State::memory_used() {
        std::lock_guard<std::mutex> state_guard(mutex_);
        return lua.memory_used();
    }


}
