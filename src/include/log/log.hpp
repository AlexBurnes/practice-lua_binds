#pragma once

#include <unistd.h>
#include <stdio.h>

#include <algorithm>
#include <array>
#include <map>
#include <stdexcept>
#include <string_view>
#include <ctime>
#include <chrono>
#include <thread>
#include <memory>
#include <utility>

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/format.h>

#include <tscns.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>

/*!@namespace cx
 * Define constexpr flat Maps, throwing and not throwing
 */

namespace cx {

    /*!
     *  constexpr flat map
     */

    template <typename Key, typename Value, std::size_t Size>
    struct Map {
        std::array<std::pair<Key, Value>, Size> data;

        [[nodiscard]] constexpr Value at(const Key& key, const std::string_view what) const {
            const auto itr = std::find_if(begin(data), end(data), [&key](const auto& v) { return v.first == key; });
            if (itr != end(data)) {
                return itr->second;
            } else {
                throw std::range_error("Not Found");
            }
        }
    };

    template <typename Key, typename Value, std::size_t Size>
    struct NotThrowingMap {
        std::array<std::pair<Key, Value>, Size> data;

        [[nodiscard]] constexpr Value at(const Key& key, const std::string_view what) const {
            const auto itr = std::find_if(begin(data), end(data), [&key](const auto& v) { return v.first == key; });
            if (itr != end(data)) {
                return itr->second;
            } else {
                return data[Size-1].second;
            }
        }
    };

    /*!
     *  constexpr std::string using callable
     */

    struct oversized_array {
        std::array<char, 1024 * 1024> data{};
        std::size_t size;
    };

    constexpr auto to_oversized_array(const std::string &input) {
        oversized_array result;
        std::copy(input.begin(), input.end(), result.data.begin());
        result.size = input.size();
        return result;
    }

    template<typename Callable>
    consteval auto to_right_sized_array(Callable callable) {
        constexpr auto oversized = to_oversized_array(callable());
        std::array<char, oversized.size> result;
        std::copy(
            oversized.data.begin(),
            std::next(oversized.data.begin(), oversized.size),
            result.begin()
        );
        return result;
    }

    template<auto Data>
    consteval const auto &make_static() {
        return Data;
    }

    consteval auto to_string_view(auto callable) -> std::string_view {
        constexpr auto &static_data = make_static<to_right_sized_array(callable)>();
        return std::string_view{static_data.begin(), static_data.size()};
    }

    /*!
     * constexpr string_to_array<string_length(s)>(s)
     */
    constexpr auto string_length(const std::string &string) {
        return string.size();
    }

    template<std::size_t Len>
    constexpr auto string_to_array(const std::string &str) {
        std::array<char, Len> result{};
        std::copy(str.begin(), str.end(), result.begin());
        return result;
    }

} // namespace cx

/*!@namespace lg
 * Defines logger structures and methods for format parser
 */
namespace lg {

    enum class Levels: int {
        ANY = 0,
        FATAL,
        ERROR,
        WARN,
        INFO,
        DEBUG,
        TRACE
    };

    typedef FILE log_file_t;

    struct Context {
        //!@warn this is a reference to stdout or stderr only!
        log_file_t* output_m;
        bool support_colors_m;
        const std::string_view file_m;
        const std::string_view func_m;
        const size_t line_m;
        const pthread_t tid;
        Context(log_file_t *output, bool support_colors, std::string_view file, std::string_view func, size_t line)
        : output_m{output}, support_colors_m{support_colors}, file_m{file}, func_m{func}, line_m{line},
          tid{pthread_self()}
        {}
    };

    struct Logger {

        static inline Levels level = Levels::INFO;
        static inline uint8_t log_level = 1;
        static inline uint8_t debug_level = 0;
        static inline bool quiet = false;
        static inline bool simple = false;
        static inline bool stdout_support_colors = isatty(fileno(stdout));
        static inline bool stderr_support_colors = isatty(fileno(stderr));
        static inline pid_t pid = getpid();

        std::string thread_name;
        TSCNS tscns;

        bool log_file;
        log_file_t* output;
        std::string file_name_m;
        std::string file_name_mask_m;
        time_t rawseconds_m{0};
        int64_t rawtime_m{0};

        std::thread flush_thread;
        volatile bool flush_running = false;
        int64_t flush_delay;
        int64_t flush_next_time = (std::numeric_limits<int64_t>::max)();

        bool io_error;

        Logger()
        :
            log_file{false},
            output{nullptr},
            flush_delay{100000000},
            io_error{false}
         {
            char name[100];
            tscns.init();
            pthread_getname_np(pthread_self(), name, 100);
            thread_name.assign(name, strlen(name));
            start_flushing(flush_delay);
        }
        ~Logger() {
            Close();
            stop_flushing();
        };

        Logger(Logger&& other) = delete;

        static Logger& logger() {
            static Logger logger_;
            return logger_;
        }

        void Open(std::string&& file_name_mask) {
            rawtime_m = tscns.rdns() / 1e9;
            file_name_mask_m = file_name_mask;
            open_(format_name_(rawtime_m));
        }

        void Close() {
            close_();
        }

        void Fatal() {
            Close();
            quick_exit(EXIT_FAILURE);
        }

        void reopen(const time_t rawseconds) {
            auto const file_name = format_name_(rawseconds);
            if (file_name_m.compare(file_name) == 0) return;
            open_(file_name);
        }

        void update_time() {
            rawtime_m = tscns.rdns();
        }

        void flush() {
            fflush(stdout);
            fflush(stderr);
            if (log_file) {
                fflush(output);
            }
        }

        void Simple(bool t_simple) {simple = t_simple;}
        void Level(Levels t_level) {level = t_level;}
        void Level(int t_level) {log_level = t_level;}
        void Debug(int t_debug) {debug_level = t_debug;}
        void Quiet(bool t_quiet) {quiet = t_quiet;}

        private:
            std::string format_name_(const time_t rawtime) {
                auto const tm = std::localtime(&rawtime);
                char file_name[1024];
                auto len = std::strftime(file_name, sizeof(file_name), file_name_mask_m.c_str(), tm);
                return {file_name, len};
            }

            void close_() {
                if (log_file) {
                    log_file = false;
                    auto file = std::exchange(output, nullptr);
                    if (file) {
                        fflush(file);
                        fclose(file);
                    }
                }
            }

            void open_(const std::string &file_name) {
                auto file = fopen(file_name.c_str(), "a");
                if (file == nullptr) {
                    fmt::print(stderr, "ERROR: failed open log file '{}'\n", file_name);
                    return;
                }
                file = std::exchange(output, file);
                log_file = true;
                io_error = false;
                if (file != nullptr) {
                    fflush(file);
                    fclose(file);
                }
                file_name_m = file_name;


            }

            void start_flushing(int64_t flush_interval) {
                stop_flushing();
                flush_running = true;
                flush_thread = std::thread(
                    [flush_interval, this]() {
                        while (flush_running) {

                            tscns.calibrate();
                            int64_t before = tscns.rdns();
                            flush();

                            auto const raw_seconds = before / 1e9;
                            if (rawseconds_m != raw_seconds) {
                                rawseconds_m = raw_seconds;
                                if (log_file) {
                                    reopen(rawseconds_m);
                                }
                            }

                            int64_t delay = tscns.rdns() - before;
                            if (delay < flush_interval) {
                                std::this_thread::sleep_for(std::chrono::nanoseconds(flush_interval - delay));
                            }

                        }
                        flush();
                    }
                );
            }

            void stop_flushing() {
                if (!flush_running) return;
                flush_running = false;
                if (flush_thread.joinable()) {
                    flush_thread.join();
                }
            }

    };

    enum class Tags : uint8_t {
        TID = 1,
        PID,
        Date,
        Message,
        File,
        Func,
        Line,
        FG,
        BG,
        Style,
        StyleOnly,
        StyleAndFG,
        FGAndBG
    };

    static constexpr std::array<std::pair<std::string_view, Tags>, 10> option_values{
        {
            {"tid",     Tags::TID},
            {"pid",     Tags::PID},
            {"date",    Tags::Date},
            {"message", Tags::Message},
            {"file",    Tags::File},
            {"func",    Tags::Func},
            {"line",    Tags::Line},
            {"fg",      Tags::FG},
            {"bg",      Tags::BG},
            {"",        Tags::Style}
        }
    };
    static constexpr auto options =
        cx::NotThrowingMap<std::string_view, Tags, option_values.size()>{{option_values}};

