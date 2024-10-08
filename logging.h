#pragma once
#include "constants.h"

#include <stdio.h>
#include <time.h>
#include <memory.h>
#include <errno.h>

#define COLOR_RESET "\x1b[0m"

#ifndef ERROR_COLOR
#define ERROR_COLOR "\x1b[31m"
#endif

#ifndef DEBUG_COLOR
#define DEBUG_COLOR "\x1b[36m"
#endif

#ifndef SUCCESS_COLOR
#define SUCCESS_COLOR "\x1b[32m"
#endif

#ifndef WARNING_COLOR
#define WARNING_COLOR "\x1b[33m"
#endif

#ifndef INFO_COLOR
#define INFO_COLOR "\x1b[34m"
#endif

#ifndef MAGENTIA
#define MAGENTIA "\x1b[35m"
#endif

extern uint32_t nerrs;
extern uint32_t nwarns;
static struct timespec ts;

#define ERRORF(f, ...) printf(ERROR_COLOR "[ERROR]   " f COLOR_RESET, __VA_ARGS__); ++nerrs
#define ERROR(f) printf(ERROR_COLOR "[ERROR]   " f COLOR_RESET); ++nerrs
#define WARNF(f, ...) printf(WARNING_COLOR "[WARNING] " f COLOR_RESET, __VA_ARGS__); ++nwarns
#define WARN(f) printf(WARNING_COLOR "[WARNING] " f COLOR_RESET); ++nwarns
#define INFOF(f, ...) printf(INFO_COLOR "[INFO]    " f COLOR_RESET, __VA_ARGS__)
#define INFO(f) printf(INFO_COLOR "[INFO]    " f COLOR_RESET)
#define SUCCESS(f) printf(SUCCESS_COLOR "[SUCCESS] " f COLOR_RESET)
#define SUCCESSF(f, ...) printf(SUCCESS_COLOR "[SUCCESS] " f COLOR_RESET, __VA_ARGS__)

#ifdef M_DEBUG
#define DEBUG(f) printf(DEBUG_COLOR "[DEBUG]   " f COLOR_RESET)
#define DEBUGF(f, ...) printf(DEBUG_COLOR "[DEBUG]   " f COLOR_RESET, __VA_ARGS__)
#else
#define DEBUG
#define DEBUGF
#endif

static inline void start_time() {
    nwarns = 0;
    nerrs = 0;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
}

static inline void stop_time_print_data() {
    struct timespec ets;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ets) != 0) {
        ERRORF("Failed to start clock, errno %d\n", errno);
        return;
    }

    ets.tv_sec -= ts.tv_sec;
    ets.tv_nsec -= ts.tv_nsec;

    uint32_t hours = ets.tv_sec / 3600,
             minutes = (ets.tv_sec % 3600) / 60;
    ets.tv_sec %= 60 * 3600;

    printf(MAGENTIA "\n | -----Execution Stats-----\n | Run Time: ");
    if (hours) printf("%dh ", hours);
    if (minutes) printf("%dn ", minutes);
    if (ets.tv_sec) printf("%lds ", ets.tv_nsec);
    printf("%.4f ms\n", ets.tv_nsec / 1.0e7);

    printf(" | Issues: ");
    if (nerrs > 0) printf(ERROR_COLOR);
    printf("%d" MAGENTIA " errors; ", nerrs);
    if (nwarns > 0) printf(WARNING_COLOR);
    printf("%d" MAGENTIA " warnings\n | \n\n", nwarns);
    
    printf(COLOR_RESET);
}

static inline struct timespec stop_time() {
    struct timespec ets;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ets) != 0) {
        memset(&ets, 0, sizeof(ets));
        ERRORF("Failed to start clock, errno %d\n", errno);
        return ets;
    }

    ets.tv_sec -= ts.tv_sec;
    ets.tv_nsec -= ts.tv_nsec;
    return ets;
}
