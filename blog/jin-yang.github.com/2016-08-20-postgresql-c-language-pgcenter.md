---
title: PostgreSQL C 語言編程
layout: post
comments: true
language: chinese
category: [database,postgresql,linux]
keywords: libpq,c,語言,編程,pgcenter
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
---

對於 PostgreSQL 的 C 語言編程，可以直接使用 libpq 庫，這裡簡單介紹其使用方法。

<!-- more -->

## libpq

在 CentOS 中，處理通過源碼編譯安裝之外，還可以安裝 `postgresqlXX-devel` 包，然後可以通過如下的方式進行編譯。

{% highlight text %}
$ pg_config --includedir
$ pg_config --libdir
$ gcc -I/usr/pgsql-9.2/include/ -L/usr/pgsql-9.2/lib/ -lpq -o version version.c
{% endhighlight %}

如下是一個常見操作的示例集合。

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

### 表膨脹

http://mysql.taobao.org/monthly/2015/12/07/
https://chenhuajun.github.io/2017/08/15/PostgreSQL%E7%9A%84%E8%A1%A8%E8%86%A8%E8%83%80%E5%8F%8A%E5%AF%B9%E7%AD%96.html
PostgreSQL 秒殺場景優化
https://yq.aliyun.com/articles/3010
PG壓測
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

## 監控

#### 鏈接數

所有的當前鏈接信息保存在 `pg_stat_activity` 視圖中，可以通過 `SELECT definition FROM pg_views WHERE viewname = 'pg_stat_activity';` 查看視圖的定義。

https://www.postgresql.org/docs/9.2/static/monitoring-stats.html#PG-STAT-ACTIVITY-VIEW

----- 所有連接數及其狀態
SELECT * FROM pg_stat_activity;
----- 查看最大連接數限制
SHOW max_connections;
----- 為超級用戶保留的連接數
SHOW superuser_reserved_connections;

----- 鏈接狀態統計(單個數據庫還是整體的) ****
SELECT state, count(*) FROM pg_stat_activity GROUP BY state;
----- 獲取最大鏈接數
SELECT setting::int FROM pg_settings WHERE name = 'max_connections';
----- 等待鎖的鏈接數
SELECT count(*) FROM pg_stat_activity WHERE waiting = 'true';
----- TPS遞增
select datname, xact_commit, xact_rollback, (xact_commit + xact_rollback) as all from pg_stat_database WHERE datname NOT IN ('TEMPLATE0', 'TEMPLATE1', 'POSTGRES') AND xact_commit > 0 AND xact_rollback > 0;



SELECT s.state,COUNT(datid) AS current FROM pg_stat_activity s GROUP BY 1 ORDER BY s.state

trans(dbname)   commit,rollback,total
connect percent,total,active,idle,idle_tx

獲取當前長事務的最大執行時間
select max(extract(epoch from  now()-xact_start)) as interval from pg_stat_activity where state<>'idle' and now()-xact_start > interval '1 sec';




pg_stat_bgwriter

#### 數據庫大小

----- 除去內部數據庫之外的數據庫大小
SELECT datname, pg_database_size(oid) size FROM pg_database WHERE datname NOT IN ('TEMPLATE0', 'TEMPLATE1', 'POSTGRES');


----- SLOW DML (長事務的最大時間)
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

pgcenter 是一個 PG 終端的監控工具，但又不僅僅是監控工具，還提供了一些快速訪問功能，例如編輯配置文件、重載配置、查看日誌文件等。

可以直接查看 [github.com/lesovsky/pgcenter](https://github.com/lesovsky/pgcenter)，實際上很多的查詢 SQL 是在 [pg-utils](https://github.com/PostgreSQL-Consulting/pg-utils/blob/master/sql/global_reports/query_stat_total.sql) 。

{% highlight text %}
----- 安裝依賴庫
# yum install ncurses-devel postgresql-devel
----- 直接編譯、安裝
# make && make install

----- 登陸查看當前的狀態
$ pgcenter -h <host> -p <port> -U <user> -d <dbname>
$ pgcenter -h 127.1 -p 5432 -U postgres -d postgres
{% endhighlight %}

## 參考

另外常用的 C 語言組件還有 libzdb、pgpool (提供連接池+負載均衡)、pgbouncer (只有連接池)。

關於 C 語言編程，可以參考 [libpq](https://www.postgresql.org/docs/9.2/static/libpq.html) 、[libpq-example](https://www.postgresql.org/docs/9.2/static/libpq-example.html) 。

<!--
https://github.com/ty4z2008/Qix/blob/master/pg.md

## pg_activity

https://github.com/julmon/pg_activity
-->

{% highlight text %}
{% endhighlight %}
