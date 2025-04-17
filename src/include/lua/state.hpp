#pragma once

#include <sol/sol.hpp>
#include <thread>
#include <mutex>
#include <log/log.hpp>
#include <typedef.h>

namespace Lua {

    std::string Error(sol::protected_function_result& result);

    sol::protected_function entry_state(sol::state_view lua, std::string entry_function);

    int check_load(sol::protected_function_result load_result);

    int exception_handler(
        lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description
    );

    sol::metatable create_readonly_table(sol::state_view &lua);

    template <typename... T>
    sol::table create_readonly_table_with(sol::state_view &lua, sol::metatable& metatable, T&&... args) {
        auto table = lua.create_table_with(args...);
        table[sol::metatable_key] = metatable;
        return table;
    }


    class Coroutine {
        private:
            class StateVew;

        public:
            std::mutex &mutex_;
            sol::table request;

            Coroutine(std::mutex &mutex, sol::state& lua, const char* entry_function)
            :
                mutex_{mutex}, thread_{sol::thread::create(lua.lua_state())}, lua_{thread_.state()},
                coroutine_{entry_state(lua_, entry_function)}
            {}
            ~Coroutine() {};
            sol::coroutine& operator()(void) {return coroutine_;}
            bool runnable() {return coroutine_.runnable();}

            template <typename ...T>
            sol::protected_function_result operator()(T&& ...args) {
                std::lock_guard<std::mutex> state_guard(mutex_);
                return coroutine_(args...);
            }

            sol::state_view& lua() {
                return lua_;
            }

            decltype(auto) status() {
                return coroutine_.status();
            }

            size_t memory_used() {
                std::lock_guard<std::mutex> state_guard(mutex_);
                return lua_.memory_used();
            }
            void collect_garbage() {
                std::lock_guard<std::mutex> state_guard(mutex_);
                lua_.collect_garbage();
            }

        private:
            sol::thread thread_;
            sol::state_view lua_;
            sol::coroutine coroutine_;

    };

    class State {
        public:
            std::mutex mutex_;

            State() {
                readonly_metatable = Lua::create_readonly_table(lua);
            }
            ~State() {}

            sol::state& operator()(void){return lua;}

            Coroutine* coroutine(const char* entry_function) {
                ld(3, "create coroutine entry function '{}'", entry_function);
                std::lock_guard<std::mutex> state_guard(mutex_);
                auto coroutine_ = new Coroutine(mutex_, lua, entry_function);
                return coroutine_;
            }

            int init(std::string script_code);

            std::string& version(void) {return script_version;};
            int load(std::string& script_code);

            template <typename... T>
            sol::table create_readonly_table_with(T&&... args) {
                auto table = lua.create_table_with(args...);
                table[sol::metatable_key] = readonly_metatable;
                return table;
            }

            template <typename... T>
            sol::table create_readonly_table_with(sol::metatable& metatable, T&&... args) {
                auto table = lua.create_table_with(args...);
                table[sol::metatable_key] = metatable;
                return table;
            }

            int check_load(sol::protected_function_result load_result);

            size_t memory_used();

            void garbage_collect() {
                std::lock_guard<std::mutex> state_guard(mutex_);
                lua.collect_garbage();
            }

        private:
            sol::state lua;
            sol::metatable readonly_metatable;

            std::string script_version;

            hash_map<std::string, sol::table> loaded_modules;

            int version(sol::state &lua);


    };

}
