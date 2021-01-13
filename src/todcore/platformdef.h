#pragma once

#ifdef _MSC_VER
#define TOD_PLATFORM_WINDOWS
#elif __APPLE__
#define TOD_PLATFORM_MAC
#endif

#if defined(TOD_PLATFORM_WINDOWS)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <limits.h>
#include <intrin.h>
#define TOD_SNPRINTF sprintf_s
#else
#include <cxxabi.h>
#include <stdint.h>
#include <unistd.h>
#define TOD_SNPRINTF sprintf
#endif

#define TOD_SAFE_RELEASE(p) if (p) { p->release(); p = nullptr; }
#define TOD_SAFE_DELETE(p) if (p) { delete p; p = nullptr; }
#define TOD_SAFE_ARRAY_DELETE(p) if (p) { delete [] p; p = nullptr; }
#define TOD_SAFE_COM_RELEASE(p) if (p) { p->Release(); p = nullptr; }

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif