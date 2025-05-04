include_directories(include)

add_executable(
    luacpp
        main.cpp
        script.cpp
        file.cpp
        regexp.cpp
        lua/state.cpp
        lua/bind.cpp
        lua/bind/common.cpp
        lua/bind/data.cpp
        lua/bind/enums.cpp
        lua/bind/funcs.cpp
        lua/bind/types.cpp
        utils/split.cpp
)

target_link_libraries(
    luacpp
    PRIVATE
        sol2::sol2
        fmt::fmt
        tscns::tscns
        cxxopts::cxxopts
        pcre::pcre
)

add_executable(
    luabind
        lua_bind.cpp
)

target_link_libraries(
    luabind
    PRIVATE
        sol2::sol2
        fmt::fmt
        tscns::tscns
)

install(
    TARGETS
        luacpp
        luabind
    RUNTIME DESTINATION bin
)

add_subdirectory(test)
