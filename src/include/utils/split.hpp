#pragma once

namespace utils {

    std::vector<std::string> split(const std::string &str, char delimeter) {
        std::vector<std::string> array;
        std::string::size_type   prev_pos = 0, pos = 0;

        while ((pos = str.find(delimeter, pos)) != std::string::npos) {
            array.push_back(str.substr(prev_pos, pos - prev_pos));
            prev_pos = ++pos;
        }
        array.push_back(str.substr(prev_pos, pos - prev_pos));
        return array;
    }

}