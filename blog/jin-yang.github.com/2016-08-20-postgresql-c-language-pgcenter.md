---
title: PostgreSQL C 语言编程
layout: post
comments: true
language: chinese
category: [database,postgresql,linux]
keywords: libpq,c,语言,编程,pgcenter
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

对于 PostgreSQL 的 C 语言编程，可以直接使用 libpq 库，这里简单介绍其使用方法。

<!-- more -->

## libpq

在 CentOS 中，处理通过源码编译安装之外，还可以安装 `postgresqlXX-devel` 包，然后可以通过如下的方式进行编译。

{% highlight text %}
$ pg_config --includedir
$ pg_config --libdir
$ gcc -I/usr/pgsql-9.2/include/ -L/usr/pgsql-9.2/lib/ -lpq -o version version.c
{% endhighlight %}

如下是一个常见操作的示例集合。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

int main()
{
	int ver, i;
	PGconn *conn;
	PGresult *res;

	/* Get the libpq library version. */
	ver = PQlibVersion();
	printf("Libpq version: %d\n", ver);

	/* Connect to PostgreSQL with default user and database. */
	conn = PQconnectdb("user=postgres dbname=postgres");
	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}

	/* Get the connection infomation */
	printf("User: %s\n", PQuser(conn));
	printf("Database name: %s\n", PQdb(conn));
	printf("Password: %s\n", PQpass(conn));

	/* Get the server version */
	ver = PQserverVersion(conn);
	printf("Server version: %d\n", ver);

	/* Retrieve the server version again with different method */
	res = PQexec(conn, "SELECT VERSION()");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "No data retrieved\n");
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	printf("Current version: %s\n", PQgetvalue(res, 0, 0));
	PQclear(res);

	/* Create a table, and insert some data */
	res = PQexec(conn, "DROP TABLE IF EXISTS foobar");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Drop table failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	PQclear(res); /* Free the result */

	res = PQexec(conn, "CREATE TABLE foobar(id INTEGER PRIMARY KEY, name VARCHAR(32))");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Create table failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	PQclear(res);

	res = PQexec(conn, "INSERT INTO foobar VALUES(1, 'Audi'),(2, 'Volvo')");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Insert into table failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	PQclear(res);

	/* Retrieve multiple rows of data and also it's metadata */
	res = PQexec(conn, "SELECT * FROM foobar LIMIT 5");
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "No data retrieved\n");
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	int ncols = PQnfields(res);
	printf("There are %d columns:\n", ncols);
	for (i = 0; i < ncols; i++)
		printf("col #%d: %s\n", i, PQfname(res, i));
	for(i = 0; i < PQntuples(res); i++) /* another way */
		printf("line #%d: %s %s\n", i, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
	PQclear(res);


	/* Prepared statements */
	const char *params[1] = { "1" };
	char *stm = "SELECT * FROM foobar WHERE Id=$1";
	res = PQexecParams(conn, stm, 1, NULL, params, NULL, NULL, 0);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "No data retrieved\n");
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	printf("Data: %s %s\n", PQgetvalue(res, 0, 0), PQgetvalue(res, 0, 1));
	PQclear(res);

	/* Transaction */
	res = PQexec(conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "BEGIN command failed\n");
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	PQclear(res);

	res = PQexec(conn, "UPDATE foobar SET name='Mercedes' WHERE id=1");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "UPDATE command failed\n");
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	/* Maybe more statements */
	res = PQexec(conn, "COMMIT");
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "COMMIT command failed\n");
		PQclear(res);
		PQfinish(conn);
		exit(EXIT_FAILURE);
	}
	PQclear(res);

	PQfinish(conn);

	return 0;
}
{% endhighlight %}


<!--
## 其它

----- s kill running query
SELECT pg_cancel_backend(procpid);
-- kill idle query
SELECT pg_terminate_backend(procpid);

### 表膨胀

http://mysql.taobao.org/monthly/2015/12/07/
https://chenhuajun.github.io/2017/08/15/PostgreSQL%E7%9A%84%E8%A1%A8%E8%86%A8%E8%83%80%E5%8F%8A%E5%AF%B9%E7%AD%96.html
PostgreSQL 秒杀场景优化
https://yq.aliyun.com/articles/3010
PG压测
http://www.zhongweicheng.com/?p=3103

SELECT
    schemaname||'.'||relname as table_name,
    pg_size_pretty(pg_relation_size(schemaname||'.'||relname)) as table_size,
    n_dead_tup, n_live_tup,
    round(n_dead_tup * 100 / (n_live_tup + n_dead_tup), 2) AS dead_tup_ratio
