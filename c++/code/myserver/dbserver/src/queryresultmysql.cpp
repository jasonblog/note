//
// Created by DGuco on 17-7-13.
//

#include "../inc/queryresultmysql.h"
#include "../../framework/base/my_assert.h"

QueryResultMysql::QueryResultMysql(MYSQL_RES *result,
                                   uint64 rowCount,
                                   uint32 fieldCount)
        : QueryResult(rowCount, fieldCount), mResult(result)
{

    mCurrentRow = new Field[mFieldCount];
    MY_ASSERT( mCurrentRow != NULL, return );

    MYSQL_FIELD *fields = mysql_fetch_fields(mResult);

    for (uint32 i = 0; i < mFieldCount; i++)
    {
        mFieldNames[i] = fields[i].name;
        mCurrentRow[i].SetType(ConvertNativeType(fields[i].type));
    }
}

QueryResultMysql::~QueryResultMysql()
{
    EndQuery();
}

bool QueryResultMysql::NextRow()
{
    MYSQL_ROW row;

    if (!mResult)
        return false;

    row = mysql_fetch_row(mResult);
    unsigned long *field_len = mysql_fetch_lengths(mResult);
    if (!row || !field_len)
    {
        EndQuery();
        return false;
    }

    for (uint32 i = 0; i < mFieldCount; i++)
    {
        mCurrentRow[i].SetValue(row[i], field_len[i]);
    }
    return true;
}

void QueryResultMysql::EndQuery()
{
    if (mCurrentRow)
    {
        delete [] mCurrentRow;
        mCurrentRow = 0;
    }

    if (mResult)
    {
        mysql_free_result(mResult);
        mResult = 0;
    }
}

enum Field::DataTypes QueryResultMysql::ConvertNativeType(enum_field_types mysqlType) const
{
    switch (mysqlType)
    {
        case FIELD_TYPE_TIMESTAMP:
        case FIELD_TYPE_DATE:
        case FIELD_TYPE_TIME:
        case FIELD_TYPE_DATETIME:
        case FIELD_TYPE_YEAR:
        case FIELD_TYPE_STRING:
        case FIELD_TYPE_VAR_STRING:
        case FIELD_TYPE_BLOB:
        case FIELD_TYPE_SET:
        case FIELD_TYPE_NULL:
            return Field::DB_TYPE_STRING;
        case FIELD_TYPE_TINY:

        case FIELD_TYPE_SHORT:
        case FIELD_TYPE_LONG:
        case FIELD_TYPE_INT24:
        case FIELD_TYPE_LONGLONG:
        case FIELD_TYPE_ENUM:
            return Field::DB_TYPE_INTEGER;
        case FIELD_TYPE_DECIMAL:
        case FIELD_TYPE_FLOAT:
        case FIELD_TYPE_DOUBLE:
            return Field::DB_TYPE_FLOAT;
        default:
            return Field::DB_TYPE_UNKNOWN;
    }
}

