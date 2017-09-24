#pragma once

#ifdef _MSC_VER
#define PLATFORM_WINDOWS
#elif __APPLE__
#define PLATFORM_MAC
#endif

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;

#ifdef PLATFORM_MAC
#include <cxxabi.h>
#include <stdint.h>
typedef long long int64;
typedef unsigned long long uint64;
#else
#include <limits.h>
#include <intrin.h>
typedef __int64 int64;
typedef unsigned __int64 uint64;
#endif

#define SAFE_RELEASE(p) if (p) { p->release(); p = nullptr; }
#define SAFE_DELETE(p) if (p) { delete p; p = nullptr; }
#define SAFE_ARRAY_DELETE(p) if (p) { delete [] p; p = nullptr; }
#define SAFE_COM_RELEASE(p) if (p) { p->Release(); p = nullptr; }