    static constexpr std::array<std::pair<std::string_view, std::string_view>, 17> fg_color_values{
        {
            {"", ""},
            {"black",          "\x1b[30m"},
            {"red",            "\x1b[31m"},
            {"green",          "\x1b[32m"},
            {"yellow",         "\x1b[33m"},
            {"blue",           "\x1b[34m"},
            {"magenta",        "\x1b[35m"},
            {"cyan",           "\x1b[36m"},
            {"white",          "\x1b[37m"},
            {"bright_black",   "\x1b[90;1m"},
            {"bright_red",     "\x1b[91;1m"},
            {"bright_green",   "\x1b[92;1m"},
            {"bright_yellow",  "\x1b[93;1m"},
            {"bright_blue",    "\x1b[94;1m"},
            {"bright_magenta", "\x1b[95;1m"},
            {"bright_cyan",    "\x1b[96;1m"},
            {"bright_white",   "\x1b[97;1m"}
        }
    };
    static constexpr auto fg_colors =
        cx::Map<std::string_view, std::string_view, fg_color_values.size()>{{fg_color_values}};

    static constexpr std::array<std::pair<std::string_view, std::string_view>, 16> bg_color_values{
        {
            {"black",          "\x1b[40m"},
            {"red",            "\x1b[41m"},
            {"green",          "\x1b[42m"},
            {"yellow",         "\x1b[43m"},
            {"blue",           "\x1b[44m"},
            {"magenta",        "\x1b[45m"},
            {"cyan",           "\x1b[46m"},
            {"white",          "\x1b[47m"},
            {"bright_black",   "\x1b[100;1m"},
            {"bright_red",     "\x1b[101;1m"},
            {"bright_green",   "\x1b[102;1m"},
            {"bright_yellow",  "\x1b[103;1m"},
            {"bright_blue",    "\x1b[104;1m"},
            {"bright_magenta", "\x1b[105;1m"},
            {"bright_cyan",    "\x1b[106;1m"},
            {"bright_white",   "\x1b[107;1m"}
        }
    };
    static constexpr auto bg_colors =
        cx::Map<std::string_view, std::string_view, bg_color_values.size()>{{bg_color_values}};

    static constexpr std::array<std::pair<std::string_view, std::string_view>, 10> style_values{
        {
            {"reset",     "\x1b[0m"},
            {"bold",      "\x1b[01m"},
            {"faint",     "\x1b[02m"},
            {"italic",    "\x1b[03m"},
            {"underline", "\x1b[04m"},
            {"blink",     "\x1b[05m"},
            {"reverse",   "\x1b[07m"},
            {"conceal",   "\x1b[08m"},
            {"strike",    "\x1b[09m"},
            {"", ""}
        }
    };
    static constexpr auto styles =
        cx::NotThrowingMap<std::string_view, std::string_view, style_values.size()>{{style_values}};

    template<typename Char>
    struct FormatSpec {
        typedef Char type;
        lg::Tags option{lg::Tags::Style};
        std::string_view style{""};
        std::string_view fg{""};
        std::string_view bg{""};
        std::string_view date_format{""};
    };

    template <typename Char>
    constexpr auto parse_name(const Char* begin, const Char* end) -> const Char * {
        auto it = begin;
        while (it != end) {
            if (*it == '}') return it;
            if (*it == ':') return it;
            ++it;
        }
        return it;
    }

    template <typename Char>
    constexpr auto parse_fg_color(std::string_view name, const Char* begin, const Char* end, lg::FormatSpec<Char>& spec)
    -> const Char * {
        spec.fg = lg::fg_colors.at(name, "fg_color");
        return begin;
    }

    template <typename Char>
    constexpr auto parse_bg_color(std::string_view name, const Char* begin, const Char* end, lg::FormatSpec<Char>& spec)
    -> const Char * {
        spec.bg = lg::bg_colors.at(name, "bg_color");
        return begin;
    }

