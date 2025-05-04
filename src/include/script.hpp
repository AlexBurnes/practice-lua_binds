#pragma once

#include <string>

typedef std::vector<std::pair<std::string, std::string>> script_dependencies_t;

int script_load(const std::string& file_name, const std::string& script_dir, bool test_script, bool show_coverage);
