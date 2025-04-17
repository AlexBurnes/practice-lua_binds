#pragma once

namespace utils {

    class pad : public std::string {
        public:
            explicit pad(const char *s) : std::string(s) {}

            std::string operator*(int a) {
                std::string out{data()};
                while (--a > 0) out += data();
                return out;
            }
    };

}