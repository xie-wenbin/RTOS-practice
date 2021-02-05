#ifndef _VOS_LOG_H_
#define _VOS_LOG_H_
 /**
*****************************************************************
* @file: voslog.h
* @author: wen  
* @date:  1/11/2021
* @brief:  Interface for LOG access.
* @attention:
****************************************************************
 */


enum 
{
    VOS_LOG_ERROR    =  0,
    VOS_LOG_WARNING ,
    VOS_LOG_INFO  ,
    VOS_LOG_DEBUG,
    VOS_LOG_BUTT,
    VOS_LOG_NUM,
};


#define VOS_LOG(level,fmt, args...)   VOS_logger(level, __FILE__, __LINE__,fmt,##  args)

void VOS_SetLogLevel(int iLevel);

int VOS_logger(int level,char *filename,int lineno,const char* fmt,...);

#endif// _VOS_LOG_H_