    template <typename Char>
    constexpr auto parse_style(std::string_view name, const Char* begin, const Char* end, lg::FormatSpec<Char>& spec)
    -> const Char * {
        auto it = begin;
        spec.style = lg::styles.at(name, "style");
        if (spec.style == "") {
            spec.option = lg::Tags::FG;
            it = parse_fg_color(name, it, end, spec);
            if (it == end or *it == '}') return it;
            spec.option = lg::Tags::FGAndBG;
            auto next = parse_name(++it, end);
            return parse_bg_color({it, next}, next, end, spec);
        }
        if (it == end or *it == '}') {
            spec.option = lg::Tags::StyleOnly;
            return it;
        }
        auto next = parse_name(++it, end);
        it = parse_fg_color({it, next}, next, end, spec);
        if (it == end or *it == '}') {
            spec.option = lg::Tags::StyleAndFG;
            return it;
        }
        next = parse_name(++it, end);
        return parse_bg_color({it, next}, next, end, spec);
    }

    template <typename Char>
    constexpr auto parse_date(const Char* begin, const Char* end, lg::FormatSpec<Char>& spec) -> const Char * {
        if (begin == end or *begin == '}') return begin;
        auto it = begin;
        ++it;
        auto next = fmt::detail::parse_chrono_format(it, end, fmt::detail::tm_format_checker());
        spec.date_format = std::string_view{it, next};
        return next;
    }

    template <typename FormatContext, typename Char>
    auto format_date(FormatContext& ctx, const FormatSpec<Char>& spec) -> decltype(ctx.out()) {
        auto buf = fmt::basic_memory_buffer<Char>();
        auto out = std::back_inserter(buf);

        auto loc_ref = ctx.locale();

        auto const rawtime = lg::Logger::logger().rawtime_m;
        const time_t raw_seconds = rawtime / 1e9;

        auto const raw_nanoseconds = rawtime % (long int)1e9;
        auto const tm = fmt::localtime(raw_seconds);
        const auto duration = std::chrono::nanoseconds(raw_nanoseconds);

        fmt::detail::get_locale loc(static_cast<bool>(loc_ref), loc_ref);

        auto w = fmt::detail::tm_writer<decltype(out), Char, std::chrono::nanoseconds>(
            loc, out, tm, &duration
        );
        fmt::detail::parse_chrono_format(spec.date_format.begin(), spec.date_format.end(), w);
        return fmt::detail::write(
            ctx.out(), fmt::basic_string_view<Char>(buf.data(), buf.size())
        );
    }

    template <typename Char>
    constexpr auto parse_option(const Char* begin, const Char* end, lg::FormatSpec<Char>& spec) -> const Char * {
        if (begin == end or *begin == '}') return begin;
        auto it = parse_name(begin, end);
        auto next = it;
        spec.option = lg::options.at({begin, it}, "options");
        switch (spec.option) {
            case lg::Tags::Style:
                return parse_style({begin, it}, it, end, spec);
            case lg::Tags::FG:
                next = parse_name(++it, end);
                return parse_fg_color({it, next}, next, end, spec);
            case lg::Tags::BG:
                next = parse_name(++it, end);
                return parse_bg_color({it, next}, next, end, spec);
            case lg::Tags::Date:
                return parse_date(next, end, spec);
            case lg::Tags::TID:
            case lg::Tags::PID:
            case lg::Tags::File:
            case lg::Tags::Func:
            case lg::Tags::Line:
            case lg::Tags::Message:
            case lg::Tags::StyleAndFG:
            case lg::Tags::StyleOnly:
            case lg::Tags::FGAndBG:
                break;
        }
        return it;
    }

    constexpr inline void index_to_buffer(std::string &buf, size_t n) {
        if (n / 10) index_to_buffer(buf, n / 10);
        buf.push_back('0' + n % 10);
    }

    constexpr inline size_t index_length(size_t n) {
        size_t size = 1;
        while(n /= 10) ++size;
        return size;
    }

