#pragma once

// Hash functions
// Exception safe (no throws)

#include "utils.hh"

// TODO : Set seed at runtime

// Returns the hash of an int
int_t hash_int(const int_t &val);

// Hash of a string
int_t hash_str(const str_t &val);

inline size_t hash_sz(const int_t &val) {
    // TODO Big Int : Update (hash again)
    return static_cast<size_t>(val);
}

// Hashes (base, val)
int_t hash_combine(const int_t &base, const int_t &val);

template <typename T>
inline int_t hash_ptr(const T *ptr) {
    return hash_int(int_t(reinterpret_cast<long long>(ptr)));
}

// Hashes a C++ iterator of int_t
// Returns 0 if empty
template <typename ForwardIterator>
int_t hash_iterator(
        ForwardIterator begin,
        ForwardIterator end) {
    if (begin == end) return 0;

    int_t base = *begin;
    ++begin;

    // Hash everything and update base
    while (begin != end) {
        base = hash_combine(base, *begin);

        ++begin;
    }

    return base;
}
