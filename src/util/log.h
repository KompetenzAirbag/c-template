#pragma once

#include "util.h"

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

/* Logging library
   Provides useful logging tools such as DEBUG, WARN, VERBOSE, ERROR
   Defaults to stdout but can be redirected by #define "filename" BEFORE
   including this file
   Useful Makefile defines:
     DEBUG    enables  LOG_DEBUG
     VERBOSE  enables  LOG_VERBOSE
     NO_WARN  disables LOG_WARN
     NO_COLOR disables colorful logging */

#ifndef LOG_FILE
#define LOG_FILE stdout
#endif

#define AN_LOG_WALLCLOCK_CSTR_BUF_SZ (40ul)

#define CLOSE_STREAM( x ) \
    __builtin_choose_expr( \
        __builtin_types_compatible_p( typeof(x), FILE* ), \
        0, \
        1 \
    )


#define LOG_STREAM(x) \
    __builtin_choose_expr( \
        __builtin_types_compatible_p( typeof(x), FILE* ), \
        (FILE*)(x), \
        fopen( (char*)(x), "a+" ) \
    )

/* AN_WARN("%d is the loneliest number", 1) will print something like:
     [WARN] 09-02-2026 22:15:23.26 src/file.c(102): 1 is the loneliest number
*/
#define LOG_INFO( fmt, ... ) do { log_no_err( LOG_STREAM( LOG_FILE ), CLOSE_STREAM( LOG_FILE ), 0, __FILE__, __LINE__, fmt, ##__VA_ARGS__ ); } while( 0 )

#ifdef DEBUG
#define LOG_DEBUG( fmt, ... ) do { log_no_err( LOG_STREAM( LOG_FILE ), CLOSE_STREAM( LOG_FILE ), 1, __FILE__, __LINE__, fmt, ##__VA_ARGS__ ); } while( 0 )
#else
#define LOG_DEBUG( fmt, ... ) do {} while( 0 )
#endif /* DEBUG */

#ifdef VERBOSE
#define LOG_VERBOSE( fmt, ... ) do { log_no_err( LOG_STREAM( LOG_FILE ), CLOSE_STREAM( LOG_FILE ), 2, "", 0, fmt, ##__VA_ARGS__ ); } while( 0 )
#else
#define LOG_VERBOSE( fmt, ... ) do {} while( 0 )
#endif /* VERBOSE */

#ifdef NO_WARN
#define LOG_WARN( fmt, ... ) do {} while( 0 )
#else
#define LOG_WARN( fmt, ... ) do { log_no_err( LOG_STREAM( LOG_FILE ), CLOSE_STREAM( LOG_FILE ), 3, __FILE__, __LINE__, fmt, ##__VA_ARGS__ ); fflush( stdout ); } while( 0 )
#endif /* NO_WARN */

#define LOG_ERROR( fmt, ... ) do { log_err( LOG_STREAM( LOG_FILE ), CLOSE_STREAM( LOG_FILE ), __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__ ); } while( 0 )

/* log_no_err will log with any level without terminating the program */
void
log_no_err( FILE*       file_ptr,
            int         close_file,
            int         level,
            const char* file,
            int         line,
            const char* message_fmt,
            ... );

/* log_err will log an error and terminate the program */
void
log_err( FILE*       file_ptr,
         int         close_file,
         const char* file,
         int         line,
         const char* func,
         const char* message_fmt,
         ...
) __attribute__((noreturn)); /* Let compiler know this will not be returning */
