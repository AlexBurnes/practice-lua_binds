#pragma once

#include "non_movable.hpp"
#include <tuple>

template <typename Callable, typename ...Args>
class deffer : public nonmovable {
    private:
        const Callable call;
        const std::tuple<Args& ...> args;
    public:
        deffer(Callable&& call, Args& ...args): call{call}, args{args...} {}
        deffer(const Callable& call, Args& ...args): call{call}, args{args...} {}
        ~deffer() {std::apply(call, args);}
};

