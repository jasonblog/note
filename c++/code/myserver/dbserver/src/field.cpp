//
// Created by DGuco on 17-7-13.
//

#include <stdio.h>
#include <cstring>
#include "../inc/field.h"

Field::Field() : mValue(NULL), mType(DB_TYPE_UNKNOWN)
{
}

Field::Field(Field &f)
{
    const char *value;

    value = f.GetString();

    if (value && (mValue = new char[strlen(value) + 1]))
        strcpy(mValue, value);
    else
        mValue = NULL;

    mType = f.GetType();
}

Field::Field(const char *value, enum Field::DataTypes type) : mType(type)
{
    if (value && (mValue = new char[strlen(value) + 1]))
        strcpy(mValue, value);
    else
        mValue = NULL;
}

Field::~Field()
{
    if(mValue)
    {
        delete [] mValue;
    }
}

void Field::SetValue(const char *value)
{
    if(mValue) {
        delete [] mValue;
    }

    if (value) {
        mValue = new char[strlen(value) + 1];
        strcpy(mValue, value);
    }
    else{
        mValue = NULL;
    }
}

void Field::SetValue(const void *value, uint16 value_len)
{
    delete [] mValue;
    if (value && value_len > 0)
    {
        mValue = new char[value_len+1];
        mValue[value_len] = 0;
        mValueLen = value_len;
        memcpy(mValue, value, value_len);
    }
    else
    {
        mValueLen = 0;
        mValue = NULL;
    }
}
