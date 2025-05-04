#pragma once

//!@note define some types for binding to lua

#include "sparsepp/spp.h"
#include "utils/lvs.hpp"
#include "utils/pad.hpp"
#include "utils/split.hpp"

template <typename Key, typename Value>
using hash_map = spp::sparse_hash_map<Key, Value>;

template <typename T>
using unique_free_ptr = std::unique_ptr<T, void (*)(void*)>;