    constexpr inline size_t preformat_length(std::string_view format) {
        auto const begin = format.begin();
        auto const end = format.end();
        auto it = begin;
        auto index = 1;
        size_t size = 0;

        auto inline_argument = [&]() -> void {
            ++size;
            ++it;
            if (*it >= '0' and *it <= '9') {
                while (*it >= '0' and *it <= '9') {
                    ++size;
                    ++it;
                }
            }
            else {
                size += index_length(index);
            }
            if (*it == '}') {
                ++size;
                ++it;
                return;
            }
            fmt::detail::throw_format_error("wrong format");
        };

        while (it != end) {
            if (*it == '{') {
                ++size;
                ++it;
                if (it == end) continue;
                if (*it == '{') {
                    ++size;
                    ++it;
                    continue;
                }
                if (*it >= '0' and *it <= '9') {
                    while (it != end and *it != '}') {
                        if (*it == '{') {
                            inline_argument();
                            continue;
                        }
                        ++size;
                        ++it;
                    }
                    continue;
                }
                if (*it == ':') {
                    size += index_length(index);
                    ++index;
                    while (it != end and *it != '}') {
                        if (*it == '{') {
                            inline_argument();
                            continue;
                        }
                        ++size;
                        ++it;
                    }
                    continue;
                }
                if (*it == '}') {
                    size += index_length(index);
                    ++index;
                    continue;
                }
                size += 2; //'0' + ':'
                while (it != end and *it != '}') {
                    if (*it == '{') {
                        inline_argument();
                        continue;
                    }
                    ++size;
                    ++it;
                }
            }
            if (it == end) continue;
            ++size;
            ++it;
        }
        return size;
    }

    constexpr inline std::string preformat(std::string_view format) {
        std::string buf{};
        auto const begin = format.begin();
        auto const end = format.end();
        auto it = begin;
        auto index = 1;

        auto inline_argument = [&]() -> void {
            buf.push_back(*it);
            ++it;
            if (*it >= '0' and *it <= '9') {
                while (*it >= '0' and *it <= '9') {
                    buf.push_back(*it);
                    ++it;
                }
            }
            else {
                index_to_buffer(buf, index);
            }
            if (*it == '}') {
                buf.push_back(*it);
                ++it;
                return;
            }
            fmt::detail::throw_format_error("wrong format");
        };

        while (it != end) {
            if (*it == '{') {
                buf.push_back(*it);
                ++it;
                if (it == end) continue;
                if (*it == '{') {
                    buf.push_back(*it);
                    ++it;
                    continue;
                }
                if (*it >= '0' and *it <= '9') {
                    while (it != end and *it != '}') {
                        if (*it == '{') {
                            inline_argument();
                            continue;
                        }
                        buf.push_back(*it);
                        ++it;
                    }
                    continue;
                }
                if (*it == ':') {
                    index_to_buffer(buf, index);
                    ++index;
                    while (it != end and *it != '}') {
                        if (*it == '{') {
                            inline_argument();
                            continue;
                        }
                        buf.push_back(*it);
                        ++it;
                    }
                    continue;
                }
                if (*it == '}') {
                    index_to_buffer(buf, index);
                    ++index;
                    continue;
                }
                buf.push_back('0');
                buf.push_back(':');
                while (it != end and *it != '}') {
                    if (*it == '{') {
                        inline_argument();
                        continue;
                    }
                    buf.push_back(*it);
                    ++it;
                }

            }
            if (it == end) continue;
            buf.push_back(*it);
            ++it;
        }
        return buf;
    }

    template <typename LogContext, typename ...Args>
        constexpr inline void output(
        fmt::format_string<LogContext, Args...> format,
        LogContext&&                ctx,
        Args&&...                   args
    ) {

        lg::Logger::logger().update_time();

        //FIXME if output does not support colors then make string once!
        if (!lg::Logger::logger().quiet) {
            fmt::print(ctx.output_m, format, std::forward<LogContext>(ctx), std::forward<Args>(args)...);
        }
        if (Logger::logger().log_file) {
            ctx.support_colors_m = false;
            try {
                fmt::print(
                    Logger::logger().output, format, std::forward<LogContext>(ctx),
                    std::forward<Args>(args)...
                );

            }
            catch (fmt::format_error& e) {
                fmt::print(stderr, "FATAL: logger format error '{}'\n", e.what());
                Logger::logger().Close();
                quick_exit(EXIT_FAILURE);
            }
            catch (std::system_error &e) {
                if (Logger::logger().io_error == false) {
                    Logger::logger().io_error = true;
                    fmt::print(stderr, "ERROR: logger system error '{}'\n", e.what());
                }
            }
            catch (...) {
                //ignore other
                fmt::print(stderr, "ERROR: logger error unknown\n");
            }
        }
#ifdef LOG_FLUSH
        Logger::logger().flush();
#endif

    }

