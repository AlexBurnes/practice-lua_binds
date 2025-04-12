/*
    testing lua coroutine in multithreaded environment
*/

#include <log/log.hpp>
#include <sol/sol.hpp>
#include <mutex>
#include <thread>
#include <time.h>
#include <stdlib.h>

class Bench {
    std::chrono::high_resolution_clock::time_point start_, end_;
    std::string what_;
    const size_t records_;
    public:
        Bench(std::string_view what, const size_t records)
        : what_{what}, records_{records}
        {
            start_ = std::chrono::high_resolution_clock::now();
        };
        ~Bench() {
            end_ = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_ - start_).count();
            le(
                "{} {} in {}s avg {}ns {}r/s",
                what_, records_, duration, (duration / records_) * 1e9, 1 / (duration / records_)
            );
        };
};

//https://sol2.readthedocs.io/en/latest/exceptions.html
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

int check_script(sol::protected_function_result load_result) {
    if (not load_result.valid()) {
        sol::error err = load_result;
        le("failed load error: {}", err.what());
        return 1;
    }
    return 0;
}

class Task {
    public:
        Task(const int worker, const size_t test, sol::state* lua, std::mutex *mutex)
        : worker{worker}, test{test}, step{0},
          thread{sol::thread::create(lua->lua_state())}, state{thread.state()}, coroutine{state["main"]},
          mutex{mutex}
        {}
        ~Task() {}

        template<typename ...T>
        auto call(std::shared_ptr<Task> task, T&&... args) {
            mutex->lock();
            state["task"] = task;
            auto return_func = coroutine.call(args...);
            state["task"] = nullptr;
            mutex->unlock();
            return return_func;
        }

        const int worker;
        const size_t test;
        int step;

        sol::thread thread;
        sol::state_view state;
        sol::coroutine coroutine;

        std::mutex  *mutex;

};

int main() {

    lg::log_init(lg::Levels::ANY, 1, 1);

    size_t tests = 1e3;
    int workers = 10;

    sol::state lua;
    std::mutex mutex;
    lua.open_libraries(
        sol::lib::base, sol::lib::jit, sol::lib::string, sol::lib::coroutine, sol::lib::math, sol::lib::os
    );
    lua.set_exception_handler(&exception_handler);
    lua.set_function("throw", [](){throw std::runtime_error("exception!");});

    lua["lg_"]=[](std::shared_ptr<Task>& task, std::string_view msg){
        lg(1, "[thread:{}] [{}] step {}: {}", task->worker, task->test, task->step, msg);
    };

    check_script(lua.script(R"(
        function lg(fmt, ...)
            local msg = string.format(fmt, ...)
            lg_(task, msg)
        end
    )"));

    check_script(lua.script(R"(
        function main(step)
            task["step"] = step;
            lg("first step")
            task["step"] = coroutine.yield(task["step"])
            lg("second step")
            return
        end
    )"));

    {
        auto task_type = lua.new_usertype<Task>("Task");
        task_type.set("worker", sol::readonly(&Task::worker));
        task_type.set("test"  , sol::readonly(&Task::test));
        task_type.set("step"  , &Task::step);
    }

    auto fn = [tests](int worker, sol::state* lua, std::mutex* mutex) {
        size_t i = 0;
        size_t t = tests;
        while (i < t) {
            ++i;
            int step;
            timespec sleep = {0};

            {
                //create coroutine

                ld(1, "[thread:{}] [{}] step {}: make lua state", worker, i, 0);

                mutex->lock();
                auto task = std::make_shared<Task>(worker, i, lua, mutex);
                mutex->unlock();

                sleep.tv_nsec = rand() % 500;
                ld(1, "[thread:{}] [{}] step {}: sleep {}(ns)", task->worker, task->test, task->step, sleep.tv_nsec);
                nanosleep(&sleep, nullptr);

                //call coroutine
                ld(1, "[thread:{}] [{}] step {}: call first step", task->worker, task->test, task->step);
                task->call(task, 1);
                lg(1, "[thread:{}] [{}] step {}: return first step", task->worker, task->test, task->step);

                sleep.tv_nsec = rand() % 500;
                ld(1, "[thread:{}] [{}] step {}: sleep {}(ns)", task->worker, task->test, task->step, sleep.tv_nsec);
                nanosleep(&sleep, nullptr);

                //call yelded coroutine
                ld(1, "[thread:{}] [{}] step {}: call second step", task->worker, task->test, task->step);
                task->call(task, 2);
                lg(1, "[thread:{}] [{}] step {}: return second step", task->worker, task->test, task->step);

                //lock for destroy
                mutex->lock();
            }
            mutex->unlock();

        }
    };

    {

        Bench bench("lua", workers * tests);

        std::vector<std::thread *> threads;

        lg::quiet(true);

        for(int i = 0; i < workers; i++) {
            threads.push_back(new std::thread(fn, i, &lua, &mutex));
        }

        for(int i = 0; i < workers; i++) {
            threads[i]->join();
        }

        for(int i = 0; i < workers; i++) {
            delete threads[i];
        }

        lg::quiet(false);
    }

    lg(1, "done");

    return 0;
}
