#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <syslog.h>

// when openlog is 1 ,log will work
#define OPEN_LOG 1
#define TAG "WISP"
#define LOG_FILTER LOG_DEBUG
// #define LOG_FILTER LOG_ERR
//LOG_ERR


// extern debugconf_t debugconf;

/** Used to output messages.
 * The messages will include the filename and line number, and will be sent to syslog if so configured in the config file 
 * @param level Debug level
 * @param format... sprintf like format string
 */
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#define debug(level, format...) _debug(__FILENAME__, __LINE__, level, format)
#define dbg(format...) _debug(__FILENAME__, __LINE__, LOG_INFO, format)

/** @internal */
void _debug(const char *, int, int, const char *, ...);

#endif /* _DEBUG_H_ */
