#pragma once

#include "gen_types.h"

/* USE WITH CAUTION */
#define LIKELY( cond )   __builtin_expect( !!(cond), 1L )
#define UNLIKELY( cond ) __builtin_expect( !!(cond), 0L )

#define ENSURE( cond, message, ... )     if( !(cond) ) { LOG_WARN( (message), ##__VA_ARGS__ ); }
#define ENSURE_ERR( cond, message, ... ) if( !(cond) ) { LOG_ERROR( (message), ##__VA_ARGS__ ); }

/* get_millis gets the time in milliseconds */
long
get_millis();
