#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include "util.h"
#include "debug.h"
#include "ctype.h"
  
/*
cmd  对应的指令
pRetMsg 返回值的指针
ret_msg_len 最长接收返回值的长度
返回值：
0 正常
-1  参数错误
-2  popen执行出错
 */
int _System(const char * cmd, char *pRetMsg, int ret_msg_len)  
{  
    FILE * fp;  
    char * p = NULL;  
    int res = -1;  
    if (cmd == NULL || pRetMsg == NULL || ret_msg_len < 0)  
    {  
        debug(LOG_ERR,"Param Error!\n");  
        return -1;  
    }  
    if ((fp = popen(cmd, "r") ) == NULL)  
    {  
        debug(LOG_ERR,"Popen Error!\n");  
        return -2;  
    }  
    else  
    {  
        memset(pRetMsg, 0, ret_msg_len);  
        //get lastest result  
        int once_len=ret_msg_len>64?64:ret_msg_len;
        int cycle=0;
        char once_msg_buff[64];
        while(
         fgets(once_msg_buff, once_len, fp)!= NULL)  
        {  
            strncat(pRetMsg,once_msg_buff,once_len);
            // debug(LOG_INFO,"_System Msg:%s",once_msg_buff); //print all info 
            if((++cycle +1)*once_len >= ret_msg_len) 
                break;
        }
        if ( (res = pclose(fp)) == -1)  
        {  
            debug(LOG_ERR,"close popenerror!\n");  
            return -3;  
        }
        if(cycle == 0){
            sprintf(pRetMsg,"shell dont return any result,maybe return errors!");
            debug(LOG_ERR,"shell dont return any result!\n");  
            return 0;
        }
        pRetMsg[strlen(pRetMsg)-1] = '\0';  
        return 0;  
    }  
}  
int _System_clear_command(const char * cmd, int cmdsize,char *pRetMsg, int ret_msg_len){
int  i =0;
i =_System(cmd,pRetMsg,ret_msg_len);
memset((char *)cmd,0,cmdsize);
return i;
}

int _Syetem_without_ret(const char * cmd){
char ret[256];
_System(cmd,ret,256);
return 0;
}


int get_dir_file_count(char* dir){
    struct dirent **ptr;
    return scandir(dir, &ptr, 0, 0); 
}

void get_time(char *buf,int buf_size){
    time_t raw_time;
    struct tm *tm;
    time(&raw_time);
    tm=localtime (&raw_time);
    strftime(buf,buf_size,"%m%d_%H%M%S",tm);

    debug(LOG_INFO,"now time %s\n",buf);
}

int get_time_int(void){
    char buf[256];
    time_t raw_time;
    struct tm *tm;
    time(&raw_time);
    tm=localtime (&raw_time);
    strftime(buf,256,"%m%d%H%M%S",tm);
    return atoi(buf);
}

time_t gettime_sec() {//注意这里用的是CLOCK_MONOTONIC
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec;
}

time_t get_boottime_sec(){
    struct timespec now;
    clock_gettime(CLOCK_BOOTTIME, &now);
    return now.tv_sec;
}

int get_msize(char *path){

    char command[256];
    char ret_msg[256];
    int  ret_size = 256;
    snprintf(command,256,"du -ms %s",path);
    _System(command,ret_msg,ret_size);


    int i;
    char really_size[64];
    for(i=0;i<sizeof(ret_msg);i++){
        if(isdigit(ret_msg[i])){
            really_size[i]=ret_msg[i];
        }else
            break;
    }

    debug(LOG_INFO,"the size of path %s is : %d M\n",path,atoi(really_size));
    return atoi(really_size);
}

//判断指定pid的proc是否存在
int is_dir_exist(char* dirname ){ 
    struct stat stat_buf;

    if(stat(dirname,&stat_buf)<0){
        debug(LOG_ERR,"can't find dir %s",dirname);
        return -1;
    }
    else
        return 1;
}