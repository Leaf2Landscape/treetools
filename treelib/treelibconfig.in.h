// Copyright (c) 2017
// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
// ABN 41 687 119 230
//
// Author: Kazys Stepanas

//
// Project configuration header. This is a generated header; do not modify
// it directly. Instead, modify the config.h.in version and run CMake again.
//
#ifndef TREE_CONFIG_H
#define TREE_CONFIG_H

#include "treelib_export.h"

#define RASCORE_VERSION_MAJOR @rascore_VERSION_MAJOR@
#define RASCORE_VERSION_MINOR @rascore_VERSION_MINOR@
#define RASCORE_VERSION_PATCH @rascore_VERSION_PATCH@
#define RASCORE_VERSION "@rascore_VERSION_MAJOR@.@rascore_VERSION_MINOR@.@rascore_VERSION_PATCH@"

// MSVC does not define common maths constants like M_PI by default.
// Make sure they are defined if we include cmath
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif  // _USE_MATH_DEFINES
#ifndef NOMINMAX
// Disable MSVC min() and max() macros in favour of of function implementations.
#define NOMINMAX
#endif  // NOMINMAX
#ifndef NOMINMAX
#define NOMINMAX
#endif  // NOMINMAX

// Ensure M_PI is defined by including cmath.
#include <cmath>

#ifdef _MSC_VER
// Avoid dubious security warnings for plenty of legitimate code
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS 1
#endif  // _SCL_SECURE_NO_WARNINGS
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif  // _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 1
#endif  // _MSC_VER

#define GLM_ENABLE_EXPERIMENTAL
// TODO(KS): C++17 if enabled.
#define GLM_FORCE_CXX14
#define GLM_FORCE_INLINE
// #define GLM_FORCE_ALIGNED_GENTYPES
// #define GLM_FORCE_INTRINSICS // SimD
// #define GLM_FORCE_SWIZZLE
#define GLM_FORCE_XYZW_ONLY
#define GLM_FORCE_SIZE_T_LENGTH

#endif  // TREE_CONFIG_H