    template <typename LogContext, typename ...Args>
    constexpr inline auto format_to(
        fmt::format_string<LogContext, Args...> format,
        LogContext&&                ctx,
        Args&&...                   args
    )
    -> std::string {
        return fmt::format(format, std::forward<LogContext>(ctx), std::forward<Args>(args)...);
    }

    inline void open(std::string &&file_name) {lg::Logger::logger().Open(std::forward<std::string>(file_name));}
    inline void close() {lg::Logger::logger().Close();}
    inline void simple(bool simple) {lg::Logger::logger().Simple(simple);}
    inline void level(int level) {lg::Logger::logger().Level(level);}
    inline void debug(int debug) {lg::Logger::logger().Debug(debug);}
    inline void quiet(bool quiet) {lg::Logger::logger().Quiet(quiet);}
    inline void level(Levels level) {lg::Logger::logger().Level(level);}

    inline void log_init(
        Levels logger_level = Levels::ANY, int log_level = 1, int debug_level = 0, bool log_simple = false
    ) {
        level(logger_level);
        level(log_level);
        debug(debug_level);
        simple(log_simple);
    }

    inline void log_configure(
        std::string&& log_file, Levels logger_level = Levels::ANY, int log_level = 1, int debug_level = 0,
        bool log_quiet = false
    ) {
        log_init(logger_level, log_level, debug_level, lg::Logger::logger().simple);
        if (log_file != "") {
            open(std::forward<std::string>(log_file));
        }
        quiet(log_quiet);
    }


    template <typename Container, typename OutputIt>
    auto inline copy(const Container& c, OutputIt out) -> OutputIt {
        return std::copy(c.begin(), c.end(), out);
    }

    template <typename OutputIt, typename Number>
    auto inline number_to_out(OutputIt &out, Number n) -> OutputIt {
    if (n / 10) number_to_out(out, n / 10);
        *out = '0' + n % 10;
        return out;
    }

} //namespace lg


/*!struct fmt::formatter<lg::Context, char>
 * Logger format parser and formatter
 */
template <typename Char>
struct fmt::formatter<lg::Context, Char> {
    lg::FormatSpec<Char> format_spec;
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
        return lg::parse_option(ctx.begin(), ctx.end(), format_spec);
    }

    template <typename FormatContext>
    auto format(lg::Context const& c, FormatContext& ctx) const -> decltype(ctx.out()) {
        auto out = ctx.out();
        switch (format_spec.option) {
            case lg::Tags::StyleOnly:
                return c.support_colors_m ? lg::copy(format_spec.style, out) : out;
            case lg::Tags::StyleAndFG:
                return c.support_colors_m ? lg::copy(format_spec.fg, lg::copy(format_spec.style, out)) : out;
            case lg::Tags::Style:
                return c.support_colors_m
                    ? lg::copy(format_spec.style, lg::copy(format_spec.fg, lg::copy(format_spec.bg, out))) : out;
            case lg::Tags::FGAndBG:
                return c.support_colors_m ? lg::copy(format_spec.fg, lg::copy(format_spec.bg, out)) : out;
            case lg::Tags::FG:
                return c.support_colors_m ? lg::copy(format_spec.fg, out) : out;
            case lg::Tags::BG:
                return c.support_colors_m ? lg::copy(format_spec.bg, out) : out;
            case lg::Tags::File:
                return lg::copy(c.file_m, out);
            case lg::Tags::Func:
                return lg::copy(c.func_m, out);;
            case lg::Tags::Line:
                return lg::number_to_out(out, c.line_m);
            case lg::Tags::Date:
                return lg::format_date(ctx, format_spec);
            case lg::Tags::PID:
                return lg::number_to_out(out, lg::Logger::pid);
            case lg::Tags::TID:
                return lg::number_to_out(out, c.tid);
                //return lg::copy(lg::Logger::logger().thread_name, out);
            default:
                return out;
        }
    }
};

