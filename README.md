## Student Practice C++: Lua Binds

Experements with C++ [Lua](https://www.lua.org/) binds with [sol2](https://github.com/ThePhD/sol2) library.

## Author

Aleksey.Ozhigov <AlexBurnes@gmail.com>

## Version

0.1.0

## Task

* Write unit tests to test the correctness of the binding of arguments of C++ functions called from Lua.
* Checking code coverage with tests on Lua.
* Bind C++ objects and containers.
* Bind data by value, reference and pointers, smart pointers.
* Benchmark Lua functions, calls from Lua to C++, calls from C++ to Lua.
* Research analysis of other language binding: JavaScript [V8](https://v8.dev/docs/embed), [Squirrel](http://squirrel-lang.org/), [ChaiScript](https://chaiscript.com/), 
a lot of them [embedded-scripting-languages](https://github.com/dbohdan/embedded-scripting-languages).

## Change log

[CHANGELOG](CHANGELOG.md)

## Build

    docker build --tag lua_bind:latest --network host --progress=plain .

## Test coverage

Run docker container to see coverage

    docker run -d --rm --name lua_bind --publish 8081:80 practice_sort_keys

Open in a browser [Coverage](http://localhost:8081)

## Code style check

For style check used clang-format-19. 
Format described in style_format.txt, based on [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

    clang-format-19 --style="file:style_format.txt" source

## License

This work is distributed under the [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0), see [LICENSE](https://github.com:AlexBurnes/practice-lua_binds/blob/master/LICENSE) for more information


## Run lua sctipt

    ./bin/luacpp -d ./lua -t -c ./lua/luaunit.lua