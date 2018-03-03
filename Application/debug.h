#ifndef __DEBUG_H__
#define __DEBUG_H__


void debug(const char *fmt, ...);


void debugPrint(const char *fmt, ...);

#define debug(...)      do { debugPrint("[%s][%d][%s] ", __FILE__, __LINE__, __FUNCTION__); debugPrint(__VA_ARGS__); debugPrint("\r\n"); } while (0)
#define debug_log(...)  do { debugPrint("[%s][%d][%s] ", __FILE__, __LINE__, __FUNCTION__); debugPrint(__VA_ARGS__); debugPrint("\r\n"); } while (0)

#endif