#ifdef __PATH_LEN__
#define __FILENAME__ (__FILE__ + __PATH_LEN__)
#else
#define __FILENAME__ __FILE__
#endif

#define LOG_FORMAT_LG "{date:%Y.%m.%d %H:%M:%S} [{pid}] [{tid}]: "
#define LOG_FORMAT_LD "{yellow}{date:%Y.%m.%d %H:%M:%S} [{pid}] [{tid}]: #DEBUG: {file}:{line}: "
#define LOG_FORMAT_LW "{magenta}{date:%Y.%m.%d %H:%M:%S} [{pid}] [{tid}]: WARNING: "
#define LOG_FORMAT_LE "{red}{date:%Y.%m.%d %H:%M:%S} [{pid}] [{tid}]: ERROR: "
#define LOG_FORMAT_LF "{bold:red}{date:%Y.%m.%d %H:%M:%S} [{pid}] [{tid}]: FATAL: "
#define LOG_FORMAT_LR "{bold:blue}{date:%Y.%m.%d %H:%M:%S} [{pid}] [{tid}]: "

#define LOG_SIMPLE_LG ""
#define LOG_SIMPLE_LD "{yellow}#DEBUG: {file}:{line}: "
#define LOG_SIMPLE_LW "{magenta}WARNING: "
#define LOG_SIMPLE_LE "{red}ERROR: "
#define LOG_SIMPLE_LF "{bold:red}FATAL: "
#define LOG_SIMPLE_LR "{bold:blue}"

#define LOG_FORMAT_ANY   "{date:%Y-%m-%d %H:%M:%S} ANY     "
#define LOG_FORMAT_FATAL "{date:%Y-%m-%d %H:%M:%S} FATAL   "
#define LOG_FORMAT_ERROR "{date:%Y-%m-%d %H:%M:%S} ERROR   "
#define LOG_FORMAT_WARN  "{date:%Y-%m-%d %H:%M:%S} WARNING "
#define LOG_FORMAT_INFO  "{date:%Y-%m-%d %H:%M:%S} INFO    "
#define LOG_FORMAT_DEBUG "{date:%Y-%m-%d %H:%M:%S} DEBUG   {file}:{line}: "
#define LOG_FORMAT_TRACE "{date:%Y-%m-%d %H:%M:%S} TRACE   {file}:{line}, {func}: "

#define lg(level, format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_simple = [](){return lg::preformat(LOG_SIMPLE_LG format "{reset}\n");};                        \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_LG format "{reset}\n");};                        \
    if (level > lg::Logger::logger().log_level) break;                                                                 \
    if (lg::Logger::simple) {                                                                                          \
        lg::output(cx::to_string_view(make_simple),                                                                    \
            lg::Context(                                                                                               \
                stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                 \
            ), ##__VA_ARGS__                                                                                           \
        );                                                                                                             \
        break;                                                                                                         \
    }                                                                                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define lr(format, ...)                                                                                                \
do {                                                                                                                   \
    constexpr auto make_simple = [](){return lg::preformat(LOG_SIMPLE_LR format "{reset}\n");};                        \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_LR format "{reset}\n");};                        \
    if (lg::Logger::simple) {                                                                                          \
        lg::output(cx::to_string_view(make_simple),                                                                    \
            lg::Context(                                                                                               \
                stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                 \
            ), ##__VA_ARGS__                                                                                           \
        );                                                                                                             \
        break;                                                                                                         \
    }                                                                                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#ifndef NDEBUG
#define ld(level, format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_simple = [](){return lg::preformat(LOG_SIMPLE_LD format "{reset}\n");};                        \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_LD format "{reset}\n");};                        \
    if (level > lg::Logger::logger().debug_level) break;                                                               \
    if (lg::Logger::simple) {                                                                                          \
        lg::output(cx::to_string_view(make_simple),                                                                    \
            lg::Context(                                                                                               \
                stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                 \
            ), ##__VA_ARGS__                                                                                           \
        );                                                                                                             \
        break;                                                                                                         \
    }                                                                                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)
#else
#define ld(level, format, ...)
#endif

