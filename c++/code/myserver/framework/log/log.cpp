#include <spdlog/spdlog.h>
#include "log.h"

// 初始一种类型的日志
int InitBaseLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				const char *vLogDir,                        /*文件名称(路径)*/
				level_enum level,        /*日志等级*/
				bool vAppend)
{
	if (NULL == vLogName || NULL == vLogDir) {
		return -1;
	}
#ifdef LOG_THREAD_SAFE
#ifdef _DEBUG_
	auto log = spdlog::stdout_logger_mt(vLogName);
#else
	auto log = spdlog::basic_logger_mt(vLogName, vLogDir, vAppend);
#endif
#else
#ifdef _DEBUG_
	auto log = spdlog::stdout_logger_st(vLogName);
#else
	auto log = spdlog::basic_logger_st(vLogName, vLogDir,vAppend);
#endif
#endif
	if (log == nullptr) {
		return -1;
	}
	log->set_level(level);
	log->flush_on(level);
	return 0;
}

int InitRoatingLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				   const char *vLogDir,                        /*文件名称(路径)*/
				   level_enum level,        /*日志等级*/
				   unsigned int vMaxFileSize,    /*回卷文件最大长度*/
				   unsigned int vMaxBackupIndex)           /*回卷文件个数*/
{
	if (NULL == vLogName || NULL == vLogDir) {
		return -1;
	}
#ifdef LOG_THREAD_SAFE
#ifdef _DEBUG_
	auto log = spdlog::stdout_logger_mt(vLogName);
#else
	auto log = spdlog::rotating_logger_mt(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
#endif
#else
#ifdef _DEBUG_
	auto log = spdlog::stdout_logger_st(vLogName);
#else
	auto log = spdlog::rotating_logger_st(vLogName, vLogDir, vMaxFileSize, vMaxBackupIndex);
#endif
#endif
	if (log == NULL) {
		return -1;
	}
	log->set_level(level);
	log->flush_on(level);
	return 0;
}

int InitDailyLog(const char *vLogName,                        /*日志类型的名称(关键字,由此定位到日志文件)*/
				 const char *vLogDir,                        /*文件名称(路径)*/
				 level_enum level,        /*日志等级*/
				 unsigned int hour,
				 unsigned int minute)
{
	if (NULL == vLogName || NULL == vLogDir) {
		return -1;
	}
#ifdef LOG_THREAD_SAFE
#ifdef _DEBUG_
	auto log = spdlog::stdout_logger_mt(vLogName);
#else
	auto log = ::daily_logger_mt(vLogName, vLogDir, hour, minute);
#endif
#else
#ifdef _DEBUG_
	auto log = spdlog::stdout_logger_st(vLogName);
#else
	auto log = spdlog::daily_logger_st(vLogName, vLogDir, hour, minute);
#endif
#endif
	if (log == NULL) {
		return -1;
	}
	log->set_level(level);
	log->flush_on(level);
	return 0;
}

int ShutdownAllLog()
{
	spdlog::drop_all();
	return 0;
}