include_directories(include)

add_executable(
    luacpp
        main.cpp
        script.cpp
        file.cpp
        regexp.cpp
)

target_link_libraries(
    luacpp
    PRIVATE
        sol2::sol2
        fmt::fmt
        tscns::tscns
        cxxopts::cxxopts
)

add_executable(
    lua_bind
        lua_bind.cpp
)

target_link_libraries(
    lua_bind
    PRIVATE
        sol2::sol2
        fmt::fmt
        tscns::tscns
)

install(
    TARGETS
        luacpp
        lua_bind
    RUNTIME DESTINATION bin
)

add_subdirectory(test)