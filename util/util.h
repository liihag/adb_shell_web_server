#ifndef _UTIL_H_
#define _UTIL_H_

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>



#include "debug.h"


int _System(const char * cmd, char *pRetMsg, int msg_len);
int _System_clear_command(const char * cmd, int cmdsize,char *pRetMsg, int msg_len);
int runshellcmd(char* result_buf,char* command);
void get_time(char *buf,int buf_size);
int get_time_int(void);
int get_msize(char *path);
int is_dir_exist(char* dirname );
time_t gettime_sec();
time_t get_boottime_sec();
int _Syetem_without_ret(const char * cmd);
#define BUF_SIZE 256



#endif

