#pragma once

#include "redis/redis.hpp"
#include "redis/logic.hpp"
#include "database/redis.hpp"
#include "lua/state.hpp"

struct ModuleT {

    config::config_ptr config = nullptr;
    std::shared_ptr<config::Scheme> config_scheme = nullptr;

    std::string config_file = "/opt/svyazcom/etc/lur.conf";

    bool help = false;

    char* log_file  = nullptr;
    int log_level   = -1;
    int debug_level = -1;
    int log_quiet   = -1;

    std::string module_name;

    Redis redis;
    std::unique_ptr<RedisLogic> logic;

    ComParameters_t com_parameters;
    ComParameterHandlers_t com_parameter_handlers;
    base_changes_t base_changes;
    std::unique_ptr<lur::redis::entity::Cache> cache;

    thread::Event wait;

    struct ev_loop* loop = nullptr;
    struct ev_signal sig_term, sig_int, sig_hup;
    struct ev_async async_timer, async_stop;
    thread::Mutex loop_mutex;

    bool assume_yes = false;
    bool assume_no = false;

    ModuleT()
    {
        init_config_scheme();
    };
    ~ModuleT() {};
    void init_config_scheme();
    static ModuleT& module() {
        static ModuleT module;
        return module;
    }
    ModuleT(ModuleT const&) = delete;
    ModuleT& operator=(ModuleT const&) = delete;
    ModuleT(ModuleT const&&) = delete;
    ModuleT& operator=(ModuleT const&&) = delete;
};

static ModuleT& Module = ModuleT::module();



