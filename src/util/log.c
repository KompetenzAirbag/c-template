#include "log.h"
#include "gen_types.h"

#include <time.h>
#include <string.h>

FILE* LOG_FILE         = NULL;
FILE* LOG_FILE_INFO    = NULL;
FILE* LOG_FILE_DEBUG   = NULL;
FILE* LOG_FILE_VERBOSE = NULL;
FILE* LOG_FILE_WARN    = NULL;
FILE* LOG_FILE_ERROR   = NULL;

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

    snprintf( buf, LOG_WALLCLOCK_CSTR_BUF_SZ, ANSI_COLOR_MAGENTA"%02d-%02d-%d %02d:%02d:%02d.%03u"ANSI_COLOR_RESET, DD, MM, YYYY, hh, mm, ss, ms );
}

FILE*
get_log_file( LOG_LEVEL level )
{
    if( UNLIKELY( LOG_FILE == NULL ) ) {
        LOG_FILE = stdout;
    }

    switch( level ) {
        case LOG_LEVEL_INFO:    if( LOG_FILE_INFO != NULL )    return LOG_FILE_INFO; break;
        case LOG_LEVEL_DEBUG:   if( LOG_FILE_DEBUG != NULL )   return LOG_FILE_DEBUG; break;
        case LOG_LEVEL_VERBOSE: if( LOG_FILE_VERBOSE != NULL ) return LOG_FILE_VERBOSE; break;
        case LOG_LEVEL_WARN:    if( LOG_FILE_WARN != NULL )    return LOG_FILE_WARN; break;
        case LOG_LEVEL_ERROR:   if( LOG_FILE_ERROR != NULL )   return LOG_FILE_ERROR; break;
        default: {
            LOG_FILE = NULL;
            LOG_ERROR( "Log level %i not implemented", level );
        }
    }

    return LOG_FILE;
}

void
log_variadic( LOG_LEVEL   level,
              const char* file,
              int         line,
              const char* func,
              const char* message_fmt,
              va_list     args )
{
    FILE* log_file = get_log_file( level );

    char now_cstr[LOG_WALLCLOCK_CSTR_BUF_SZ];
    log_wallclock_cstr( get_millis(), now_cstr );

    static const char* const prefixes[] = {
        ANSI_COLOR_GREEN  "[INFO]   " ANSI_COLOR_RESET,
        ANSI_COLOR_CYAN   "[DEBUG]  " ANSI_COLOR_RESET,
        ANSI_COLOR_BLUE   "[VERBOSE]" ANSI_COLOR_RESET,
        ANSI_COLOR_YELLOW "[WARN]   " ANSI_COLOR_RESET,
        ANSI_COLOR_RED    "[ERROR]  " ANSI_COLOR_RESET
    };

    ENSURE_ERR( level < LOG_LEVEL_COUNT, "Provided level (%i) cannot exceed LOG_LEVEL_COUNT %i", level, LOG_LEVEL_COUNT );

    if( file == NULL || strlen( file ) == 0 ) {
        if( func == NULL || strlen( func ) == 0 ) {
            fprintf( log_file, "%s %s: ", prefixes[level], now_cstr );
        } else {
            fprintf( log_file, "%s %s "ANSI_COLOR_GREEN"in %s: "ANSI_COLOR_RESET, prefixes[level], now_cstr, func );
        }
    } else {
        if( func == NULL || strlen( func ) == 0 ) {
            fprintf( log_file, "%s %s "ANSI_COLOR_GREEN"%s(%d): "ANSI_COLOR_RESET, prefixes[level], now_cstr, file, line );
        } else {
            fprintf( log_file, "%s %s "ANSI_COLOR_GREEN"%s(%d) in %s: "ANSI_COLOR_RESET, prefixes[level], now_cstr, file, line, func );
        }
    }

    if( args != NULL ) vfprintf( log_file, message_fmt, args );
    else if( message_fmt != NULL ) fprintf( log_file, "%s", message_fmt );
    fprintf( log_file, "\n" );

    fflush( log_file );
}

FILE*
open_log_file( char* file_path )
{
    FILE* f = fopen( file_path, "w" );

    if( f == NULL ) {
        log_variadic( LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, "Failed to open log file", NULL );
        exit( 1 );
    }

    return f;
}

void
log_no_err( LOG_LEVEL   level,
            const char* file,
            int         line,
            const char* message_fmt,
            ... )
{
    va_list args;
    va_start( args, message_fmt );

    log_variadic( level, file, line, NULL, message_fmt, args );

    va_end( args );
}

void
log_err( const char* file,
         int         line,
         const char* func,
         const char* message_fmt,
         ... )
{
    va_list args;
    va_start( args, message_fmt );

    log_variadic( LOG_LEVEL_ERROR, file, line, func, message_fmt, args );

    va_end( args );

    exit(1);
}
