#pragma once

#ifndef FUNCTION_NAME
#if defined(__GNUC__) || defined(__clang__)
#define FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define FUNCTION_NAME __FUNCSIG__ // Microsoft Visual C++ equivalent
#else
#define FUNCTION_NAME __func__ // Standard C++11 and later
#endif
#endif
