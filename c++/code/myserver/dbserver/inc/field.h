//
// filed.h
// Created by DGuco on 17-7-13.
// Copyright © 2018年 DGuco. All rights reserved.
//


#ifndef SERVER_FILED_H
#define SERVER_FILED_H

#include "base.h"
#include "basedb.h"
#include <string>

class Field
{
public:

	enum DataTypes
	{
		DB_TYPE_UNKNOWN = 0x00,
		DB_TYPE_STRING = 0x01,
		DB_TYPE_INTEGER = 0x02,
		DB_TYPE_FLOAT = 0x03,
		DB_TYPE_BOOL = 0x04
	};

	Field();
	Field(Field &f);
	Field(const char *value, enum DataTypes type);
	~Field();

	enum DataTypes GetType() const
	{ return mType; }
	const char *GetString() const
	{ return mValue; }

	std::string GetCppString() const
	{
		return mValue ? mValue : "";
	}

	float GetFloat() const
	{ return mValue ? static_cast<float>(atof(mValue)) : 0; }
	bool GetBool() const
	{ return mValue ? atoi(mValue) > 0 : false; }
	int32 GetInt32() const
	{ return mValue ? static_cast<int32>(atol(mValue)) : 0; }
	uint8 GetUInt8() const
	{ return mValue ? static_cast<uint8>(atol(mValue)) : 0; }
	uint16 GetUInt16() const
	{ return mValue ? static_cast<uint16>(atol(mValue)) : 0; }
	int16 GetInt16() const
	{ return mValue ? static_cast<int16>(atol(mValue)) : 0; }
	uint32 GetUInt32() const
	{ return mValue ? static_cast<uint32>(atol(mValue)) : 0; }
	uint64 GetUInt64() const
	{ return mValue ? static_cast<uint64>(atoll(mValue)) : 0; }
	uint16 GetValueLen()
	{ return mValueLen; }

	void SetType(enum DataTypes type)
	{ mType = type; }

	void SetValue(const char *value);
	void SetValue(const void *value, uint16 value_len);

private:
	char *mValue;
	uint16 mValueLen;
	enum DataTypes mType;
};
#endif //SERVER_FILED_H
