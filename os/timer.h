//
// os/timer.h
//
// Functions for mapping ROM images into the address space.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#ifndef __os_timer_h__
#define __os_timer_h__
#define NS_PER_SEC 1000000000ULL
#include "common.h"

#if defined(CLOCK_MONOTONIC_PRECISE)
#define GETTIME_SOURCE CLOCK_MONOTONIC_PRECISE
#else
#define GETTIME_SOURCE CLOCK_MONOTONIC_RAW
#endif

#ifdef _WIN32
#include <windows.h>
typedef DWORD cen64_time;

#elif defined(__APPLE__)
#include <time.h>
typedef struct timeval cen64_time;

#else
#include <time.h>
typedef struct timespec cen64_time;
#endif

cen64_cold unsigned long long compute_time_difference(
  const cen64_time *now, const cen64_time *before);

cen64_cold void get_time(cen64_time *t);

#endif

