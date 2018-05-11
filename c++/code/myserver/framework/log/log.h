//
//  log.h
//  Created by DGuco on 18/02/28.
//  Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef _LOG_H_
#define _LOG_H_
#include <string>
#include <stdarg.h>
#include <spdlog/spdlog.h>

using namespace std;
using namespace spdlog;
using namespace spdlog::level;

#	define INIT_BASE_LOG    InitBaseLog                        // 初始化基本的文件log
#	define INIT_ROATING_LOG    InitRoatingLog                        // 初始化一种日志类型（基于回卷文件)
#	define INIT_DAILY_LOG    InitDailyLog                        // 初始化一种日志类型（每天)
#	define LOG_SHUTDOWN_ALL        ShutdownAllLog()        // 关闭所有类型日志
#	define LOG_NOTICE                        LogTrace
#	define LOG_DEBUG                        LogDebug
#	define LOG_INFO                        LogInfo
#	define LOG_WARN                        LogWarn
#	define LOG_ERROR                        LogError
#	define LOG_FATAL                        LogCritical


int InitBaseLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				const char *vLogDir,                        /*文件名称(路径)*/
				level_enum level,        /*日志等级*/
				bool vAppend = true);                /*是否截断(默认即可)*/


int InitRoatingLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				   const char *vLogDir,                        /*文件名称(路径)*/
				   level_enum level,        /*日志等级*/
				   unsigned int vMaxFileSize = 10 * 1024 * 1024,    /*回卷文件最大长度*/
				   unsigned int vMaxBackupIndex = 5);            /*回卷文件个数*/

int InitDailyLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				 const char *vLogDir,                        /*文件名称(路径)*/
				 level_enum level,        /*日志等级*/
				 unsigned int hour,
				 unsigned int minute);

int ShutdownAllLog();

template<typename... Args>
int LogTrace(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LogDebug(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LogInfo(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LogWarn(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LogError(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int LogCritical(const char *vLogName, const char *vFmt, const Args &... args);
template<typename... Args>
int log(const char *vLogName, level_enum vPriority, const char *vFmt, const Args &... args);

template<typename... Args>
int LogTrace(const char *vLogName, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::trace, vFmt, args...);
	return 0;
}

template<typename... Args>
int LogDebug(const char *vLogName, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::debug, vFmt, args...);
	return 0;
}

template<typename... Args>
int LogInfo(const char *vLogName, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::info, vFmt, args...);
	return 0;
}

template<typename... Args>
int LogWarn(const char *vLogName, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::warn, vFmt, args...);
	return 0;
}

template<typename... Args>
int LogError(const char *vLogName, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}
	log->log(level_enum::err, vFmt, args...);
	return 0;
}

template<typename... Args>
int LogCritical(const char *vLogName, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(level_enum::critical, vFmt, args...);
	return 0;
}

template<typename... Args>
int log(const char *vLogName, level_enum vPriority, const char *vFmt, const Args &... args)
{
	if (NULL == vLogName) {
		return -1;
	}

	auto log = spdlog::get(vLogName);
	if (NULL == log) {
		return -1;
	}

	log->log(vPriority, vFmt, args...);
	return 0;
}

#endif // _LOG_H_
