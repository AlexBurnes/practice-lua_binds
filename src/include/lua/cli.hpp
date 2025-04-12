#pragma once

#include "lua/options.hpp"
#include "lua/module.hpp"

#include <thread/thread.hpp>

#include <sys/types.h>
#include <dirent.h>
#include <malloc.h>
#include <sys/mman.h>
#include <gperftools/malloc_extension.h>

#include <list>

#include <ev.h>
#include <functional>

#include "sparsepp/spp.h"

#include "lua/state.hpp"
#include "lua/bind.hpp"
#include <sol/sol.hpp>

#include <log/log.hpp>

extern "C" {

#include <stdlib/typedef.h>

}

//fixed EASTL error for C++20
#if __cplusplus >= 202002L
#define CHAR8_T_DEFINED
#endif

//#include <EASTL/string.h>
#include <EASTL/list.h>

template<typename T>
using list=eastl::list<T>;

/*
#include <list>
template<typename T>
using list=std::list<T>;
*/

#include <sparsepp/spp.h>
template<typename K, typename T>
using hash_map=spp::sparse_hash_map<K,T>;


//EASTL documenation recommends use void* __cdecl and new uint8_t[size]
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
    //return malloc(size);
    return new uint8_t[size];
}

void* operator new[](
    size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags,
    const char* file, int line
) {
    return memalign(alignment, size);
}

