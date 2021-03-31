#pragma once

// Debug functions

#include "utils.hh"

#ifdef DEBUG

// Display standard debug trace in stdout
void debug_info(const str_t &msg);

// Display warning message in stdout
void debug_warn(const str_t &msg);

// Display error message in stderr
void debug_err(const str_t &msg);

#else
#define debug_info(...)
#define debug_warn(...)
#define debug_err(...)
#endif