#define lw(format, ...)                                                                                                \
do {                                                                                                                   \
    constexpr auto make_simple = [](){return lg::preformat(LOG_SIMPLE_LW format "{reset}\n");};                        \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_LW format "{reset}\n");};                        \
    if (lg::Logger::simple) {                                                                                          \
        lg::output(cx::to_string_view(make_simple),                                                                    \
            lg::Context(                                                                                               \
                stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                 \
            ), ##__VA_ARGS__                                                                                           \
        );                                                                                                             \
        break;                                                                                                         \
    }                                                                                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define le(format, ...)                                                                                                \
do {                                                                                                                   \
    constexpr auto make_simple = [](){return lg::preformat(LOG_SIMPLE_LE format "{reset}\n");};                        \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_LE format "{reset}\n");};                        \
    if (lg::Logger::simple) {                                                                                          \
        lg::output(cx::to_string_view(make_simple),                                                                    \
            lg::Context(                                                                                               \
                stderr, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                 \
            ), ##__VA_ARGS__                                                                                           \
        );                                                                                                             \
        break;                                                                                                         \
    }                                                                                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stderr, lg::Logger::stderr_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define lf(format, ...)                                                                                                \
do {                                                                                                                   \
    constexpr auto make_simple = [](){return lg::preformat(LOG_SIMPLE_LF format "{reset}\n");};                        \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_LF format "{reset}\n");};                        \
    if (lg::Logger::simple) {                                                                                          \
        lg::output(cx::to_string_view(make_simple),                                                                    \
            lg::Context(                                                                                               \
                stderr, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                 \
            ), ##__VA_ARGS__                                                                                           \
        );                                                                                                             \
        break;                                                                                                         \
    }                                                                                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stderr, lg::Logger::stderr_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
    lg::Logger::logger().Fatal();                                                                                      \
} while (0)

#define log_level(level) lg::Logger::logger().Level(level)

#define log_any(format, ...)                                                                                           \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_ANY format "{reset}\n");};                       \
    if (lg::Logger::logger().level < lg::Levels::ANY) break;                                                           \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define log_fatal(format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_FATAL format "{reset}\n");};                     \
    if (lg::Logger::logger().level < lg::Levels::FATAL) break;                                                         \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stderr, lg::Logger::stderr_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
    lg::Logger::logger().Fatal();                                                                                      \
} while (0)

#define log_error(format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_ERROR format "{reset}\n");};                     \
    if (lg::Logger::logger().level < lg::Levels::ERROR) break;                                                         \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stderr, lg::Logger::stderr_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define log_warn(format, ...)                                                                                          \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_WARN format "{reset}\n");};                      \
    if (lg::Logger::logger().level < lg::Levels::WARN) break;                                                          \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define log_info(format, ...)                                                                                          \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_INFO format "{reset}\n");};                      \
    if (lg::Logger::logger().level < lg::Levels::INFO) break;                                                          \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#ifndef NDEBUG
#define log_debug(format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_DEBUG format "{reset}\n");};                     \
    if (lg::Logger::logger().level < lg::Levels::DEBUG) break;                                                         \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)
#else
#define log_debug(format, ...)
#endif

#ifndef NDEBUG
#define log_trace(format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(LOG_FORMAT_TRACE format "{reset}\n");};                     \
    if (lg::Logger::logger().level < lg::Levels::TRACE) break;                                                         \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)
#else
#define log_trace(format, ...)
#endif

#define log_print(format, ...)                                                                                         \
do {                                                                                                                   \
    constexpr auto make_format = [](){return lg::preformat(format);};                                                  \
    lg::output(cx::to_string_view(make_format),                                                                        \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
} while (0)

#define log_format(format, ...)                                                                                        \
[]() -> std::string {                                                                                                  \
    constexpr auto make_format = [](){return lg::preformat(format);};                                                  \
    return lg::format_to(cx::to_string_view(make_format),                                                              \
        lg::Context(                                                                                                   \
            stdout, lg::Logger::stdout_support_colors, __FILENAME__, __PRETTY_FUNCTION__, __LINE__                     \
        ), ##__VA_ARGS__                                                                                               \
    );                                                                                                                 \
}()

#ifndef NDEBUG
#define affirm(cond, ...) if (cond) lf("affirm: " #cond, ##__VA_ARGS__)
#else
#define affirm(cond, ...)
#endif
