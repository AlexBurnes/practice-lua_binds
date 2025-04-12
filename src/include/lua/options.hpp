#pragma once

#include <string>
#include "lua/module.hpp"
#include <log/log.hpp>
#include <options/options.hpp>

#include "version.h"
// FIXME get target name from cmake
#ifndef NAME
#define NAME "lur-lua"
#endif


// set configuration option parameters
void option_config_file(int argc, const char *argv[]);
void option_cli_help(int argc, const char *argv[]);
void option_log_level(int argc, const char *argv[]);
void option_log_file(int argc, const char *argv[]);
void option_debug_level(int argc, const char *argv[]);
void option_log_quiet(int argc, const char *argv[]);
void option_usage(int argc, const char *argv[]);
void option_log_version(int argc, const char *argv[]);
void option_version(int argc, const char *argv[]);
void option_assume_yes(int argc, const char *argv[]);
void option_assume_no(int argc, const char *argv[]);
void about();
void version();