FROM
    pg_stat_all_tables
WHERE
    n_dead_tup >= 1000
ORDER BY dead_tup_ratio DESC
LIMIT 10;

## 监控

#### 链接数

所有的当前链接信息保存在 `pg_stat_activity` 视图中，可以通过 `SELECT definition FROM pg_views WHERE viewname = 'pg_stat_activity';` 查看视图的定义。

https://www.postgresql.org/docs/9.2/static/monitoring-stats.html#PG-STAT-ACTIVITY-VIEW

----- 所有连接数及其状态
SELECT * FROM pg_stat_activity;
----- 查看最大连接数限制
SHOW max_connections;
----- 为超级用户保留的连接数
SHOW superuser_reserved_connections;

----- 链接状态统计(单个数据库还是整体的) ****
SELECT state, count(*) FROM pg_stat_activity GROUP BY state;
----- 获取最大链接数
SELECT setting::int FROM pg_settings WHERE name = 'max_connections';
----- 等待锁的链接数
SELECT count(*) FROM pg_stat_activity WHERE waiting = 'true';
----- TPS递增
select datname, xact_commit, xact_rollback, (xact_commit + xact_rollback) as all from pg_stat_database WHERE datname NOT IN ('TEMPLATE0', 'TEMPLATE1', 'POSTGRES') AND xact_commit > 0 AND xact_rollback > 0;



SELECT s.state,COUNT(datid) AS current FROM pg_stat_activity s GROUP BY 1 ORDER BY s.state

trans(dbname)   commit,rollback,total
connect percent,total,active,idle,idle_tx

获取当前长事务的最大执行时间
select max(extract(epoch from  now()-xact_start)) as interval from pg_stat_activity where state<>'idle' and now()-xact_start > interval '1 sec';




pg_stat_bgwriter

#### 数据库大小

----- 除去内部数据库之外的数据库大小
SELECT datname, pg_database_size(oid) size FROM pg_database WHERE datname NOT IN ('TEMPLATE0', 'TEMPLATE1', 'POSTGRES');


----- SLOW DML (长事务的最大时间)
SELECT count(*) from pg_stat_activity where state = 'active' and now() - query_start > '1 sec'::interval;

SELECT count(*) from pg_stat_activity where state = 'active' and now() - query_start > '$PARAM1 sec'::interval and query ~* '^(insert|update|delete)');
select count(*) from pg_stat_activity where state = 'active' and now() - query_start > '$PARAM1 sec'::interval and query ilike 'select%';


SELECT pg_relation_size(c.oid) AS rsize,
  pg_size_pretty(pg_relation_size(c.oid)) AS psize,
  relkind, relname, nspname
FROM pg_class c, pg_namespace n,pg_user u WHERE n.oid = c.relnamespace
and u.usesysid=c.relowner  AND u.usename = 'OPENSTACK' order by rsize desc limit 10;



https://www.postgresql.org/docs/9.3/static/monitoring.html
-->



## pgcenter

pgcenter 是一个 PG 终端的监控工具，但又不仅仅是监控工具，还提供了一些快速访问功能，例如编辑配置文件、重载配置、查看日志文件等。

可以直接查看 [github.com/lesovsky/pgcenter](https://github.com/lesovsky/pgcenter)，实际上很多的查询 SQL 是在 [pg-utils](https://github.com/PostgreSQL-Consulting/pg-utils/blob/master/sql/global_reports/query_stat_total.sql) 。

{% highlight text %}
----- 安装依赖库
# yum install ncurses-devel postgresql-devel
----- 直接编译、安装
# make && make install

----- 登陆查看当前的状态
$ pgcenter -h <host> -p <port> -U <user> -d <dbname>
$ pgcenter -h 127.1 -p 5432 -U postgres -d postgres
{% endhighlight %}

## 参考

另外常用的 C 语言组件还有 libzdb、pgpool (提供连接池+负载均衡)、pgbouncer (只有连接池)。

关于 C 语言编程，可以参考 [libpq](https://www.postgresql.org/docs/9.2/static/libpq.html) 、[libpq-example](https://www.postgresql.org/docs/9.2/static/libpq-example.html) 。

<!--
https://github.com/ty4z2008/Qix/blob/master/pg.md

## pg_activity

https://github.com/julmon/pg_activity
-->

{% highlight text %}
{% endhighlight %}
