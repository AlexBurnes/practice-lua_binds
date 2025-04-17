#include "lua/state.hpp"
#include "lua/bind.hpp"
#include "lua/module.hpp"
#include "context.hpp"

namespace Lua {

    int State::init(std::string script_code) {
        lua.open_libraries(
            sol::lib::base, sol::lib::jit, sol::lib::string, sol::lib::coroutine, sol::lib::math, sol::lib::os,
            sol::lib::table, sol::lib::io, sol::lib::bit32, sol::lib::ffi
        );
        lua.set_exception_handler(&exception_handler);

        lua.set_function("throw", [](){throw std::runtime_error("exception!");});
        lua.set_function("require", [this] (std::string module, std::string version) -> sol::table& {
            ld(1, "require module {} version {}", module, version);
            auto module_key = fmt::format("{}:{}", module, version);
            if (loaded_modules.count(module_key)) {
                lw("requied module {} version {} already loaded, check loop dependency", module, version);
                return loaded_modules[module_key];
            }
            auto module_code = load_module(module);
            if (module_code == "") {
                le("required module {} version {} is not loaded into database", module, version);
                throw std::runtime_error("exception!");
            }
            auto load_result = lua.script(*module_code, [](lua_State*, sol::protected_function_result pfr){
                return pfr;
            });
            unless (load_result.valid()) {
                sol::error err = load_result;
                le("failed load module {}, error: {}", module, err.what());
                throw std::runtime_error("exception!");
            }
            try {
                loaded_modules[module_key] = load_result;
                return loaded_modules[module_key];
            }
            catch (...) {
                le("failed load lua module '{}', wrong return, expect lua module", module);
                throw std::runtime_error("exception!");
            }
        });

        unless (binds(lua, readonly_metatable)) {
            le("failed bind userdata structures");
            return error;
        }

        unless (load(script_code)) {
            return error;
        }

        unless (bind_functions(lua)) {
            le("failed bind userdata structures");
            return error;
        }


        return ok;
    }

} // namespace Lua
