#include "lua/state.hpp"
#include "lua/bind.hpp"
#include "regexp/regexp.hpp"
#include "file/file.hpp"
#include "script.hpp"

//https://sol2.readthedocs.io/en/latest/exceptions.html
inline void lua_panic(sol::optional<std::string> maybe_message) {
    le("Lua is in a panic state and will now abort() the application");
    if (maybe_message) {
        const std::string& message = maybe_message.value();
        le("exception error message: {}", message);
    }
    // When this function exits, Lua will exhibit default behavior and abort()
}

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

int script_load(const std::string& file_name, const std::string& script_dir, bool test_script, bool show_coverage) {

    Regexp file_re((char *)*file_name);
    unless (file_re(R"(/^(.+\/)*?([^\/\.]+?)\.lua$/)")) {
        le("wrong script file name, expect path/name.lua, got {}", file_name);
        return error;
    }
    std::string script_path = file_re[1];
    std::string script_name = file_re[2];
    ld(1, "script path '{}' name '{}'", script_path, script_name);

    std::string test_module = fmt::format("{}.test.lua", script_name);
    std::string test_name = fmt::format("{}{}", script_path, test_module);

    //!@note load code from script file
    File logic_file;
    unless (logic_file.Open(*file_name)) {
        le("logic script '{}' not found", file_name);
        return error;
    }

    auto logic_code = unique_free_ptr<char>((char *)calloc(logic_file.Size()+1, sizeof(char)), free);

    auto read_size = logic_file.Read(logic_code.get(), logic_file.Size());
    logic_file.Close();
    unless (read_size == logic_file.Size()) {
        le("failed read logic script '{}', readed {}, file size {}", file_name, read_size, logic_file.Size());
        return error;
    }

    /*!@type vector<int, int, string>
     * @field int bitmask
     * | 0 undefined line
     * | 1 linedefinition
     * | 2 last line definition
     * | 4 code run
     * | 8 covered
     * | 16 comment
     * @field int runs counter
     * @field string source code line
    */
    std::vector<std::tuple<int, int, std::string>> source_coverage;
    //!@type char* pointer to current position
    char *p = logic_code.get();
    //!@type char* pointer to line begin
    char *l = p;
    //!@note split source code to lines
    while (*p) {
        if (*p == 0x0d) {
            ++p;
            ++l;
            continue;
        }
        if (*p == 0x0a) {
            ld(3, "[{}] line length {}", source_coverage.size(), ((size_t)(p-l)));
            source_coverage.push_back(std::make_tuple<int, int, std::string>(0, 0, std::string{l, ((size_t)(p-l))}));
            l = ++p;
            continue;
        }
        ++p;
    };
    ld(1, "source {} lines", source_coverage.size());

    //!@note load code from test file, test file is script file_name.test.lua
    File test_file;
    unless (test_file.Open(*test_name)) {
        le("logic script '{}' require test script '{}'", file_name, test_name);
        return error;
    }

    auto test_code = unique_free_ptr<char>((char *)calloc(test_file.Size()+1, sizeof(char)), free);
    read_size = test_file.Read(test_code.get(), test_file.Size());
    test_file.Close();
    unless (read_size == test_file.Size()) {
        le("failed read test script '{}', readed {}, file size {}", test_name, read_size, logic_file.Size());
        return error;
    }


    sol::state lua(sol::c_call<decltype(&lua_panic), &lua_panic>);
    lua.open_libraries(
        sol::lib::base, sol::lib::jit, sol::lib::string, sol::lib::coroutine, sol::lib::math, sol::lib::os,
        sol::lib::table, sol::lib::io, sol::lib::bit32, sol::lib::ffi, sol::lib::debug
    );
    lua.set_exception_handler(&exception_handler);
    lua.set_function("throw", [](){throw std::runtime_error("exception!");});

    lua["_SOL_VERSION"]=SOL_VERSION_STRING;
    auto version_result = lua.script(R"(
        local jit_version
        if type(jit) == 'table' then jit_version = jit.version else jit_version = "not loaded" end
        return _VERSION..", sol2 ".._SOL_VERSION..", jit "..jit_version

    )");
    unless (version_result.valid()) {
        sol::error err = version_result;
        le("failed get version of lua logic script, error: {}", err.what());
        return error;
    }

    std::string full_version = version_result;
    lg(1, "{}", full_version);

    auto readonly_metatable = Lua::create_readonly_table(lua);
    unless(Lua::binds(lua, readonly_metatable)) {
        le("failed bind userdata structures");
        return error;
    }

    unless(Lua::bind_functions(lua)) {
        le("failed bind functions");
        return error;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mock lur main logic script
    auto lur_lua = std::make_shared<Lua::State>();
    unless(lur_lua->init(R"(local lur = {} function lur.main() end return lur)")) {
        le("failed init mock lur logic script");
        return error;
    }

    //C++ require to remember such thigs, what container would contain, and when it's lifetime is over
    hash_map<std::string, sol::table> loaded_modules;
    hash_map<std::string, bool> required_modules;

    auto debug_info = lua["debug"]["getinfo"];

    auto dump_table = [](sol::table &i, int d, std::string_view align) ->void {
        for(auto &[k, v] : i) {
            auto t = v.get_type();
            auto key = k.as<std::string>();
            if (t == sol::type::string) {
                ld(d, "{}{} '{}'", align, key, v.as<std::string>());
                continue;
            }
            if (t == sol::type::number) {
                ld(d, "{}{} '{}'", align, key, v.as<double>());
                continue;
            }
            ld(d, "{}{}", align, key);
        }
    };

    auto set_coverage = [&source_coverage](int line, sol::table &i) -> void{
        auto lines = source_coverage.size();
        std::string source = i["source"];
        std::string name   = i["name"];
        int linedefined    = i["linedefined"];
        int laslinedefined = i["lastlinedefined"];
        ld(3, "{}.{} {} [{}:{}]", source, line, name, linedefined, laslinedefined);
        //!@note remember lua arrays index start from 1!
        if (line > 0 and lines >= line) {
            std::get<0>(source_coverage.at(line-1)) |= 4;
            std::get<1>(source_coverage.at(line-1))++;
        }
        if (linedefined > 0 and lines >= linedefined) {
            std::get<0>(source_coverage.at(linedefined-1)) |= 1;
        }
        if (laslinedefined > 0 and lines >= laslinedefined) {
            std::get<0>(source_coverage.at(laslinedefined-1)) |= 2;
        }
    };

    lua["_load_coverage_hook"] =
        [&debug_info, &script_name, &source_coverage, &set_coverage, &dump_table]
        (sol::object event, int line, int level) {
            sol::table i = debug_info(2, "nS");
            if (i != sol::nil) {
                std::string source = i["source"];
                if (source == script_name) {
                    set_coverage(line, i);
                    dump_table(i, 2, "  ");
                }
            }
        };

    lua["debug"]["sethook"](lua["_load_coverage_hook"], "l");

    auto recursive_level = 0;

    lua.set_function("require",
        [&loaded_modules, &lua, &required_modules, &recursive_level]
        (std::string module, std::string version)
        -> sol::table& {
            ld(1, "require module {} version {}, recursive level {}", module, version, recursive_level);
            auto module_key = fmt::format("{}:{}", module, version);
            if (loaded_modules.count(module)) {
                ld(1, "requied module {} already loaded", module);
                return loaded_modules[module];
            }
            if (loaded_modules.count(module_key)) {
                ld(1, "requied module {} version {} already loaded", module, version);
                return loaded_modules[module_key];
            }


            auto module_code = load_module(module);

            if (module_code == "") {
                le("required module {}-{} is not loaded into database", module, version);
                throw std::runtime_error("exception!");
            }
            recursive_level++;
            auto load_result = lua.script(*module_code, [](lua_State*, sol::protected_function_result pfr){
                return pfr;
            }, module_key);
            recursive_level--;
            unless (load_result.valid()) {
                sol::error err = load_result;
                le("failed load module {}, error: {}", module, err.what());
                throw std::runtime_error("exception!");
            }
            try {
                loaded_modules[module_key] = load_result;
                if (recursive_level == 0) {
                    required_modules[module_key] = true;
                }
                ld(1, "load module {} success", module_key);
                return loaded_modules[module_key];
            }
            catch (...) {
                le("failed load lua module '{}', wrong return, expect lua module", module);
                throw std::runtime_error("exception!");
            }
        }
    );
    auto load_result = lua.script(logic_code.get(), [](lua_State*, sol::protected_function_result pfr){
        return pfr;
    }, script_name);
    unless (load_result.valid()) {
        sol::error err = load_result;
        le("failed load logic lua script, error: {}", err.what());
        return error;
    }

    lua["debug"]["sethook"](sol::nil, "l");

    std::string module;
    std::string version;

    sol::table result;

    try
    {
        result = load_result;
        module = result["module"];
        version = result["version"];
    }
    catch (...) {
        le("failed load lua script '{}', wrong return, expect lua module", file_name);
        return error;
    }

    if (module == "" ) {
        le("wrong lua script, module do not define module name");
        return error;
    }

    if (version == "" ) {
        le("wrong lua script, module do not define version");
        return error;
    }

    // check version
    Regexp re((char *)*version);
    unless (re(R"(/^[0-9]+\.[0-9]+\.[0-9]+$/)")) {
        le("wrong version '{}', expect form x.y.z", version);
        return error;
    }

    auto module_key = fmt::format("{}:{}", module, version);
    if (loaded_modules.count(module_key)) {
        le("module {}-{} could not be loaded, module dependency cycle detected:", module, version);
        for(auto &[key, _] : required_modules) {
            auto &module_lua = loaded_modules[key];
            std::string required_module = module_lua["module"];
            std::string required_version = module_lua["version"];
            lg(1, "     {}-{}", required_module, required_version);
            //auto graph = Module.logic->GetDependencyGraph(required_module, required_version);
            //dump_graph(graph);
        }
        return error;
    }

    // require for testing, do not load it again
    loaded_modules[module] = load_result;

    // hack to prevent load test dependecies into required module list
    recursive_level = 1;

    hash_map<std::string, std::shared_ptr<int>> alias_coverage;
    hash_map<int, std::shared_ptr<int>> defineline_coverage;
    hash_map<int, int> defineline_lastdefineline;

    lg(1, "lua load module {}-{} success, run tests from {}", module, version, test_name);
    auto function_result = lua.script(R"(
        function filter_functions(M)
            local functions = {}
            local references = {}
            for k, v in pairs(M) do
                if type(k) == "string" and type(v) == "function" then
                    if references[v] ~= nil then
                        --print(k.." is alias to "..references[v])
                        table.insert(functions[references[v]], k)
                    else
                        references[v] = k
                        --print(k.." is a function")
                        functions[k] = {}
                        table.insert(functions[k], k)
                    end
                end
            end
            return functions
        end
    )");
    unless (function_result.valid()) {
        sol::error err = version_result;
        le("failed get function filter, error: {}", err.what());
        return error;
    }

    sol::table function_aliases = lua["filter_functions"](loaded_modules[module]);

    for (auto &[k, v] : function_aliases) {
        std::string key = k.as<std::string>();
        sol::type t = v.get_type();
        ld(1, "function {}", key);
        if (t == sol::type::table) {
            auto aliases = v.as<sol::table>();
            ld(1, "    function aliases {}", aliases.size());
            auto count = std::make_shared<int>(0);
            for(auto &[a, v] : aliases) {
                std::string index = a.as<std::string>();
                std::string alias = v.as<std::string>();
                ld(1, "        [{}] {}", index, alias);
                alias_coverage[alias] = count;
            }
        }
        else {
            le("BUG: functions aliasese is not a table");
            return error;
        }
    }

    ld(2, "module functions: ");
    {
        sol::table &module_table = loaded_modules[module];
        for (auto &[k, v] : module_table) {
            std::string key = k.as<std::string>();
            sol::type t = v.get_type();
            if (t == sol::type::function) {
                ld(2, "    function {}", key);
                sol::table i = debug_info(v);

                unless (alias_coverage.count(key)) {
                    lf("BUG: no coverage alias found for '{}'", key);
                    alias_coverage[key] = std::make_shared<int>(0);
                }
                if (i != sol::nil) {
                    int linedefined = i["linedefined"];
                    int lastlinedefined = i["lastlinedefined"];
                    defineline_coverage[linedefined] = alias_coverage[key];
                    defineline_lastdefineline[linedefined] = lastlinedefined;
                    dump_table(i, 3, "      ");
                }
            }
        }
    }

    lua["_coverage_hook"] =
        [&debug_info, &script_name, &alias_coverage, &source_coverage, &defineline_coverage, &set_coverage, &dump_table]
        (sol::object event, int line, int level) {
            level = level > 0 ? level : 2;
            sol::table info = debug_info(level, "nfS");
            if (info != sol::nil) {
                std::string source = info["source"];
                std::string name   = info["name"];
                int linedefined    = info["linedefined"];
                int lastline       = info["lastlinedefined"];
                if (false and line > lastline) {
                    lw("strange source line {} is bigger that last defined line {}", line, lastline);
                }
                int nups = info["nups"];
                if (source == script_name or (false and line > lastline and line > 0)) {
                    set_coverage(line, info);
                    if (defineline_coverage.count(linedefined)) {
                        *defineline_coverage[linedefined] = 1;
                    }
                    if (alias_coverage.count(name)) {
                        *alias_coverage[name] = 1;
                    }
                    else {
                        alias_coverage[name] = std::make_shared<int>(1);
                    }
                    dump_table(info, 4, "  ");
                }
                if (false) {
                    for(int i = nups + level; i >= 0; i--) {
                        lg(1, "level {}", i);
                        info = debug_info(i, "unfS");
                        if (info != sol::nil) {
                            dump_table(info, 4, "  ");
                        }
                    }
                }
            }
        };

    //lua["debug"]["sethook"](lua["_coverage_hook"], "l");
    // debug hooks does not work for coroutine, trouble in sol?
    // found the way to test logic - redefine coroutine.yeld = function(...) return ... end
    // and code bellow will not break coroutine usage inside testing function!!!
    auto set_hook_result = lua.script(R"(
        -- keep coroutine yield function for created coroutines
        local rawcoroutineyield = coroutine.yield
        local rawcoroutinecreate = coroutine.create
        local rawcoroutineresume = coroutine.resume

        coroutine.create = function(...)
            local co = rawcoroutinecreate(...)
            return co
        end

        coroutine.wrap = function(...)
            local co = rawcoroutinecreate(...)
            return function(...)
                return coroutine.resume(co, ...)
            end
        end

        coroutine.resume = function(...)
            local oldcoroutineyield = coroutine.yield
            coroutine.yield = function(...)
                return rawcoroutineyield(...)
            end
            local returns = table.pack(rawcoroutineresume(...))
            coroutine.yield = oldcoroutineyield
            return table.unpack(returns)
        end

        debug.sethook(_coverage_hook, "l")
        --[[local hook_per_thread

        -- Determines whether debug hooks are separate for each thread.
        local function has_hook_per_thread()
            if hook_per_thread == nil then
                local old_hook, old_mask, old_count = debug.gethook()
                local noop = function() end
                debug.sethook(noop, "l")
                local thread_hook = coroutine.wrap(function() return debug.gethook() end)()
                hook_per_thread = thread_hook ~= noop
                debug.sethook(old_hook, old_mask, old_count)
            end

            return hook_per_thread
        end

        if has_hook_per_thread() then
            -- debug must be set for each coroutine separately
            -- hence wrap coroutine function to set the hook there
            -- as well
            local rawcoroutinecreate = coroutine.create
            coroutine.create = function(...)
                local co = rawcoroutinecreate(...)
                debug.sethook(co, _coverage_hook, "l")
                return co
            end

            -- Version of assert which handles non-string errors properly.
            local function safeassert(ok, ...)
                if ok then
                    return ...
                else
                    error(..., 0)
                end
            end

            coroutine.wrap = function(...)
                local co = rawcoroutinecreate(...)
                debug.sethook(co, _coverage_hook, "l")
                return function(...)
                    return safeassert(coroutine.resume(co, ...))
                end
            end
        end
        --]]
    )");

    unless (set_hook_result.valid()) {
        sol::error err = set_hook_result;
        le("failed set debug hook, error: {}", err.what());
        return error;
    }

    // run tests
    ld(1, "load and run tests");
    auto test_result = lua.script(test_code.get(), [](lua_State*, sol::protected_function_result pfr){
        return pfr;
    }, test_module);
    unless (test_result.valid()) {
        sol::error err = test_result;
        le("module {}-{} failed run test, error: {}", module, version, err.what());
        return error;
    }

    lua["debug"]["sethook"](sol::nil, "l");

    int not_success_count;
    double test_coverage;
    sol::tie(not_success_count, test_coverage) = test_result;
    if (test_coverage == 0) test_coverage = 100.0;

    double coverage = 0;
    double total = 0;
    ld(2, "coverage: ");
    for(auto &[k, v] : alias_coverage) {
        ld(2, "    {}: {}", k, *v);
        total += 1.0;
        if (*v) coverage += 1.0;
    }
    ld(1, "coverage {} total {}", coverage, total);

    lg(2, "{} coverage:", file_name);

    size_t line = 0;
    size_t cover_lines = 0;
    size_t till_line = 0;
    bool comment = false;
    Regexp scan_re;
    auto aling_length = fmt::format("{}", source_coverage.size()).length();
    for (auto &line_coverage : source_coverage) {
        ++line;
        auto &[line_cover, line_count, code] = line_coverage;
        bool covered = true;
        int cover_flag = 0;
        scan_re.string = (char *)*code;
        scan_re.pos = 0;
        if (defineline_coverage.count(line)) {
            unless (*defineline_coverage[line]) {
                covered = false;
                till_line = defineline_lastdefineline[line];
            }
        }
        if (line_cover == 0 and line_count == 0) {
            while (true) {
                if (scan_re(R"(/^$/o)")) break;
                if (scan_re(R"(/^[\s\t]*\}/o)")) break;
                if (scan_re(R"(/^[\s\t]*end\b/o)")) break;
                //if (scan_re(R"(/^[\s\t]*return\b/o)")) break;
                if (scan_re(R"(/^[\s\t]*if[\s\t]*$/o)")) break;
                if (scan_re(R"(/^[\s\t]*else\b/o)")) break;
                if (scan_re(R"(/^[\s\t]*repeat\b/o)")) break;
                if (scan_re(R"(/^[\s\t]*break\b/o)")) break;
                if (scan_re(R"(/^[\s\t]*until\b/o)")) break;
                if (scan_re(R"(/^[\s\t]*goto\b/o)")) break;
                if (scan_re(R"(/^[\s\t]*\:\:[^\:]+\:\:/o)")) break;
                if (scan_re(R"(/\[\[/o)")) {
                    comment = true;
                    break;
                }
                if (comment and scan_re(R"(/\]\]/o)")) {
                    comment = false;
                    cover_flag |= 16;
                    break;
                }
                if (scan_re(R"(/^[\s\t]*\-\-/o)")) {
                    cover_flag |= 16;
                    break;
                }
                unless (comment) {
                    covered = false;
                }
                break;
            }
        }
        if (till_line) {
            if (line > till_line) till_line = 0;
            if (line <= till_line and covered) {
                covered = false;
            }
        }

        if (covered) {
            line_cover |= 8;
            ++cover_lines;
        }
        if (comment) line_cover |= 16;
        line_cover |= cover_flag;

        ld(2, "[{}] {} {}: {}", line, line_cover, line_count, code);

        std::string align = utils::pad(" ") * (aling_length - fmt::format("{}", line).length() + 1);

        if (line_cover & 16) {
            //! output comments
            lg(2, "[{}]{}│ {black}{}{reset}", line, align, code);
        }
        else if (line_cover & 8) {
            //! output covered code
            lg(2, "[{}]{}│ {}{reset}", line, align, code);
        }
        else {
            lg(2, "[{}]{}│ {red}{}{reset}", line, align, code);
        }
    }

    double lines_coverage = 100.0 * cover_lines / source_coverage.size();

    if (total == 0) {
        // module do not export any functions
        coverage = 1;
    }
    else {
        coverage /=  total;
    }

    coverage *= 100.0;

    ld(1, "module {}-{} failed tests {}, coverage functions {:.5}% source {:.5}%",
        module, version, not_success_count, coverage, lines_coverage
    );

    if (not_success_count) {
        le("module {}-{} not all tests passed, failed {}, could not be loaded",
            module, version, not_success_count
        );
        return error;
    }
    if (coverage < test_coverage) {
        le("module {}-{} tests coverage {:.5}%, could not be loaded", module, version, coverage);
        return error;
    }

    lg(1, "module {}-{} all test passed, coverage functions {:.5}% source {:.5}%",
        module, version, coverage, lines_coverage
    );

    ld(1, "module dependency:");

    script_dependencies_t dependency;
    for (auto &[require, item] : required_modules) {
        auto tockens = utils::split(require, ':');
        auto require_module = tockens[0];
        auto require_version = tockens[1];
        ld(1, "   module {}-{}", require_module, require_version);
        dependency.push_back({require_module, require_version});
    }

    lr("loaded module '{}' version '{}'", module, version);

    return ok;
}
