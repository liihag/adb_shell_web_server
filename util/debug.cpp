#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "debug.h"
#include <android/log.h>


/** @internal
Do not use directly, use the debug macro */
void
_debug(const char *filename, int line, int level, const char *format, ...)
{
#if OPEN_LOG
  char buf[28];
  va_list vlist;
  time_t ts;
  sigset_t block_chld;

  time(&ts);
  struct tm *p;
  p = localtime(&ts);
  char msg[1024] = "";
	char debug_info[768] = "";
//#输出日期，时间 文件 行数	
	// sprintf(msg,"[%d/%d %d:%d:%d](%s/%d):", p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,filename, line);
//#输出日期，时间，
//	sprintf(msg,"%d/%d %d:%d:%d:", p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	sprintf(msg,"%dd%d:%d:%d(%s/%d):", p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,filename, line);
//#输出文件，行数
//	sprintf(msg,"(%s/%d:", filename, line);
	
 	va_start(vlist, format);
	vsnprintf(debug_info,768,format,vlist);
	va_end(vlist);
	strcat(msg,debug_info);
	if(level == LOG_DEBUG&&LOG_FILTER > (LOG_DEBUG-1))
		__android_log_write(ANDROID_LOG_DEBUG,TAG,msg);	

	else if(level == LOG_ERR&&LOG_FILTER > (LOG_ERR-1))
		__android_log_write(ANDROID_LOG_ERROR,TAG,msg);

	else if(level == LOG_INFO&&LOG_FILTER > (LOG_INFO-1))
		__android_log_write(ANDROID_LOG_INFO,TAG,msg);

	else
	;

#else
;
#endif
	//sigprocmask(SIG_UNBLOCK, &block_chld, NULL);
	return ;
    
      
}
