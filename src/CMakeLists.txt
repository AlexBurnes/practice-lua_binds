add_executable(
    luacpp
        main.cpp
)

target_link_libraries(
    luacpp
    PRIVATE
        sol2::sol2
        fmt::fmt
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
)

install(
    TARGETS
        luacpp
        lua_bind
    RUNTIME DESTINATION bin
)

add_subdirectory(test)