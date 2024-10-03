#pragma once
#include <stdio.h>
#include "constants.h"

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

#define ERRORF(f, ...) printf(ERROR_COLOR "[ERROR]   " f COLOR_RESET, __VA_ARGS__)
#define ERROR(f) printf(ERROR_COLOR "[ERROR]   " f COLOR_RESET)
#define WARNF(f, ...) printf(WARNING_COLOR "[WARNING] " f COLOR_RESET, __VA_ARGS__)
#define WARN(f) printf(WARNING_COLOR "[WARNING] " f COLOR_RESET)
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
