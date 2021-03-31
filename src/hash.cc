#include "hash.hh"
#include <functional>

using namespace std;

// Base seed for every hash
constexpr int_t HASH_SEED = 0x9e3779b9;

// Mask to avoid negative hashes
constexpr size_t HASH_MASK = 0x7fffffff'ffffffff;

int_t hash_int(const int_t &val) {
    constexpr hash<int_t> hfun;

    return int_t(hfun(val ^ HASH_SEED) & HASH_MASK);
}

int_t hash_str(const str_t &val) {
    constexpr hash<str_t> hfun;

    return int_t(hfun(val) & HASH_MASK);
}

int_t hash_combine(const int_t &base, const int_t &val) {
    return base ^ (hash_int(val) + HASH_SEED + (base << 6) + (base >> 2));
}
