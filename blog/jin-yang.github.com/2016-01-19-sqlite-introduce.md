---
title: SQLite 使用简介
layout: post
comments: true
language: chinese
category: [misc]
keywords: sqlite,c language,sample
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---


<!-- more -->

## 简介

### 安装

在 CentOS 中可以可以直接通过如下方式安装。

{% highlight text %}
# yum install sqlite
{% endhighlight %}

### 常见命令

{% highlight text %}
----- 直接新建一个文件名为foobar.db的数据库
$ sqlite foobar.db

----- 新建一个表
sqlite> create table foobar(id integer primary key, value text);

----- 直接插入部分数据
sqlite> insert into foobar(id, value) values(1, 'Micheal'), (2, 'Jenny'), (3, 'Francis');

----- 查看数据
sqlite> select * from foobar;
1|Micheal
2|Jenny
3|Francis

----- 设置查询返回的结果
sqlite> .mode column;                         # 按照列格式显示
sqlite> .header on;                           # 显示列名称
sqlite> select * from foobar;
id          value
----------- -------------
1           Micheal
2           Jenny
3           Francis

----- 添加列
sqlite> alter table foobar add column email text not null '' collate nocase;

----- 创建视图
sqlite> create view nameview as select value from foobar;
sqlite> select * from nameview;

----- 创建索引
sqlite> create index idx_value on foobar(value);

----- 查看帮助
sqlite> .help

----- 查看所有表，包括视图
sqlite> .tables

----- 显示表结构
sqlite> .schema [table|view]

----- 获取指定表的索引列表
sqlite > .indices [table]
idx_value

----- 导出数据库到SQL文件
sqlite > .output [filename ]
sqlite > .dump
sqlite > .output stdout

----- 备份、恢复数据库
$ sqlite foobar.db .dump > backup.sql
$ cat backup.sql | sqlite3 foobar-restore.db
$ sqlite3 foobar-restore.db < backup.sql
{% endhighlight %}

## C 编程

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define SQL_CREATE_TBL_DAEMON                                     \
	"CREATE TABLE IF NOT EXISTS daemon("                      \
		"name CHAR(64) PRIMARY KEY NOT NULL, "            \
		"version CHAR(64) NOT NULL, "                     \
		"gmt_modify NOT NULL DEFAULT CURRENT_TIMESTAMP, " \
		"gmt_create NOT NULL DEFAULT CURRENT_TIMESTAMP"   \
	");"

static int callback(void *non, int argc, char **argv, char **cols)
{
	(void) non;
	int i;

	for (i = 0; i < argc; i++)
		printf("%s = %s\n", cols[i], argv[i] ? argv[i] : "NULL");
	printf("\n");

	return 0;
}



int main(void)
{
	int rc;
	sqlite3 *db;
	sqlite3_stmt *res;
	char *sql, *errmsg;

	printf("Current SQLite version: %s\n", sqlite3_libversion());

	/* OR ":memory:" to create a memory database */
	rc = sqlite3_open("daemon.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	printf("Opened SQLite handle successfully.\n");

	rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	rc = sqlite3_step(res);
	if (rc == SQLITE_ROW)
		printf("Got version from SQL: %s\n", sqlite3_column_text(res, 0));
	sqlite3_finalize(res);

	/* "INSERT INTO XXXX VALUES(1, 'Audi', 52642);" */
	rc = sqlite3_exec(db, SQL_CREATE_TBL_DAEMON, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}
	printf("Create daemon table successfully.\n");

	sql = "INSERT INTO daemon(name, version) VALUES "
		"('MonitorAgent', 'V1.0.0'), ('SecurityAgent', 'V1.0.2');";
	rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}
        printf("Insert data successfully\n");
	int lastid = sqlite3_last_insert_rowid(db);
	printf("The last Id of the inserted row is %d\n", lastid);


	/* SELECT DATETIME(gmt_modify, "localtime") FROM daemon; */
	sql = "SELECT * FROM daemon";
	rc = sqlite3_exec(db, sql, callback, NULL, &errmsg);
	if (rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}

	char **result;
	int nrow, ncol;
	rc = sqlite3_get_table( db , sql , &result , &nrow , &ncol , &errmsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}
	for(rc = 0; rc < (nrow + 1 ) * ncol ; rc++ )
		printf( "result[%d] = %s\n", rc, result[rc]);
	sqlite3_free_table(result);


	rc = sqlite3_prepare_v2(db, "SELECT * FROM daemon", -1, &res, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	while((rc = sqlite3_step(res)) == SQLITE_ROW)
		printf("Got version from SQL: %s\n", sqlite3_column_text(res, 0));
		//printf("Got version from SQL: %d\n", sqlite3_column_int(res, 0));
	sqlite3_finalize(res);

	sql = "UPDATE daemon SET version = 'V2.0.1', gmt_modify = CURRENT_TIMESTAMP WHERE name = ?1;";
	rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	sqlite3_bind_text(res, 1, "MonitorAgent", -1, SQLITE_STATIC);
        //sqlite3_bind_int(res, 1, 3);
	rc = sqlite3_step(res);
	if (rc == SQLITE_ROW)
		printf("%s: ", sqlite3_column_text(res, 0));

	sql = "UPDATE daemon SET version = 'V2.0.1', gmt_modify = CURRENT_TIMESTAMP WHERE name = 'MonitorAgent';";
	rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
	if (rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}

	sql = "DELETE FROM daemon";
	rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
	if (rc != SQLITE_OK ) {
		fprintf(stderr, "SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}

	sqlite3_close(db);

	return 0;
}
{% endhighlight %}

## 参考

[SQLite C tutorial](http://zetcode.com/db/sqlitec)

{% highlight text %}
{% endhighlight %}
