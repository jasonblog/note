//
// queryresultmysql.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_QUERYRESULTMYSQL_H
#define SERVER_QUERYRESULTMYSQL_H

#include <mysql/mysql.h>

#include "../inc/basedb.h"
#include "../inc/queryresult.h"

class QueryResultMysql: public QueryResult
{
public:
	QueryResultMysql(MYSQL_RES *result, uint64 rowCount, uint32 fieldCount);

	~QueryResultMysql();

	bool NextRow();

private:
	enum Field::DataTypes ConvertNativeType(enum_field_types mysqlType) const;
	void EndQuery();

	MYSQL_RES *mResult;
};

#endif //SERVER_QUERYRESULTMYSQL_H
