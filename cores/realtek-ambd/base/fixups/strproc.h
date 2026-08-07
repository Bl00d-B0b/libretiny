/* Copyright (c) Bl00d-B0b 2026-08-07. */

#pragma once

// The SDK's swlib strproc.h defines isprint/isdigit/isspace/isupper/islower/
// isalpha as function-like macros. C++ <locale> calls std::isspace(c, loc)
// with two arguments, so any translation unit that sees both fails to compile.
// Shadow the header, pull the real one in, then drop the macros for C++.

#include_next <strproc.h>

#ifdef __cplusplus
#undef in_range
#undef isprint
#undef isdigit
#undef isxdigit
#undef isspace
#undef isupper
#undef islower
#undef isalpha
#endif
