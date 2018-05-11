//
// database.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include "log.h"
#include <string>
#include <string.h>

#define MAX_QUERY_LEN   1024

class QueryResult;

class Database
{
protected:
    Database();

public:

    virtual ~Database();

    bool InitLog(const char *vLogName,
                 const char *vLogDir,
                 level_enum vPriority = level_enum::trace,
                 unsigned int vMaxFileSize = 10 * 1024 * 1024,
                 unsigned int vMaxBackupIndex = 1,
                 bool vAppend = true);
    virtual bool Initialize(const char *infoString, int rw_timeout, int sleep_time, int loop);

    virtual QueryResult *Query(const char *sql, unsigned long len) = 0;
    virtual QueryResult *QueryForprocedure(const char *sql, unsigned long len, int number)=0;
    QueryResult *PQuery(const char *format, ...);

    virtual bool Execute(const char *sql) = 0;
    bool PExecute(const char *format, ...);
    virtual bool DirectExecute(const char *sql) = 0;
    virtual bool RealDirectExecute(const char *sql, unsigned long len) = 0;
    bool DirectPExecute(const char *format, ...);

    // Writes SQL commands to a LOG file (see mangosd.conf "LogSQL")
    bool PExecuteLog(const char *format, ...);

    virtual bool BeginTransaction()
    {
        return true;
    }
    virtual bool CommitTransaction()
    {
        return true;
    }
    virtual bool RollbackTransaction()
    {
        return false;
    }

    virtual operator bool() const = 0;

    virtual unsigned long escape_string(char *to, const char *from, unsigned long length)
    {
        strncpy(to, from, length);
        return length;
    }
    void escape_string(std::string &str);

    // must be called before first query in thread (one time for thread using one from existed Database objects)
    virtual void ThreadStart();
    // must be called before finish thread run (one time for thread using one from existed Database objects)
    virtual void ThreadEnd();

private:
    bool m_logSQL;
    std::string m_logsDir;

protected:
    std::string m_hostInfoString;
    std::string m_logsName;
};

#endif //SERVER_DATABASE_H
