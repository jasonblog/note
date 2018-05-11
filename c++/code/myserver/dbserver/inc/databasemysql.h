//
// databasemysql.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_DATABASEMYSQL_H
#define SERVER_DATABASEMYSQL_H

#include "database.h"
#include <mysql/mysql.h>
#include <mysql/errmsg.h>

class QueryResult;

class DatabaseMysql: public Database
{
public:
	DatabaseMysql();
	~DatabaseMysql();

	//! Initializes Mysql and connects to a server.
	/*! infoString should be formated like hostname;username;password;database. */
	bool Initialize(const char *infoString, int rw_timeout, int sleep_time, int loop);
	bool Reconnect(void);
	bool Connect(void);
	QueryResult *Query(const char *sql, unsigned long len);
	QueryResult *QueryForprocedure(const char *sql, unsigned long len, int number);
	bool Execute(const char *sql);
	bool DirectExecute(const char *sql);
	bool RealDirectExecute(const char *sql, unsigned long len);
	bool BeginTransaction();
	bool CommitTransaction();
	bool RollbackTransaction();

	operator bool() const
	{ return mMysql != NULL; }

	unsigned long escape_string(char *to, const char *from, unsigned long length);
	using Database::escape_string;

	// must be call before first query in thread
	void ThreadStart();
	// must be call before finish thread run
	void ThreadEnd();

	int LastErrorNo() const
	{ return mysql_errno(mMysql); }

private:

	MYSQL *mMysql;

	static size_t db_count;

	bool _TransactionCmd(const char *sql);

	int m_connflag;
	int m_rwtimeout;
	int m_sleeptime;
	int m_loop;

	std::string m_host;
	std::string m_port_or_socket;
	std::string m_user;
	std::string m_password;
	std::string m_database;
};

#endif //SERVER_DATABASEMYSQL_H
