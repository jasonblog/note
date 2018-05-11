//
// queryresult.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//

#ifndef SERVER_QUERYRESULT_H
#define SERVER_QUERYRESULT_H

#include "base.h"
#include <map>
#include <string>
#include <assert.h>
#include "../inc/field.h"

typedef std::map<uint32, std::string> FieldNames;

class QueryResult
{
public:
	QueryResult(uint64 rowCount, uint32 fieldCount)
		: mFieldCount(fieldCount), mRowCount(rowCount)
	{}

	virtual ~QueryResult()
	{}

	virtual bool NextRow() = 0;

	uint32 GetField_idx(const std::string &name) const
	{
		for (FieldNames::const_iterator iter = GetFiedNames().begin(); iter != GetFiedNames().end(); ++iter) {
			if (iter->second == name)
				return iter->first;
		}
		return uint32(-1);
	}

	Field *Fetch() const
	{
		return mCurrentRow;
	}

	const Field &operator[](int index) const
	{
		return mCurrentRow[index];
	}

	const Field &operator[](const std::string &name) const
	{
		return mCurrentRow[GetField_idx(name)];
	}

	uint32 GetFieldCount() const
	{
		return mFieldCount;
	}
	uint64 GetRowCount() const
	{
		return mRowCount;
	}

	FieldNames const &GetFiedNames() const
	{
		return mFieldNames;
	}

protected:
	Field *mCurrentRow;
	uint32 mFieldCount;
	uint64 mRowCount;
	FieldNames mFieldNames;
};

#endif //SERVER_QUERYRESULT_H
