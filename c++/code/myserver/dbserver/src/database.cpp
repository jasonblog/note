//
// Created by DGuco on 17-7-13.
//

#include <ctime>
#include <iostream>
#include <fstream>
#include "../inc/queryresult.h"
#include "../inc/database.h"
#include "../inc/basedb.h"

Database::Database()
{

}

Database::~Database()
{
	/*Delete objects*/
}

bool Database::Initialize(const char *, int, int, int)
{
	// Enable logging of SQL commands (usally only GM commands)
	// (See method: PExecuteLog)
	m_logSQL = true;
	m_logsDir = std::string("../log/");
	if (!m_logsDir.empty()) {
		if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
			m_logsDir.append("/");
	}

	return true;
}

bool Database::InitLog(const char *vLogName,
					   const char *vLogDir,
					   level_enum vPriority,
					   unsigned int vMaxFileSize,
					   unsigned int vMaxBackupIndex,
					   bool vAppend)
{
	m_logsName = std::string(vLogName);
	return true;
}

void Database::ThreadStart()
{
}

void Database::ThreadEnd()
{
}

void Database::escape_string(std::string &str)
{
	if (str.empty())
		return;

	char *buf = new char[str.size() * 2 + 1];
	escape_string(buf, str.c_str(), str.size());
	str = buf;
	delete[] buf;
	buf = NULL;
}

bool Database::PExecuteLog(const char *format, ...)
{
	if (!format)
		return false;

	va_list ap;
	char szQuery[MAX_QUERY_LEN];
	va_start(ap, format);
	int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
	va_end(ap);

	if (res == -1) {
		LOG_ERROR(m_logsName.c_str(), "SQL Query truncated (and not execute) for format: {}", format);
		return false;
	}

	if (m_logSQL) {
		time_t curr;
		tm local;
		time(&curr);                                        // get current time_t value
		local = *(localtime(&curr));                          // dereference and assign
		char fName[128];
		sprintf(fName, "%04d-%02d-%02d_logSQL.sql", local.tm_year + 1900, local.tm_mon + 1, local.tm_mday);

		FILE *log_file;
		std::string logsDir_fname = m_logsDir + fName;
		log_file = fopen(logsDir_fname.c_str(), "a");
		if (log_file) {
			fprintf(log_file, "{};\n", szQuery);
			fclose(log_file);
		}
		else {
			// The file could not be opened
			LOG_ERROR(m_logsName.c_str(),
					  "SQL-Logging is disabled - Log file for the SQL commands could not be openend: {}",
					  fName);
		}
	}

	return Execute(szQuery);
}

QueryResult *Database::PQuery(const char *format, ...)
{
	if (!format) return NULL;

	va_list ap;
	char szQuery[MAX_QUERY_LEN];
	va_start(ap, format);
	int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
	va_end(ap);

	if (res == -1) {
		LOG_ERROR(m_logsName.c_str(), "SQL Query truncated (and not execute) for format: {}", format);
		return NULL;
	}

	return Query(szQuery, res);
}

bool Database::PExecute(const char *format, ...)
{
	if (!format)
		return false;

	va_list ap;
	char szQuery[MAX_QUERY_LEN];
	va_start(ap, format);
	int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
	va_end(ap);

	if (res == -1) {
		LOG_ERROR(m_logsName.c_str(), "SQL Query truncated (and not execute) for format: {}", format);
		return false;
	}

	return Execute(szQuery);
}

bool Database::DirectPExecute(const char *format, ...)
{
	if (!format)
		return false;

	va_list ap;
	char szQuery[MAX_QUERY_LEN];
	va_start(ap, format);
	int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
	va_end(ap);

	if (res == -1) {
		LOG_ERROR(m_logsName.c_str(), "SQL Query truncated (and not execute) for format: {}", format);
		return false;
	}

	return DirectExecute(szQuery);
}
