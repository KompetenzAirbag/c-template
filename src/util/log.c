#include "log.h"
#include "gen_types.h"

#include <time.h>
#include <string.h>

#ifndef ANSI_COLOR_RED
#ifdef NO_COLOR
#define ANSI_COLOR_RED
#define ANSI_COLOR_GREEN
#define ANSI_COLOR_YELLOW
#define ANSI_COLOR_BLUE
#define ANSI_COLOR_MAGENTA
#define ANSI_COLOR_CYAN
#define ANSI_COLOR_RESET
#else
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#endif
#endif

void
log_wallclock_cstr( long now, char* buf )
{
    long _t  = now / 1000;
    time_t t = (time_t)_t;
    struct tm tm[1];
    if( !localtime_r( &t, tm ) ) LOG_ERROR( "Could not convert to localtime" );

    uint YYYY = (uint)(1900+tm->tm_year);
    uint MM   = (uint)(   1+tm->tm_mon );
    uint DD   = (uint)tm->tm_mday;
    uint hh   = (uint)tm->tm_hour;
    uint mm   = (uint)tm->tm_min;
    uint ss   = (uint)tm->tm_sec;
    uint ms   = (uint)(now % 1000);

    sprintf( buf, ANSI_COLOR_MAGENTA"%02d-%02d-%d %02d:%02d:%02d.%.2d"ANSI_COLOR_RESET, DD, MM, YYYY, hh, mm, ss, ms );
}

void
log_variadic( FILE*       file_ptr,
              int         close_file,
              uint        level,
              const char* file,
              int         line,
              const char* func,
              const char* message_fmt,
              va_list     args )
{
    if( file_ptr == NULL ) {
        file_ptr = stdout;
        close_file = 0;
        log_variadic( file_ptr, close_file, 3, NULL, 0, NULL, "Failed to open log file", NULL );
        abort();
    }

    char now_cstr[AN_LOG_WALLCLOCK_CSTR_BUF_SZ];
    log_wallclock_cstr( get_millis(), now_cstr );

    static const char* const prefixes[] = {
        ANSI_COLOR_GREEN  "[INFO]   " ANSI_COLOR_RESET
        ANSI_COLOR_CYAN   "[DEBUG]  " ANSI_COLOR_RESET,
        ANSI_COLOR_BLUE   "[VERBOSE]" ANSI_COLOR_RESET,
        ANSI_COLOR_YELLOW "[WARN]   " ANSI_COLOR_RESET,
        ANSI_COLOR_RED    "[ERROR]  " ANSI_COLOR_RESET
    };

    ENSURE( level < sizeof(prefixes)/sizeof(prefixes[0]), "Provided level (%i) cannot exceed LOG_LEVEL_COUNT %i", level, sizeof(prefixes)/sizeof(prefixes[0]) );

    if( file == NULL || strlen( file ) == 0 ) {
        if( func == NULL || strlen( func ) == 0 ) {
            fprintf( file_ptr, "%s %s: ", prefixes[level], now_cstr );
        } else {
            fprintf( file_ptr, "%s %s "ANSI_COLOR_GREEN"in %s: "ANSI_COLOR_RESET, prefixes[level], now_cstr, func );
        }
    } else {
        if( func == NULL || strlen( func ) == 0 ) {
            fprintf( file_ptr, "%s %s "ANSI_COLOR_GREEN"%s(%d): "ANSI_COLOR_RESET, prefixes[level], now_cstr, file, line );
        } else {
            fprintf( file_ptr, "%s %s "ANSI_COLOR_GREEN"%s(%d) in %s: "ANSI_COLOR_RESET, prefixes[level], now_cstr, file, line, func );
        }
    }

    if( args != NULL ) vfprintf( file_ptr, message_fmt, args );
    else if( message_fmt != NULL ) fprintf( file_ptr, "%s", message_fmt );
    fprintf( file_ptr, "\n" );

    if( close_file ) {
        if( file_ptr ) fclose( file_ptr );
    }
}

void
log_no_err( FILE*       file_ptr,
            int         close_file,
            int         level,
            const char* file,
            int         line,
            const char* message_fmt,
            ... )
{
    va_list args;
    va_start( args, message_fmt );

    log_variadic( file_ptr, close_file, level, file, line, NULL, message_fmt, args );

    va_end( args );
}

void
log_err( FILE*       file_ptr,
         int         close_file,
         const char* file,
         int         line,
         const char* func,
         const char* message_fmt,
         ... )
{
    va_list args;
    va_start( args, message_fmt );

    log_variadic( file_ptr, close_file, 4, file, line, func, message_fmt, args );

    va_end( args );

    abort();
}
