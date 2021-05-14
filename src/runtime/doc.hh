#pragma once

// Documentation generation related content

#include "object.hh"

// Generates a documentation recursively from an object
// Can throw
str_t autodoc(size_t indent, const str_t &name, const str_t &doc,
              const std::vector<std::pair<str_t, Object *>> &children);
